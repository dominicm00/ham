cmake_minimum_required(VERSION 3.12...3.21)

# set the project name
project(ham)

# add the executable
add_executable(ham src/ham.cpp)

# set the project name and version
project(ham VERSION 0.1)
configure_file(src/HamConfig.hpp.in HamConfig.hpp)
target_include_directories(ham PUBLIC "${PROJECT_BINARY_DIR}")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# PEGTL
find_package(pegtl 3)
