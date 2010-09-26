/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_LOCAL_VARIABLE_DECLARATION_H
#define HAM_CODE_LOCAL_VARIABLE_DECLARATION_H


#include "code/Node.h"


namespace code {


class LocalVariableDeclaration : public Node {
public:
								LocalVariableDeclaration(Node* variables);

	inline	void				SetInitializer(Node* initializer);

	virtual	StringList			Evaluate(EvaluationContext& context);

	virtual	void				Dump(DumpContext& context) const;

private:
			Node*				fVariables;
			Node*				fInitializer;
};


void
LocalVariableDeclaration::SetInitializer(Node* initializer)
{
	fInitializer = initializer;
}


}	// namespace code


#endif	// HAM_CODE_LOCAL_VARIABLE_DECLARATION_H
