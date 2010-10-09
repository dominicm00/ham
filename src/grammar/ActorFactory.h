/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_GRAMMAR_ACTOR_FACTORY_H
#define HAM_GRAMMAR_ACTOR_FACTORY_H


#include "grammar/Spirit.h"
	// precompiled -- must be first


namespace grammar {


template<typename T>
struct FactoryCreateEval0
{
	template <typename Env, typename Factory>
	struct result
	{
		typedef T* type;
	};

	typedef boost::mpl::true_ no_nullary;

	template<typename RT, typename Env, typename Factory>
	static T* eval(const Env& env, Factory& factory)
	{
		return factory.eval(env).template Create<T>();
	}
};


template<typename T>
struct FactoryCreateEval1
{
	template <typename Env, typename Factory, typename Argument1>
	struct result
	{
		typedef T* type;
	};

	typedef boost::mpl::true_ no_nullary;

	template<typename RT, typename Env, typename Factory, typename Argument1>
	static T* eval(const Env& env, Factory& factory, const Argument1& arg1)
	{
		return factory.eval(env).template Create<T>(arg1.eval(env));
	}
};


template<typename T>
struct FactoryCreateEval2
{
	template <typename Env, typename Factory, typename Argument1,
		typename Argument2>
	struct result
	{
		typedef T* type;
	};

	typedef boost::mpl::true_ no_nullary;

	template<typename RT, typename Env, typename Factory, typename Argument1,
		typename Argument2>
	static T* eval(const Env& env, Factory& factory, const Argument1& arg1,
		const Argument2& arg2)
	{
		return factory.eval(env).template Create<T>(arg1.eval(env),
			arg2.eval(env));
	}
};


template<typename T>
struct FactoryCreateEval3
{
	template <typename Env, typename Factory, typename Argument1,
		typename Argument2, typename Argument3>
	struct result
	{
		typedef T* type;
	};

	typedef boost::mpl::true_ no_nullary;

	template<typename RT, typename Env, typename Factory, typename Argument1,
		typename Argument2, typename Argument3>
	static T* eval(const Env& env, Factory& factory, const Argument1& arg1,
		const Argument2& arg2, const Argument3& arg3)
	{
		return factory.eval(env).template Create<T>(arg1.eval(env),
			arg2.eval(env), arg3.eval(env));
	}
};


template<typename T>
struct FactoryCreateEval4
{
	template <typename Env, typename Factory, typename Argument1,
		typename Argument2, typename Argument3, typename Argument4>
	struct result
	{
		typedef T* type;
	};

	typedef boost::mpl::true_ no_nullary;

	template<typename RT, typename Env, typename Factory, typename Argument1,
		typename Argument2, typename Argument3, typename Argument4>
	static T* eval(const Env& env, Factory& factory, const Argument1& arg1,
		const Argument2& arg2, const Argument3& arg3, const Argument4& arg4)
	{
		return factory.eval(env).template Create<T>(arg1.eval(env),
			arg2.eval(env), arg3.eval(env), arg4.eval(env));
	}
};


template<typename Factory>
struct ActorFactory {
	ActorFactory(Factory& factory)
		:
		fFactory(factory)
	{
	}

	template<typename T>
	const boost::phoenix::actor<
		typename boost::phoenix::as_composite<FactoryCreateEval0<T>,
			boost::phoenix::actor<boost::phoenix::reference<Factory> > >::type>
	Create()
	{
		return boost::phoenix::compose<FactoryCreateEval0<T> >(
			boost::phoenix::ref(fFactory));
	}

	template<typename T, typename Argument1>
	const boost::phoenix::actor<
		typename boost::phoenix::as_composite<FactoryCreateEval1<T>,
			boost::phoenix::actor<boost::phoenix::reference<Factory> >,
			Argument1>::type>
	Create(const Argument1& arg1)
	{
		return boost::phoenix::compose<FactoryCreateEval1<T> >(
			boost::phoenix::ref(fFactory), arg1);
	}

	template<typename T, typename Argument1, typename Argument2>
	const boost::phoenix::actor<
		typename boost::phoenix::as_composite<FactoryCreateEval2<T>,
			boost::phoenix::actor<boost::phoenix::reference<Factory> >,
			Argument1, Argument2>::type>
	Create(const Argument1& arg1, const Argument2& arg2)
	{
		return boost::phoenix::compose<FactoryCreateEval2<T> >(
			boost::phoenix::ref(fFactory), arg1, arg2);
	}

	template<typename T, typename Argument1, typename Argument2,
		typename Argument3>
	const boost::phoenix::actor<
		typename boost::phoenix::as_composite<FactoryCreateEval3<T>,
			boost::phoenix::actor<boost::phoenix::reference<Factory> >,
			Argument1, Argument2, Argument3>::type>
	Create(const Argument1& arg1, const Argument2& arg2, const Argument3& arg3)
	{
		return boost::phoenix::compose<FactoryCreateEval3<T> >(
			boost::phoenix::ref(fFactory), arg1, arg2, arg3);
	}

	template<typename T, typename Argument1, typename Argument2,
		typename Argument3, typename Argument4>
	const boost::phoenix::actor<
		typename boost::phoenix::as_composite<FactoryCreateEval4<T>,
			boost::phoenix::actor<boost::phoenix::reference<Factory> >,
			Argument1, Argument2, Argument3, Argument4>::type>
	Create(const Argument1& arg1, const Argument2& arg2, const Argument3& arg3,
		const Argument4& arg4)
	{
		return boost::phoenix::compose<FactoryCreateEval4<T> >(
			boost::phoenix::ref(fFactory), arg1, arg2, arg3, arg4);
	}

private:
	Factory&	fFactory;
};


}	// namespace grammar


#endif	// HAM_GRAMMAR_ACTOR_FACTORY_H
