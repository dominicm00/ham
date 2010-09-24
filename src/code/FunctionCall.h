/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_FUNCTION_CALL_H
#define HAM_CODE_FUNCTION_CALL_H


#include "code/List.h"


namespace code {


class FunctionCall : public Node {
public:
								FunctionCall(Node* function);
								FunctionCall(Node* function,
									const ListList& arguments);

	inline	void				AddArgument(List* argument);

	virtual	StringList			Evaluate(EvaluationContext& context);

	virtual	void				Dump(DumpContext& context) const;

private:
			typedef ListList ArgumentList;

			Node*				fFunction;
			ArgumentList		fArguments;
};


void
FunctionCall::AddArgument(List* argument)
{
	fArguments.push_back(argument);
}


}	// namespace code


#endif	// HAM_CODE_FUNCTION_CALL_H
