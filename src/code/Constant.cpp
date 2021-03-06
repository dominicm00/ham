/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "code/Constant.hpp"

#include "code/DumpContext.hpp"
#include "code/EvaluationContext.hpp"

namespace ham::code
{

Constant::Constant(const StringList& value)
	: fValue(value)
{
}

Constant::~Constant() {}

StringList
Constant::Evaluate(EvaluationContext&)
{
	return fValue;
}

code::Node*
Constant::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	return nullptr;
}

void
Constant::Dump(DumpContext& context) const
{
	context << "Constant(\"" << fValue << "\")\n";
}

} // namespace ham::code
