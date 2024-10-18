# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2010-2014 Intel Corporation

"""The DPDK hello world app test suite.

Run the helloworld example app and verify it prints a message for each used core.
No other EAL parameters apart from cores are used.
"""

from framework.remote_session.dpdk_shell import compute_eal_params
from framework.test_suite import TestSuite, func_test
from framework.testbed_model.capability import TopologyType, requires
from framework.testbed_model.cpu import (
    LogicalCoreCount,
    LogicalCoreCountFilter,
    LogicalCoreList,
)


@requires(topology_type=TopologyType.no_link)
class TestHelloWorld(TestSuite):
    """DPDK hello world app test suite."""

    def set_up_suite(self) -> None:
        """Set up the test suite.

        Setup:
            Build the app we're about to test - helloworld.
        """
        self.app_helloworld_path = self.sut_node.build_dpdk_app("helloworld")

    @func_test
    def hello_world_single_core(self) -> None:
        """Single core test case.

        Steps:
            Run the helloworld app on the first usable logical core.
        Verify:
            The app prints a message from the used core:
            "hello from core <core_id>"
        """
        # get the first usable core
        lcore_amount = LogicalCoreCount(1, 1, 1)
        lcores = LogicalCoreCountFilter(self.sut_node.lcores, lcore_amount).filter()
        eal_para = compute_eal_params(self.sut_node, lcore_filter_specifier=lcore_amount)
        result = self.sut_node.run_dpdk_app(self.app_helloworld_path, eal_para)
        self.verify(
            f"hello from core {int(lcores[0])}" in result.stdout,
            f"helloworld didn't start on lcore{lcores[0]}",
        )

    @func_test
    def hello_world_all_cores(self) -> None:
        """All cores test case.

        Steps:
            Run the helloworld app on all usable logical cores.
        Verify:
            The app prints a message from all used cores:
            "hello from core <core_id>"
        """
        # get the maximum logical core number
        eal_para = compute_eal_params(
            self.sut_node, lcore_filter_specifier=LogicalCoreList(self.sut_node.lcores)
        )
        result = self.sut_node.run_dpdk_app(self.app_helloworld_path, eal_para, 50)
        for lcore in self.sut_node.lcores:
            self.verify(
                f"hello from core {int(lcore)}" in result.stdout,
                f"helloworld didn't start on lcore{lcore}",
            )
