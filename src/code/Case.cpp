/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "code/Case.hpp"

#include "code/DumpContext.hpp"
#include "code/EvaluationContext.hpp"
#include "data/RegExp.hpp"

#include <iostream>

namespace ham::code
{

Case::Case(const String& pattern, Node* block)
	: fPattern(pattern),
	  fBlock(block)
{
	fBlock->AcquireReference();
}

Case::~Case() { fBlock->ReleaseReference(); }

bool
Case::Matches(EvaluationContext&, const StringList& value) const
{
	using data::RegExp;

	try {
		RegExp regExp(fPattern.ToCString(), RegExp::PATTERN_TYPE_WILDCARD);
		String string = value.ElementAt(0);
		RegExp::MatchResult match = regExp.Match(string.ToCString());
		return match.HasMatched() && match.StartOffset() == 0
			&& match.EndOffset() == string.Length();
	} catch (const RegExp::Exception& e) {
		std::cerr << e.what();
		return false;
	}
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

} // namespace ham::code
