# SPDX-License-Identifier: BSD-3-Clause
# (c) Copyright IBM Corp. 2019, 2020

if not dpdk_conf.get('RTE_ARCH_64')
	error('Only 64-bit compiles are supported for this platform type')
endif
dpdk_conf.set('RTE_ARCH', 's390x')
dpdk_conf.set('RTE_ARCH_S390X', 1)
dpdk_conf.set('RTE_FORCE_INTRINSICS', 1)

# overrides specific to s390x
dpdk_conf.set('RTE_MAX_LCORE', 256)
dpdk_conf.set('RTE_MAX_NUMA_NODES', 32)
dpdk_conf.set('RTE_CACHE_LINE_SIZE', 128)



# default to z13
cpu_instruction_set = 'z13'

# test compiler support
cc_march_z14 = cc.has_argument('-march=z14')
cc_march_z15 = cc.has_argument('-march=z15')


machine_args = ['-march=' + cpu_instruction_set, '-mtune=' + cpu_instruction_set]

dpdk_conf.set('RTE_MACHINE','s390x')
dpdk_conf.set('RTE_MACHINE_CPUFLAG_ZARCH', 1)   # should this be z# 13 ?
#dpdk_conf.set('RTE_MACHINE', cpu_instruction_set)

if (cc.get_define('__s390x__', args: machine_args) != '')
    compile_time_cpuflags += ['RTE_MACHINE_CPUFLAG_ZARCH']
endif


# Suppress the gcc warning "note: the layout of aggregates containing
# vectors with 4-byte alignment has changed in GCC 5".
if (cc.get_id() == 'gcc' and cc.version().version_compare('>=10.0') and
        cc.version().version_compare('<12.0') and cc.has_argument('-Wno-psabi'))
    add_project_arguments('-Wno-psabi', language: 'c')
endif









