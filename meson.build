project(
  'kindlebt',
  'c',
  version: 'v1.0.0',
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
ace_bt_pc_dep = dependency('ace_bt', method: 'pkg-config')

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
binary_sources = files(
  './src/main.c',
)

include_dirs = include_directories(
  './src/include/',
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

kindlebt_test = executable(
  'kindlebt_test',
  binary_sources,
  include_directories: include_dirs,
  dependencies: [
    acebt_dep,
    logc_dep,
  ],
  link_with: [
    kindlebt,
  ],
  c_args: '-static-libgcc',
  link_args: ['-static-libgcc'],
)

cleanup_kindlebt_test = custom_target(
  'kindlebt_test: clean up libace_bt hard link',
  depends: kindlebt_test,
  input: [
    join_paths(sysroot, 'usr/lib', 'libace_bt.so'),
    kindlebt_test,
  ],
  output: 'fake2',
  command: ['patchelf', '--replace-needed', '@INPUT0@', 'libace_bt.so', '@INPUT1@'],
  build_by_default: true,
)
