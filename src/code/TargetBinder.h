/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_TARGET_BINDER_H
#define HAM_CODE_TARGET_BINDER_H


#include "data/String.h"


namespace ham {


namespace data {
	class MakeTarget;
	class Target;
}


namespace code {


class EvaluationContext;


class TargetBinder {
public:
	static	void				Bind(EvaluationContext& context,
									const data::Target* target,
									String& _boundPath);
	static	void				Bind(EvaluationContext& context,
									data::MakeTarget* target);
};


}	// namespace code
}	// namespace ham


#endif	// HAM_CODE_TARGET_BINDER_H
