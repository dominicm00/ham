cmake_minimum_required(VERSION 3.12...3.21)

# set the project name and version
project(
  Ham
  VERSION 0.1
  DESCRIPTION "A modern replacement for the Jam build system"
  LANGUAGES CXX
  )

# configuration info
configure_file(src/HamConfig.hpp.in HamConfig.hpp)

# PEGTL
add_subdirectory(extern/pegtl)

# headers
add_subdirectory(include)

# Ham library
add_subdirectory(src)

# Ham binary
add_subdirectory(cmd)
