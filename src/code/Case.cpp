/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/Case.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


using code::Case;


Case::Case(const String& pattern, Node* block)
	:
	fPattern(pattern),
	fBlock(block)
{
}


StringList
Case::Evaluate(EvaluationContext& context)
{
	return fBlock->Evaluate(context);
}


void
Case::Dump(DumpContext& context) const
{
	context << "Case(\"" << fPattern << "\",\n";
	context.BeginChildren();

	fBlock->Dump(context);

	context.EndChildren();
	context << ")\n";
}
