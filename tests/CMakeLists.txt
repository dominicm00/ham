add_executable(
  tests

  ParseUtils.cpp
  CodeUtils.cpp

  code/Identifier.cpp
  code/Variable.cpp

  data/VariableScope.cpp

  parse/ActionDefinitions.cpp
  parse/BracketExpressions.cpp
  parse/Conditions.cpp
  parse/ForLoops.cpp
  parse/GrammarAnalysis.cpp
  parse/Identifiers.cpp
  parse/IfStatements.cpp
  parse/Leafs.cpp
  parse/Lists.cpp
  parse/RuleDefinitions.cpp
  parse/RuleInvocations.cpp
  parse/RuleSignatures.cpp
  parse/StatementBlocks.cpp
  parse/TargetStatements.cpp
  parse/VariableAssignment.cpp
  parse/Variables.cpp
  parse/WhileLoops.cpp
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
