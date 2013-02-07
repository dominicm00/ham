/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/Case.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"
#include "data/RegExp.h"


namespace ham {
namespace code {


Case::Case(const String& pattern, Node* block)
	:
	fPattern(pattern),
	fBlock(block)
{
}


Case::~Case()
{
	delete fBlock;
}


bool
Case::Matches(EvaluationContext& context, const StringList& value) const
{
	using data::RegExp;

	RegExp regExp(fPattern.ToCString(), RegExp::PATTERN_TYPE_WILDCARD);
	if (!regExp.IsValid()) {
// TODO: Throw exception!
		return false;
	}

	String string = value.ElementAt(0);
	RegExp::MatchResult match = regExp.Match(string.ToCString());
	return match.HasMatched() && match.StartOffset() == 0
		&& match.EndOffset() == string.Length();
}


StringList
Case::Evaluate(EvaluationContext& context)
{
	return fBlock->Evaluate(context);
}


code::Node*
Case::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	return fBlock->Visit(visitor);
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


}	// namespace code
}	// namespace ham
