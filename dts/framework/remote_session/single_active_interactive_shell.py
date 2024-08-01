# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2024 University of New Hampshire

"""Common functionality for interactive shell handling.

The base class, :class:`SingleActiveInteractiveShell`, is meant to be extended by subclasses that
contain functionality specific to that shell type. These subclasses will often modify things like
the prompt to expect or the arguments to pass into the application, but still utilize
the same method for sending a command and collecting output. How this output is handled however
is often application specific. If an application needs elevated privileges to start it is expected
that the method for gaining those privileges is provided when initializing the class.

This class is designed for applications like primary applications in DPDK where only one instance
of the application can be running at a given time and, for this reason, is managed using a context
manager. This context manager starts the application when you enter the context and cleans up the
application when you exit. Using a context manager for this is useful since it allows us to ensure
the application is cleaned up as soon as you leave the block regardless of the reason.

The :option:`--timeout` command line argument and the :envvar:`DTS_TIMEOUT`
environment variable configure the timeout of getting the output from command execution.
"""

from abc import ABC
from pathlib import PurePath
from typing import ClassVar

from paramiko import Channel, channel  # type: ignore[import-untyped]
from typing_extensions import Self

from framework.exception import (
    InteractiveCommandExecutionError,
    InteractiveSSHSessionDeadError,
    InteractiveSSHTimeoutError,
)
from framework.logger import DTSLogger, get_dts_logger
from framework.params import Params
from framework.settings import SETTINGS
from framework.testbed_model.node import Node


