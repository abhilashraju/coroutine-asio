project('coroutine', 'cpp',
  version: '0.1',
  default_options: ['cpp_std=c++20'])

# Specify the source files
sources = ['main.cpp']

# Add dependencies
nlohmann_json_dep = dependency('nlohmann_json', required: true)
threads_dep = dependency('threads')  # Add threading support
# Define the executable
executable('coroutineex', sources,
  dependencies: [nlohmann_json_dep,threads_dep])