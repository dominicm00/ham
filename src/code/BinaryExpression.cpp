/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/BinaryExpression.h"

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"


namespace ham {
namespace code {


template<typename Operator>
BinaryExpression<Operator>::BinaryExpression(Node* left, Node* right)
	:
	fLeft(left),
	fRight(right)
{
}


template<typename Operator>
BinaryExpression<Operator>::~BinaryExpression()
{
	delete fLeft;
	delete fRight;
}


template<typename Operator>
StringList
BinaryExpression<Operator>::Evaluate(EvaluationContext& context)
{
	return Operator::Do(fLeft->Evaluate(context),
		fRight->Evaluate(context));
}


template<typename Operator>
code::Node*
BinaryExpression<Operator>::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	if (Node* result = fLeft->Visit(visitor))
		return result;

	return fRight->Visit(visitor);
}


template<typename Operator>
void
BinaryExpression<Operator>::Dump(DumpContext& context) const
{
	context << "BinaryExpressionNode[" << Operator::kSymbol << "](\n";
	context.BeginChildren();

	fLeft->Dump(context);
	fRight->Dump(context);

	context.EndChildren();
	context << ")\n";
}


// define and instantiate the specializations

#define HAM_DEFINE_OPERATOR_EXPRESSION(name, symbol, expression)		\
	struct name##Operator {												\
		static const char* const kSymbol;								\
																		\
		static StringList Do(const StringList& a, const StringList& b)	\
		{																\
			return expression											\
				? StringList::True() : StringList::False();				\
		}																\
	};																	\
																		\
	const char* const name##Operator::kSymbol = #symbol;				\
																		\
	template class BinaryExpression<name##Operator>;

#define HAM_DEFINE_COMPARISON_OPERATOR_EXPRESSION(name, symbol,	operator)	\
	HAM_DEFINE_OPERATOR_EXPRESSION(name, symbol,							\
		a.CompareWith(b, true) operator 0)

HAM_DEFINE_COMPARISON_OPERATOR_EXPRESSION(Equal,			=,	==)
HAM_DEFINE_COMPARISON_OPERATOR_EXPRESSION(NotEqual,			!=,	!=)
HAM_DEFINE_COMPARISON_OPERATOR_EXPRESSION(Less,				<,	<)
HAM_DEFINE_COMPARISON_OPERATOR_EXPRESSION(LessOrEqual,		<=,	<=)
HAM_DEFINE_COMPARISON_OPERATOR_EXPRESSION(Greater,			>,	>)
HAM_DEFINE_COMPARISON_OPERATOR_EXPRESSION(GreaterOrEqual,	>=,	>=)

HAM_DEFINE_OPERATOR_EXPRESSION(And,	&&, a.IsTrue() && b.IsTrue())
HAM_DEFINE_OPERATOR_EXPRESSION(Or,	||, a.IsTrue() || b.IsTrue())

#undef HAM_DEFINE_OPERATOR_EXPRESSION
#undef HAM_DEFINE_COMPARISON_OPERATOR_EXPRESSION


}	// namespace code
}	// namespace ham