class SingleActiveInteractiveShell(ABC):
    """The base class for managing interactive shells.

    This class shouldn't be instantiated directly, but instead be extended. It contains
    methods for starting interactive shells as well as sending commands to these shells
    and collecting input until reaching a certain prompt. All interactive applications
    will use the same SSH connection, but each will create their own channel on that
    session.

    Interactive shells are started and stopped using a context manager. This allows for the start
    and cleanup of the application to happen at predictable times regardless of exceptions or
    interrupts.

    Attributes:
        is_alive: :data:`True` if the application has started successfully, :data:`False`
            otherwise.
    """

    _node: Node
    _stdin: channel.ChannelStdinFile
    _stdout: channel.ChannelFile
    _ssh_channel: Channel
    _logger: DTSLogger
    _timeout: float
    _app_params: Params
    _privileged: bool
    _real_path: PurePath

    #: The number of times to try starting the application before considering it a failure.
    _init_attempts: ClassVar[int] = 5

    #: Prompt to expect at the end of output when sending a command.
    #: This is often overridden by subclasses.
    _default_prompt: ClassVar[str] = ""

    #: Extra characters to add to the end of every command
    #: before sending them. This is often overridden by subclasses and is
    #: most commonly an additional newline character. This additional newline
    #: character is used to force the line that is currently awaiting input
    #: into the stdout buffer so that it can be consumed and checked against
    #: the expected prompt.
    _command_extra_chars: ClassVar[str] = ""

    #: Path to the executable to start the interactive application.
    path: ClassVar[PurePath]

    is_alive: bool = False

    def __init__(
        self,
        node: Node,
        privileged: bool = False,
        timeout: float = SETTINGS.timeout,
        app_params: Params = Params(),
        name: str | None = None,
    ) -> None:
        """Create an SSH channel during initialization.

        Args:
            node: The node on which to run start the interactive shell.
            privileged: Enables the shell to run as superuser.
            timeout: The timeout used for the SSH channel that is dedicated to this interactive
                shell. This timeout is for collecting output, so if reading from the buffer
                and no output is gathered within the timeout, an exception is thrown.
            app_params: The command line parameters to be passed to the application on startup.
            name: Name for the interactive shell to use for logging. This name will be appended to
                the name of the underlying node which it is running on.
        """
        self._node = node
        if name is None:
            name = type(self).__name__
        self._logger = get_dts_logger(f"{node.name}.{name}")
        self._app_params = app_params
        self._privileged = privileged
        self._timeout = timeout
        # Ensure path is properly formatted for the host
        self._update_real_path(self.path)

    def _setup_ssh_channel(self):
        self._ssh_channel = self._node.main_session.interactive_session.session.invoke_shell()
        self._stdin = self._ssh_channel.makefile_stdin("w")
        self._stdout = self._ssh_channel.makefile("r")
        self._ssh_channel.settimeout(self._timeout)
        self._ssh_channel.set_combine_stderr(True)  # combines stdout and stderr streams

    def _make_start_command(self) -> str:
        """Makes the command that starts the interactive shell."""
        start_command = f"{self._real_path} {self._app_params or ''}"
        if self._privileged:
            start_command = self._node.main_session._get_privileged_command(start_command)
        return start_command

    def _start_application(self) -> None:
        """Starts a new interactive application based on the path to the app.

        This method is often overridden by subclasses as their process for starting may look
        different. Initialization of the shell on the host can be retried up to
        `self._init_attempts` - 1 times. This is done because some DPDK applications need slightly
        more time after exiting their script to clean up EAL before others can start.

        Raises:
            InteractiveCommandExecutionError: If the application fails to start within the allotted
                number of retries.
        """
        self._setup_ssh_channel()
        self._ssh_channel.settimeout(5)
        start_command = self._make_start_command()
        self.is_alive = True
        for attempt in range(self._init_attempts):
            try:
                self.send_command(start_command)
                break
            except InteractiveSSHTimeoutError:
                self._logger.info(
                    f"Interactive shell failed to start (attempt {attempt+1} out of "
                    f"{self._init_attempts})"
                )
        else:
            self._ssh_channel.settimeout(self._timeout)
            self.is_alive = False  # update state on failure to start
            raise InteractiveCommandExecutionError("Failed to start application.")
        self._ssh_channel.settimeout(self._timeout)

    def send_command(
        self, command: str, prompt: str | None = None, skip_first_line: bool = False
    ) -> str:
        """Send `command` and get all output before the expected ending string.

        Lines that expect input are not included in the stdout buffer, so they cannot
        be used for expect.

        Example:
            If you were prompted to log into something with a username and password,
            you cannot expect ``username:`` because it won't yet be in the stdout buffer.
            A workaround for this could be consuming an extra newline character to force
            the current `prompt` into the stdout buffer.

        Args:
            command: The command to send.
            prompt: After sending the command, `send_command` will be expecting this string.
                If :data:`None`, will use the class's default prompt.
            skip_first_line: Skip the first line when capturing the output.

        Returns:
            All output in the buffer before expected string.

        Raises:
            InteractiveCommandExecutionError: If attempting to send a command to a shell that is
                not currently running.
            InteractiveSSHSessionDeadError: The session died while executing the command.
            InteractiveSSHTimeoutError: If command was sent but prompt could not be found in
                the output before the timeout.
        """
        if not self.is_alive:
            raise InteractiveCommandExecutionError(
                f"Cannot send command {command} to application because the shell is not running."
            )
        self._logger.info(f"Sending: '{command}'")
        if prompt is None:
            prompt = self._default_prompt
        out: str = ""
        try:
            self._stdin.write(f"{command}{self._command_extra_chars}\n")
            self._stdin.flush()
            for line in self._stdout:
                if skip_first_line:
                    skip_first_line = False
                    continue
                if line.rstrip().endswith(prompt):
                    break
                out += line
        except TimeoutError as e:
            self._logger.exception(e)
            self._logger.debug(
                f"Prompt ({prompt}) was not found in output from command before timeout."
            )
            raise InteractiveSSHTimeoutError(command) from e
        except OSError as e:
            self._logger.exception(e)
            raise InteractiveSSHSessionDeadError(
                self._node.main_session.interactive_session.hostname
            ) from e
        finally:
            self._logger.debug(f"Got output: {out}")
        return out

    def _close(self) -> None:
        self._stdin.close()
        self._ssh_channel.close()

    def _update_real_path(self, path: PurePath) -> None:
        """Updates the interactive shell's real path used at command line."""
        self._real_path = self._node.main_session.join_remote_path(path)

    def __enter__(self) -> Self:
        """Enter the context block.

        Upon entering a context block with this class, the desired behavior is to create the
        channel for the application to use, and then start the application.

        Returns:
            Reference to the object for the application after it has been started.
        """
        self._start_application()
        return self

    def __exit__(self, *_) -> None:
        """Exit the context block.

        Upon exiting a context block with this class, we want to ensure that the instance of the
        application is explicitly closed and properly cleaned up using its close method. Note that
        because this method returns :data:`None` if an exception was raised within the block, it is
        not handled and will be re-raised after the application is closed.

        The desired behavior is to close the application regardless of the reason for exiting the
        context and then recreate that reason afterwards. All method arguments are ignored for
        this reason.
        """
        self._close()
