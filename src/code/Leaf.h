/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_LEAF_H
#define HAM_CODE_LEAF_H


#include "code/Node.h"


namespace ham {
namespace code {


class Leaf : public Node {
public:
								Leaf(const String& string);
	virtual						~Leaf();

	virtual	StringList			Evaluate(EvaluationContext& context);
	virtual	Node*				Visit(NodeVisitor& visitor);
	virtual	void				Dump(DumpContext& context) const;

private:
	static	StringList			_EvaluateString(EvaluationContext& context,
									const char* stringStart,
									const char* stringEnd,
									const String* originalString);
	static	StringList			_EvaluateVariableExpression(
									EvaluationContext& context,
									const char* variableStart,
									const char* variableEnd,
									const char* colon,
									const char* openingBracket,
									const char* closingBracket,
									bool recursive);
	static	bool				_ParseSubscripts(const char* start,
									const char* end, size_t& _firstIndex,
									size_t& _endIndex);

private:
			String				fString;
};


}	// namespace code
}	// namespace ham


#endif	// HAM_CODE_LEAF_H
