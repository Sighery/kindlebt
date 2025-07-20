project(
  'kindle-bt-api',
  'c',
  version: 'v1.0.0',
  default_options: [],
  meson_version: '>=1.1'
)

logc = subproject('logc')
logc_dep = logc.get_variable('logc_dep')

# Headers dependency
ace_bt_headers = subproject('ace_bt_headers')
ace_bt_headers_inc = ace_bt_headers.get_variable('ace_bt_inc')
ace_bt_headers_dep = declare_dependency(include_directories: ace_bt_headers_inc)

# Actual .so from the Kindle
ace_bt_pc_dep = dependency('ace_bt', method: 'pkg-config')

library_sources = files(
  './src/kindlebt.c',
  './src/kindlebt_log.c',
)
binary_sources = files(
  './src/main.c',
)

include_dirs = include_directories(
  './src/include/',
)

acebt_dep = declare_dependency(
  dependencies: [ace_bt_pc_dep, ace_bt_headers_dep],
)

kindlebt = shared_library(
  'kindlebt',
  library_sources,
  include_directories: include_dirs,
  dependencies: [
    acebt_dep,
    logc_dep,
  ],
)

kindle_root = get_option('kindle_root_dir')

# ACE libraries don't have soname so meson will create hardlinks
cleanup_ace_bt = custom_target(
  'clean up libace_bt hard link',
  depends: kindlebt,
  input: [
    join_paths(kindle_root, 'usr/lib', 'libace_bt.so'),
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
