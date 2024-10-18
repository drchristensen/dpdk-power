# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2023 PANTHEON.tech s.r.o.

"""Configuration dictionary contents specification.

These type definitions serve as documentation of the configuration dictionary contents.

The definitions use the built-in :class:`~typing.TypedDict` construct.
"""

from typing import TypedDict


class PortConfigDict(TypedDict):
    """Allowed keys and values."""

    #:
    pci: str
    #:
    os_driver_for_dpdk: str
    #:
    os_driver: str
    #:
    peer_node: str
    #:
    peer_pci: str


class TrafficGeneratorConfigDict(TypedDict):
    """Allowed keys and values."""

    #:
    type: str


class HugepageConfigurationDict(TypedDict):
    """Allowed keys and values."""

    #:
    number_of: int
    #:
    force_first_numa: bool


class NodeConfigDict(TypedDict):
    """Allowed keys and values."""

    #:
    hugepages_2mb: HugepageConfigurationDict
    #:
    name: str
    #:
    hostname: str
    #:
    user: str
    #:
    password: str
    #:
    arch: str
    #:
    os: str
    #:
    lcores: str
    #:
    use_first_core: bool
    #:
    ports: list[PortConfigDict]
    #:
    memory_channels: int
    #:
    traffic_generator: TrafficGeneratorConfigDict


class BuildTargetConfigDict(TypedDict):
    """Allowed keys and values."""

    #:
    arch: str
    #:
    os: str
    #:
    cpu: str
    #:
    compiler: str
    #:
    compiler_wrapper: str


class TestSuiteConfigDict(TypedDict):
    """Allowed keys and values."""

    #:
    suite: str
    #:
    cases: list[str]


class TestRunSUTConfigDict(TypedDict):
    """Allowed keys and values."""

    #:
    node_name: str
    #:
    vdevs: list[str]


class TestRunConfigDict(TypedDict):
    """Allowed keys and values."""

    #:
    build_targets: list[BuildTargetConfigDict]
    #:
    perf: bool
    #:
    func: bool
    #:
    skip_smoke_tests: bool
    #:
    test_suites: TestSuiteConfigDict
    #:
    system_under_test_node: TestRunSUTConfigDict
    #:
    traffic_generator_node: str
    #:
    random_seed: int


class ConfigurationDict(TypedDict):
    """Allowed keys and values."""

    #:
    nodes: list[NodeConfigDict]
    #:
    test_runs: list[TestRunConfigDict]
