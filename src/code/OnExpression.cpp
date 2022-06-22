/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "code/OnExpression.hpp"

#include "code/DumpContext.hpp"
#include "code/EvaluationContext.hpp"
#include "data/TargetPool.hpp"

namespace ham::code
{

OnExpression::OnExpression(Node* object, Node* expression)
	: fObject(object),
	  fExpression(expression)
{
	fObject->AcquireReference();
	fExpression->AcquireReference();
}

OnExpression::~OnExpression()
{
	fObject->ReleaseReference();
	fExpression->ReleaseReference();
}

StringList
OnExpression::Evaluate(EvaluationContext& context)
{
	// get the target names -- we need at least one
	const StringList& objects = fObject->Evaluate(context);
	if (objects.IsEmpty())
		return StringList::False();

	// get the first of the targets and push a copy of its variable domain as a
	// new local scope
	data::Target* target = context.Targets().LookupOrCreate(objects.Head());
	data::VariableDomain localVariables(*target->Variables(true));
	data::VariableScope localScope(localVariables, context.LocalScope());
	context.SetLocalScope(&localScope);

	// execute the expression
	const StringList& result = fExpression->Evaluate(context);

	// pop the scope
	context.SetLocalScope(localScope.Parent());

	return result;
}

code::Node*
OnExpression::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	if (Node* result = fObject->Visit(visitor))
		return result;

	return fExpression->Visit(visitor);
}

void
OnExpression::Dump(DumpContext& context) const
{
	context << "OnExpression(\n";
	context.BeginChildren();

	fObject->Dump(context);
	fExpression->Dump(context);

	context.EndChildren();
	context << ")\n";
}

} // namespace ham::code
