project(
  'kindlebt',
  'c',
  version: 'v0.1.0',
  default_options: [
    'buildtype=debug',
  ],
  meson_version: '>=1.1'
)

add_project_arguments('-fno-omit-frame-pointer', language: 'c')

if get_option('force_old_api')
  add_project_arguments('-DFORCE_OLD_API', language: 'c')
endif

logc = subproject('logc')
logc_dep = logc.get_variable('logc_dep')

# Headers dependency
darkroot = subproject('darkroot')
darkroot_inc = darkroot.get_variable('darkroot_inc')
darkroot_dep = declare_dependency(include_directories: darkroot_inc)

# Actual .so from the Kindle
ace_bt_pc_dep = dependency('ace_bt', method: 'pkg-config', static: false)

library_sources = files(
  './src/kindlebt.c',
  './src/kindlebt_log.c',
  './src/compat_ace.c',
  './src/compat_ace_implementations.c',
  './src/compat_ace_shims.c',
  './src/compat_ace_utils.c',
  './src/compat_ace_handler.c',
  './src/compat_acealloc.c',
)

include_dirs = include_directories(
  './include/',
)

acebt_dep = declare_dependency(
  dependencies: [ace_bt_pc_dep, darkroot_dep],
)

# Needed for the acebt ABI introspection
dl_dep = dependency('dl')

kindlebt = shared_library(
  'kindlebt',
  library_sources,
  include_directories: include_dirs,
  dependencies: [
    acebt_dep,
    logc_dep,
    dl_dep,
  ],
)

kindlebt_static = static_library(
  'kindlebt',
  library_sources,
  include_directories: include_dirs,
  dependencies: [
    acebt_dep,
    logc_dep,
    dl_dep,
  ],
  install: true,
)

# Defined in cross file properties
sysroot = meson.get_external_property('sys_root')

# ACE libraries don't have soname so meson will create hardlinks
cleanup_ace_bt = custom_target(
  'libkindlebt: clean up libace_bt hard link',
  depends: kindlebt,
  input: [
    join_paths(sysroot, 'usr/lib', 'libace_bt.so'),
    kindlebt,
  ],
  output: 'fake1',
  command: ['patchelf', '--replace-needed', '@INPUT0@', 'libace_bt.so', '@INPUT1@'],
  build_by_default: true,
)

# Copy the logc archive into the root build dir
logc_lib = logc.get_variable('logc_lib')
copy_logc = custom_target(
  'liblogc.a: copy to root build dir',
  depends: logc_lib,
  input: logc_lib,
  output: 'liblogc.a',
  command: ['cp', '@INPUT@', '@OUTPUT@'],
  build_by_default: true,
)
