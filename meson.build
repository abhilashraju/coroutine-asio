project('coroutine', 'cpp',
  version: '0.1',
  default_options: ['cpp_std=c++23'])



# Add dependencies
nlohmann_json_dep = dependency('nlohmann_json', required: true)
threads_dep = dependency('threads')  # Add threading support
coroutine_dep = declare_dependency(
  include_directories: include_directories('include')
)

subdir('examples')
