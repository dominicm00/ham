/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_FUNCTION_CALL_H
#define HAM_CODE_FUNCTION_CALL_H


#include "code/Node.h"


namespace code {


class FunctionCall : public Node {
public:
								FunctionCall(Node* function);
								FunctionCall(Node* function,
									const NodeList& arguments);

	inline	void				AddArgument(Node* argument);

	virtual	StringList			Evaluate(EvaluationContext& context);

	virtual	void				Dump(DumpContext& context) const;

private:
			typedef NodeList ArgumentList;

			Node*				fFunction;
			ArgumentList		fArguments;
};


void
FunctionCall::AddArgument(Node* argument)
{
	fArguments.push_back(argument);
}


}	// namespace code


#endif	// HAM_CODE_FUNCTION_CALL_H
