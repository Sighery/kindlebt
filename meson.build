project(
  'kindle-bt-api',
  'cpp',
  version: 'v1.0.0',
  default_options: ['cpp_std=c++17'],
  meson_version: '>=1.1'
)

cc = meson.get_compiler('cpp')

kindle_root = get_option('kindle_root_dir')
acebt_dep = declare_dependency(
  dependencies: cc.find_library('ace_bt', dirs: join_paths(kindle_root, 'usr/lib'), required: true)
)

sources = files(
    './src/main.cpp',
)

include_dirs = include_directories(
  './src/include/',
)

executable(
  'kindle_bt_api',
  sources,
  include_directories: include_dirs,
  dependencies: [
    acebt_dep,
  ],
  cpp_args: '-static-libstdc++',
  link_args: ['-static-libstdc++'],
)
