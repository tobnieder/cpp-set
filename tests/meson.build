test_deps = [cpp_set_dep]
test_src = []

test_deps += dependency('gtest')


test_src += files('entry.cpp', 'unordered_set.cpp')



all_tests = executable(
    'all_tests',
    test_src,
    dependencies: test_deps,
)


test('all tests', all_tests, protocol: 'gtest')
