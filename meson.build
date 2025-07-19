project(
  'kindle-bt-api',
  'c',
  version: 'v1.0.0',
  default_options: [],
  meson_version: '>=1.1'
)

logc = subproject('logc')
logc_dep = logc.get_variable('logc_dep')

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

acebt_dep = dependency('ace_bt')
ace_osal = dependency('ace_osal')
pthread_dep = dependency('pthread')

kindlebt = shared_library(
  'kindlebt',
  library_sources,
  include_directories: include_dirs,
  dependencies: [
    acebt_dep,
    ace_osal,
    pthread_dep,
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

cleanup_ace_osal = custom_target(
  'clean up libace_osal hard link',
  depends: cleanup_ace_bt,
  input: [
    join_paths(kindle_root, 'usr/lib', 'libace_osal.so'),
    kindlebt,
  ],
  output: 'fake2',
  command: ['patchelf', '--replace-needed', '@INPUT0@', 'libace_osal.so', '@INPUT1@'],
  build_by_default: true,
)

kindlebt_test = executable(
  'kindlebt_test',
  binary_sources,
  include_directories: include_dirs,
  dependencies: [
    logc_dep,
  ],
  link_with: [
    kindlebt,
  ],
  c_args: '-static-libgcc',
  link_args: ['-static-libgcc'],
)
