set(CMAKE_CXX_CLANG_TIDY clang-tidy)

# create library
add_library(
  ham_lib

  code/Context.cpp
  code/Identifier.cpp
  code/Variable.cpp

  data/VariableScope.cpp

  code/Context.cpp

  data/VariableScope.cpp

  parse/PegtlParser.cpp
  parse/PegtlUtils.cpp

  util/HamError.cpp

  ${HAM_PUBLIC_HEADER_FILES}
  )

# warning level
if(MSVC)
  target_compile_options(ham_lib PRIVATE /W4 /WX)
else()
  target_compile_options(ham_lib PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# include headers
target_include_directories(
  ham_lib PUBLIC
  "${PROJECT_SOURCE_DIR}/include/public"
  )

target_include_directories(
  ham_lib PRIVATE
  "${PROJECT_SOURCE_DIR}/include/private"
  )

# set minimum C++ version
target_compile_features(ham_lib PUBLIC cxx_std_20)

# PEGTL
target_link_libraries(ham_lib PRIVATE taocpp::pegtl)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include/public"
  PREFIX "Header Files"
  FILES ${HAM_PUBLIC_HEADER_FILES}
  )
