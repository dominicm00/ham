/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_LOCAL_VARIABLE_DECLARATION_H
#define HAM_CODE_LOCAL_VARIABLE_DECLARATION_H


#include "code/List.h"


namespace code {


class LocalVariableDeclaration : public Node {
public:
								LocalVariableDeclaration(List* variables);

	inline	void				SetInitializer(List* initializer);

	virtual	StringList			Evaluate(EvaluationContext& context);

	virtual	void				Dump(DumpContext& context) const;

private:
			List*				fVariables;
			List*				fInitializer;
};


void
LocalVariableDeclaration::SetInitializer(List* initializer)
{
	fInitializer = initializer;
}


}	// namespace code


#endif	// HAM_CODE_LOCAL_VARIABLE_DECLARATION_H
