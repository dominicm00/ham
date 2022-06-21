/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "code/LocalVariableDeclaration.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"

namespace ham
{
namespace code
{

LocalVariableDeclaration::LocalVariableDeclaration(Node* variables,
												   Node* initializer)
	: fVariables(variables),
	  fInitializer(initializer)
{
	fVariables->AcquireReference();
	if (fInitializer != nullptr)
		fInitializer->AcquireReference();
}

LocalVariableDeclaration::~LocalVariableDeclaration()
{
	fVariables->ReleaseReference();
	if (fInitializer != nullptr)
		fInitializer->ReleaseReference();
}

StringList
LocalVariableDeclaration::Evaluate(EvaluationContext& context)
{
	// get the variables
	const StringList& variables = fVariables->Evaluate(context);

	if (fInitializer != nullptr) {
		// we have an initializer -- get the value and init the variables
		const StringList& value = fInitializer->Evaluate(context);

		for (StringList::Iterator it = variables.GetIterator(); it.HasNext();)
			context.LocalScope()->Set(it.Next(), value);

		return value;
	}

	// no initializer -- init with empty list
	for (StringList::Iterator it = variables.GetIterator(); it.HasNext();)
		context.LocalScope()->Set(it.Next(), StringList::False());

	return StringList::False();
}

code::Node*
LocalVariableDeclaration::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	if (Node* result = fVariables->Visit(visitor))
		return result;

	return fInitializer != nullptr ? fInitializer->Visit(visitor) : nullptr;
}

void
LocalVariableDeclaration::Dump(DumpContext& context) const
{
	context << "LocalVariableDeclaration(\n";
	context.BeginChildren();

	fVariables->Dump(context);

	if (fInitializer != nullptr)
		fInitializer->Dump(context);

	context.EndChildren();
	context << ")\n";
}

} // namespace code
} // namespace ham
