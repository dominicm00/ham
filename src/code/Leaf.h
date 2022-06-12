/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_LEAF_H
#define HAM_CODE_LEAF_H

#include "code/Node.h"

namespace ham
{

namespace data
{
class StringListOperations;
}

namespace code
{

class Leaf : public Node
{
  public:
	Leaf(const String& string);
	virtual ~Leaf();

	virtual StringList Evaluate(EvaluationContext& context);
	virtual Node* Visit(NodeVisitor& visitor);
	virtual void Dump(DumpContext& context) const;

	static StringList EvaluateString(EvaluationContext& context,
									 const char* stringStart,
									 const char* stringEnd,
									 const String* originalString);

  private:
	static StringList _EvaluateVariableExpression(
		EvaluationContext& context,
		const char* variableStart,
		const char* variableEnd,
		const std::vector<const char*>& colon,
		const char* openingBracket,
		const char* closingBracket,
		bool recursive);
	static bool _ParseSubscripts(const char* start,
								 const char* end,
								 size_t& _firstIndex,
								 size_t& _endIndex);
	static bool _ParseStringListOperationsRecursive(
		EvaluationContext& context,
		const std::vector<StringList>& operationsStringsList,
		size_t operationsStringsListIndex,
		data::StringListOperations operations,
		std::vector<data::StringListOperations>& _operationsList);

  private:
	String fString;
};

} // namespace code
} // namespace ham

#endif // HAM_CODE_LEAF_H
