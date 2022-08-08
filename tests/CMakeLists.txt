add_executable(
  tests

  Utils.cpp

  parse/Identifiers.cpp
  parse/Leafs.cpp
  parse/Variables.cpp
  )

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(tests PRIVATE taocpp::pegtl)
target_link_libraries(tests PRIVATE ham_lib)

# Manually include private headers
target_include_directories(
  tests PRIVATE
  "${PROJECT_SOURCE_DIR}/include/private"
  )

# Set minimum C++ version
target_compile_features(tests PUBLIC cxx_std_20)

# Register tests
include(CTest)
include(Catch)
catch_discover_tests(
  tests
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/tests"
  )
