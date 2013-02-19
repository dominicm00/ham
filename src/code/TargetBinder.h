/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_TARGET_BINDER_H
#define HAM_CODE_TARGET_BINDER_H


namespace ham {


namespace data {
	class Target;
}


namespace code {


class EvaluationContext;


class TargetBinder {
public:
	static	void				Bind(EvaluationContext& context,
									data::Target* target);
};


}	// namespace code
}	// namespace ham


#endif	// HAM_CODE_TARGET_BINDER_H
