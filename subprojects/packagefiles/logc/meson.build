project('logc', 'c', meson_version: '>=1.1')

if get_option('logc_use_color')
    add_project_arguments('-DLOG_USE_COLOR', language: 'c')
endif

logc_inc = include_directories('src')
logc_sources = [
    'src/log.c',
]
logc_lib = static_library('logc', logc_sources, include_directories: logc_inc)

logc_dep = declare_dependency(
    include_directories: logc_inc,
    link_with: logc_lib,
)
