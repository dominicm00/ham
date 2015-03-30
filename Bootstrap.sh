#!/bin/sh
# Bootstrap utility for Ham

CXX=${CXX-g++}
LD=${LD-$CXX}
CXXFLAGS="-std=c++0x $CXXFLAGS"

run_command()
{
	echo "$1" && $1
	if [ $? -ne 0 ]; then exit 1; fi
}

compile()
{
	SOURCES=$1
	LOCAL_CXXFLAGS="$CXXFLAGS $2"
	LINKFLAGS=$3

	LINKCMD="$LD $LINKFLAGS"

	for file in $SOURCES; do
		LINKCMD="$LINKCMD generated/$(basename $file).o"
		outfile=generated/$(basename $file).o
		if [ ! -f $outfile ] || [ src/$file -nt $outfile ]; then
			run_command "$CXX src/$file -c -o $outfile $LOCAL_CXXFLAGS"
		fi
	done

	run_command "$LINKCMD"
}

# Create build directory
mkdir -p generated

# Build ham
HAM_SRCS="code/ActionsDefinition.cpp
	code/Assignment.cpp
	behavior/Behavior.cpp
	code/BinaryExpression.cpp
	code/Block.cpp
	code/BuiltInRules.cpp
	code/Case.cpp
	code/Constant.cpp
	code/DumpContext.cpp
	code/EvaluationContext.cpp
	code/If.cpp
	code/Include.cpp
	code/Jambase.cpp
	code/For.cpp
	code/FunctionCall.cpp
	code/InListExpression.cpp
	code/Jump.cpp
	code/Leaf.cpp
	code/List.cpp
	code/LocalVariableDeclaration.cpp
	code/Node.cpp
	code/NotExpression.cpp
	code/OnExpression.cpp
	code/RuleDefinition.cpp
	code/RuleInstructions.cpp
	code/UserRuleInstructions.cpp
	code/Switch.cpp
	code/While.cpp
	data/FileStatus.cpp
	data/Path.cpp
	data/RegExp.cpp
	data/RuleActions.cpp
	data/String.cpp
	data/StringList.cpp
	data/StringListOperations.cpp
	data/Target.cpp
	data/TargetBinder.cpp
	data/TargetPool.cpp
	data/Time.cpp
	data/VariableScope.cpp
	make/Command.cpp
	make/MakeTarget.cpp
	make/Options.cpp
	make/Processor.cpp
	make/TargetBuilder.cpp
	make/TargetBuildInfo.cpp
	parser/Parser.cpp
	platform/unix/PlatformProcessDelegate.cpp
	process/Process.cpp
	util/Constants.cpp
	util/OptionIterator.cpp
	util/Referenceable.cpp
	ham.cpp
	"
compile "$HAM_SRCS" "-iquotesrc" "-o generated/ham"
