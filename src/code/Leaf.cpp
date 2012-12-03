/*
 * Copyright 2010-2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/Leaf.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


namespace ham {
namespace code {


Leaf::Leaf(const String& string)
	:
	fString(string)
{
}

Leaf::~Leaf()
{
}


StringList
Leaf::Evaluate(EvaluationContext& context)
{
#if 0
	size_t length = fString.length();
	size_t dollarIndex = 0;

	for (;;) {
		dollarIndex = fString.find('$');
		if (dollarIndex == String::n)
	}
#endif

	// TODO: Evalutate the string (variable substitution...).
	StringList list;
	list.Append(fString);
	return list;
}

code::Node*
Leaf::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	return NULL;
}


void
Leaf::Dump(DumpContext& context) const
{
	context << "Leaf(\"" << fString << "\")\n";
}


}	// namespace code
}	// namespace ham
