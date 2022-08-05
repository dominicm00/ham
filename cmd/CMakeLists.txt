# add the executable
add_executable(ham ham.cpp)
target_include_directories(
  ham PUBLIC
  "${PROJECT_BINARY_DIR}"
  "${PROJECT_SOURCE_DIR}/include/public"
  )

# link against main library
target_link_libraries(ham PRIVATE ham_lib)

# set minimum C++ version
target_compile_features(ham PUBLIC cxx_std_20)
