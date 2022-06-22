AM_CPPFLAGS = -I./src
AM_CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic

BUILT_SOURCES =					\
	ruleset/HamRuleset.cpp		\
	ruleset/HamRuleset.hpp		\
	ruleset/JamRuleset.cpp		\
	ruleset/JamRuleset.hpp
CLEANFILES = $(BUILT_SOURCES)

ruleset/HamRuleset.cpp ruleset/HamRuleset.hpp: ruleset/HamRuleset.ham
	./ruleset/build-ruleset.sh $@ ruleset/HamRuleset.ham

ruleset/JamRuleset.cpp ruleset/JamRuleset.hpp: ruleset/JamRuleset.ham
	./ruleset/build-ruleset.sh $@ ruleset/JamRuleset.ham


lib_LTLIBRARIES = libham.la
libham_la_SOURCES =								\
	behavior/Behavior.cpp						\
	code/ActionsDefinition.cpp					\
	code/Assignment.cpp							\
	code/BinaryExpression.cpp					\
	code/Block.cpp								\
	code/BuiltInRules.cpp						\
	code/Case.cpp								\
	code/Constant.cpp							\
	code/DumpContext.cpp						\
	code/EvaluationContext.cpp					\
	code/For.cpp								\
	code/FunctionCall.cpp						\
	code/If.cpp									\
	code/InListExpression.cpp					\
	code/Include.cpp							\
	code/Jump.cpp								\
	code/Leaf.cpp								\
	code/List.cpp								\
	code/LocalVariableDeclaration.cpp			\
	code/Node.cpp								\
	code/NotExpression.cpp						\
	code/OnExpression.cpp						\
	code/RuleDefinition.cpp						\
	code/RuleInstructions.cpp					\
	code/Switch.cpp								\
	code/UserRuleInstructions.cpp				\
	code/While.cpp								\
	data/FileStatus.cpp							\
	data/Path.cpp								\
	data/RegExp.cpp								\
	data/RuleActions.cpp						\
	data/String.cpp								\
	data/StringList.cpp							\
	data/StringListOperations.cpp				\
	data/Target.cpp								\
	data/TargetBinder.cpp						\
	data/TargetPool.cpp							\
	data/Time.cpp								\
	data/VariableScope.cpp						\
	make/Command.cpp							\
	make/MakeTarget.cpp							\
	make/Options.cpp							\
	make/Processor.cpp							\
	make/TargetBuildInfo.cpp					\
	make/TargetBuilder.cpp						\
	parser/Parser.cpp							\
	platform/unix/PlatformProcessDelegate.cpp	\
	process/Process.cpp							\
	util/Constants.cpp							\
	util/OptionIterator.cpp						\
	util/Referenceable.cpp						\
	ruleset/HamRuleset.cpp						\
	ruleset/JamRuleset.cpp

check_PROGRAMS = hamtest
TESTS = hamtest
hamtest_LDADD = libham.la
hamtest_SOURCES = 						\
	tests/ham-tests.cpp					\
	tests/PathTest.cpp					\
	tests/RegExpTest.cpp				\
	tests/RulesetTest.cpp				\
	tests/StringListTest.cpp			\
	tests/StringPartTest.cpp			\
	tests/StringTest.cpp				\
	tests/TargetBinderTest.cpp			\
	tests/TimeTest.cpp					\
	tests/VariableExpansionTest.cpp		\
	test/DataBasedTest.cpp				\
	test/DataBasedTestParser.cpp		\
	test/MultipleFilesDataBasedTest.cpp	\
	test/RunnableTest.cpp				\
	test/TemplateBasedTest.cpp			\
	test/Test.cpp						\
	test/TestException.cpp				\
	test/TestFixture.cpp				\
	test/TestRunner.cpp					\
	test/TestSuite.cpp

# TODO: define private/public headers
nobase_dist_include_HEADERS =					\
	behavior/Behavior.hpp						\
	behavior/Compatibility.hpp					\
	code/ActionsDefinition.hpp					\
	code/Assignment.hpp							\
	code/BinaryExpression.hpp					\
	code/Block.hpp								\
	code/BuiltInRules.hpp						\
	code/Case.hpp								\
	code/Constant.hpp							\
	code/Defs.hpp								\
	code/DumpContext.hpp						\
	code/EvaluationContext.hpp					\
	code/EvaluationException.hpp				\
	code/For.hpp								\
	code/FunctionCall.hpp						\
	code/If.hpp									\
	code/InListExpression.hpp					\
	code/Include.hpp							\
	code/Jump.hpp								\
	code/Leaf.hpp								\
	code/List.hpp								\
	code/LocalVariableDeclaration.hpp			\
	code/Node.hpp								\
	code/NotExpression.hpp						\
	code/OnExpression.hpp						\
	code/Rule.hpp								\
	code/RuleDefinition.hpp						\
	code/RuleInstructions.hpp					\
	code/RulePool.hpp							\
	code/Switch.hpp								\
	code/UserRuleInstructions.hpp				\
	code/While.hpp								\
	data/FileStatus.hpp							\
	data/Path.hpp								\
	data/RegExp.hpp								\
	data/RuleActions.hpp						\
	data/String.hpp								\
	data/StringBuffer.hpp						\
	data/StringList.hpp							\
	data/StringListOperations.hpp				\
	data/StringPart.hpp							\
	data/Target.hpp								\
	data/TargetBinder.hpp						\
	data/TargetContainers.hpp					\
	data/TargetPool.hpp							\
	data/Time.hpp								\
	data/VariableDomain.hpp						\
	data/VariableScope.hpp						\
	make/Command.hpp							\
	make/MakeException.hpp						\
	make/MakeTarget.hpp							\
	make/Options.hpp							\
	make/Processor.hpp							\
	make/TargetBuildInfo.hpp					\
	make/TargetBuilder.hpp						\
	parser/LexException.hpp						\
	parser/Lexer.hpp							\
	parser/ParseException.hpp					\
	parser/ParsePosition.hpp					\
	parser/Parser.hpp							\
	parser/Token.hpp							\
	platform/PlatformProcessDelegate.hpp		\
	platform/unix/PlatformProcessDelegate.hpp	\
	process/ChildInfo.hpp						\
	process/Process.hpp							\
	util/Constants.hpp							\
	util/Exception.hpp							\
	util/OptionIterator.hpp						\
	util/Referenceable.hpp						\
	util/SequentialSet.hpp						\
	util/TextFileException.hpp					\
	util/TextFilePosition.hpp					\
	ruleset/HamRuleset.hpp						\
	ruleset/JamRuleset.hpp
