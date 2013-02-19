/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "make/Processor.h"

#include "code/Block.h"
#include "code/BuiltInRules.h"
#include "code/Jambase.h"
#include "parser/Parser.h"


namespace ham {
namespace make {


Processor::Processor()
	:
	fGlobalVariables(),
	fTargets(),
	fEvaluationContext(fGlobalVariables, fTargets)
{
	code::BuiltInRules::RegisterRules(fEvaluationContext.Rules());
}


Processor::~Processor()
{
}


void
Processor::SetCompatibility(behavior::Compatibility compatibility)
{
	fEvaluationContext.SetCompatibility(compatibility);
}


void
Processor::SetBehavior(behavior::Behavior behavior)
{
	fEvaluationContext.SetBehavior(behavior);
}


void
Processor::SetOutput(std::ostream& output)
{
	fEvaluationContext.SetOutput(output);
}


void
Processor::SetErrorOutput(std::ostream& output)
{
	fEvaluationContext.SetErrorOutput(output);
}


void
Processor::SetTargetsToBuild(const StringList& targets)
{
	fTargetsToBuild = targets;
}


void
Processor::ProcessJambase()
{
	// parse code
	parser::Parser parser;
	parser.SetFileName("Jambase");
	util::Reference<code::Block> block(parser.Parse(code::kJambase), true);

	// execute the code
	block->Evaluate(fEvaluationContext);
}


void
Processor::BindTargets()
{
	// TODO:...
}


void
Processor::BuildTargets()
{
	// TODO:...
}


}	// namespace make
}	// namespace ham
