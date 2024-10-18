

# Specify the source files
sources = ['main.cpp']

# Add dependencies

libmctp_dep = dependency('mctp_intel', required: true)  # Add libmctp support

# Define the executable
executable('coroutineex', sources,
  dependencies: [nlohmann_json_dep,threads_dep,libmctp_dep,coroutine_dep])