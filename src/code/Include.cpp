/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/Include.h"

#include <fstream>
#include <memory>

#include "code/Block.h"
#include "code/DumpContext.h"
#include "code/EvaluationContext.h"
#include "code/EvaluationException.h"
#include "parser/Parser.h"


namespace ham {
namespace code {


Include::Include(Node* fileNames)
	:
	fFileNames(fileNames)
{
}


Include::~Include()
{
	delete fFileNames;
}


StringList
Include::Evaluate(EvaluationContext& context)
{
	StringList fileNames = fFileNames->Evaluate(context);
	if (!fileNames.IsEmpty()) {
		// Only the file referred to by the first element is included.
		String fileName = fileNames.ElementAt(0);
// TODO: Bind target!
		std::ifstream file(fileName.ToCString());
		if (file.fail()) {
			throw EvaluationException(
				std::string("include: Failed to open file \"")
					+ fileName.ToCString() + "\"");
		}

		parser::Parser parser;
		parser.SetFileName(fileName.ToStlString());
		std::auto_ptr<code::Block> block(parser.Parse(file));
// TODO: Once it goes out of scope, it will also take rules defined there down
// with it!

// TODO: New variable scope?
		block->Evaluate(context);

		if (context.GetJumpCondition() == JUMP_CONDITION_JUMP_TO_EOF)
			context.SetJumpCondition(JUMP_CONDITION_NONE);
	}

	return StringList::False();
}


code::Node*
Include::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	return fFileNames->Visit(visitor);
}


void
Include::Dump(DumpContext& context) const
{
	context << "Include(\n";
	context.BeginChildren();

	fFileNames->Dump(context);

	context.EndChildren();
	context << ")\n";
}


}	// namespace code
}	// namespace ham
