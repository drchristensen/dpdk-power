# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2017-2019 Intel Corporation

apps = [
        'dumpcap',
        'pdump',
        'proc-info',
        'test-acl',
        'test-bbdev',
        'test-cmdline',
        'test-compress-perf',
        'test-crypto-perf',
        'test-eventdev',
        'test-fib',
        'test-flow-perf',
        'test-gpudev',
        'test-pipeline',
        'test-pmd',
        'test-regex',
        'test-sad',
]

default_cflags = machine_args + ['-DALLOW_EXPERIMENTAL_API']
default_ldflags = []
if get_option('default_library') == 'static' and not is_windows
    default_ldflags += ['-Wl,--export-dynamic']
endif

foreach app:apps
    build = true
    name = app
    sources = []
    includes = []
    cflags = default_cflags
    ldflags = default_ldflags
    objs = [] # other object files to link against, used e.g. for
              # instruction-set optimized versions of code

    # use "deps" for internal DPDK dependencies, and "ext_deps" for
    # external package/library requirements
    ext_deps = []
    deps = []

    subdir(name)

    if build
        dep_objs = []
        foreach d:deps
            var_name = get_option('default_library') + '_rte_' + d
            if not is_variable(var_name)
                build = false
                message('Missing dependency "@0@" for app "@1@"'.format(d, name))
                break
            endif
            dep_objs += [get_variable(var_name)]
        endforeach
    endif

    if not build
        continue
    endif

    link_libs = []
    if get_option('default_library') == 'static'
        link_libs = dpdk_static_libraries + dpdk_drivers
    endif

    # DRC - Start
    message('DRC - cflags = @0@'.format(cflags))
    message('DRC - ldflags = @0@'.format(ldflags))
    build_libs_rpath = '/opt/at15.0/lib64'
    # DRC - End

    executable('dpdk-' + name,
            sources,
            c_args: cflags,
            link_args: ldflags,
            link_whole: link_libs,
            dependencies: base_dep_objs + ext_deps + dep_objs,
            include_directories: includes,
            build_rpath : build_libs_rpath,
            # install_rpath: join_paths(get_option('prefix'), driver_install_path),
            install: true)
endforeach

# special case the autotests
subdir('test')
