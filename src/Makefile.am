AM_CPPFLAGS = -I./src
AM_CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic

BUILT_SOURCES =					\
	ruleset/HamRuleset.cpp		\
	ruleset/HamRuleset.h		\
	ruleset/JamRuleset.cpp		\
	ruleset/JamRuleset.h
CLEANFILES = $(BUILT_SOURCES)

ruleset/HamRuleset.cpp ruleset/HamRuleset.h: ruleset/HamRuleset.ham
	./ruleset/build-ruleset.sh $@ ruleset/HamRuleset.ham

ruleset/JamRuleset.cpp ruleset/JamRuleset.h: ruleset/JamRuleset.ham
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
nobase_dist_include_HEADERS =				\
	behavior/Behavior.h						\
	behavior/Compatibility.h				\
	code/ActionsDefinition.h				\
	code/Assignment.h						\
	code/BinaryExpression.h					\
	code/Block.h							\
	code/BuiltInRules.h						\
	code/Case.h								\
	code/Constant.h							\
	code/Defs.h								\
	code/DumpContext.h						\
	code/EvaluationContext.h				\
	code/EvaluationException.h				\
	code/For.h								\
	code/FunctionCall.h						\
	code/If.h								\
	code/InListExpression.h					\
	code/Include.h							\
	code/Jump.h								\
	code/Leaf.h								\
	code/List.h								\
	code/LocalVariableDeclaration.h			\
	code/Node.h								\
	code/NotExpression.h					\
	code/OnExpression.h						\
	code/Rule.h								\
	code/RuleDefinition.h					\
	code/RuleInstructions.h					\
	code/RulePool.h							\
	code/Switch.h							\
	code/UserRuleInstructions.h				\
	code/While.h							\
	data/FileStatus.h						\
	data/Path.h								\
	data/RegExp.h							\
	data/RuleActions.h						\
	data/String.h							\
	data/StringBuffer.h						\
	data/StringList.h						\
	data/StringListOperations.h				\
	data/StringPart.h						\
	data/Target.h							\
	data/TargetBinder.h						\
	data/TargetContainers.h					\
	data/TargetPool.h						\
	data/Time.h								\
	data/VariableDomain.h					\
	data/VariableScope.h					\
	make/Command.h							\
	make/MakeException.h					\
	make/MakeTarget.h						\
	make/Options.h							\
	make/Processor.h						\
	make/TargetBuildInfo.h					\
	make/TargetBuilder.h					\
	parser/LexException.h					\
	parser/Lexer.h							\
	parser/ParseException.h					\
	parser/ParsePosition.h					\
	parser/Parser.h							\
	parser/Token.h							\
	platform/PlatformProcessDelegate.h		\
	platform/unix/PlatformProcessDelegate.h	\
	process/ChildInfo.h						\
	process/Process.h						\
	util/Constants.h						\
	util/Exception.h						\
	util/OptionIterator.h					\
	util/Referenceable.h					\
	util/SequentialSet.h					\
	util/TextFileException.h				\
	util/TextFilePosition.h					\
	ruleset/HamRuleset.h					\
	ruleset/JamRuleset.h
