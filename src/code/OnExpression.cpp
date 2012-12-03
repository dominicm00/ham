/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/OnExpression.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"
#include "data/TargetPool.h"


namespace ham {
namespace code {


OnExpression::OnExpression(Node* object, Node* expression)
	:
	fObject(object),
	fExpression(expression)
{
}


OnExpression::~OnExpression()
{
	delete fObject;
	delete fExpression;
}


StringList
OnExpression::Evaluate(EvaluationContext& context)
{
	// get the target names -- we need at least one
	const StringList& objects = fObject->Evaluate(context);
	if (objects.IsEmpty())
		return StringList::False();

	// get the first of the targets and push its variable domain as a new scope
	data::Target* target = context.Targets().LookupOrCreate(objects.Head());

	data::VariableScope scope(*target->Variables(true), context.GlobalScope());
	context.SetGlobalScope(&scope);

	// execute the expression
	const StringList& result = fExpression->Evaluate(context);

	// pop the scope
	context.SetGlobalScope(scope.Parent());

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


}	// namespace code
}	// namespace ham
