/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_GRAMMAR_SIMPLE_FACTORY_H
#define HAM_GRAMMAR_SIMPLE_FACTORY_H


namespace grammar {


struct NoOpFactoryHook {
};


template<typename Hook = NoOpFactoryHook>
struct SimpleFactory {
	SimpleFactory(const Hook& hook = Hook())
		:
		fHook(hook)
	{
	}

	template<typename Type>
	Type* Create()
	{
		return fHook(new Type());
	}

	template<typename Type, typename Argument1>
	Type* Create(const Argument1& arg1)
	{
		return fHook(new Type(arg1));
	}

	template<typename Type, typename Argument1, typename Argument2>
	Type* Create(const Argument1& arg1, const Argument2& arg2)
	{
		return fHook(new Type(arg1, arg2));
	}

	template<typename Type, typename Argument1, typename Argument2,
		typename Argument3>
	Type* Create(const Argument1& arg1, const Argument2& arg2,
		const Argument3& arg3)
	{
		return fHook(new Type(arg1, arg2, arg3));
	}

	template<typename Type, typename Argument1, typename Argument2,
		typename Argument3, typename Argument4>
	Type* Create(const Argument1& arg1, const Argument2& arg2,
		const Argument3& arg3, const Argument4& arg4)
	{
		return fHook(new Type(arg1, arg2, arg3, arg4));
	}

private:
	Hook	fHook;
};


}	// namespace grammar


#endif	// HAM_GRAMMAR_SIMPLE_FACTORY_H
