project(
    'CPP Set example',
    'cpp',
    default_options: {
        'cpp_std': 'c++23',
        'warning_level': '3',
        'werror': true,
    },
    version: '2.0.0',
    meson_version: '>=1.2.0',
)

inc_dirs = include_directories('classes')


cpp_set_dep = declare_dependency(
    include_directories: inc_dirs,
    version: meson.project_version(),
)

executable('main', files('main.cpp'), include_directories: inc_dirs)


if get_option('tests')
    subdir('tests')
endif


