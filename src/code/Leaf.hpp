/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_LEAF_HPP
#define HAM_CODE_LEAF_HPP

#include "code/Node.hpp"
#include "data/StringListOperations.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace ham
{

namespace code
{

class Leaf : public Node
{
  public:
	Leaf(std::string_view string);
	virtual ~Leaf();

	virtual std::vector<std::string> Evaluate(EvaluationContext& context
	) override;
	virtual Node* Visit(NodeVisitor& visitor) override;
	virtual void Dump(DumpContext& context) const override;

	static std::vector<std::string>
	EvaluateString(EvaluationContext& context, std::string_view string);

  private:
	static StringList _EvaluateVariableExpression(
		EvaluationContext& context,
		std::string_view variable,
		const std::vector<const char*>& colon,
		std::string_view brackets,
		bool recursive
	);
	static bool _ParseSubscripts(
		std::string_view start,
		std::string_view end,
		size_t& _firstIndex,
		size_t& _endIndex
	);
	static bool _ParseStringListOperationsRecursive(
		EvaluationContext& context,
		const std::vector<std::vector<std::string>>& operationsStringsList,
		size_t operationsStringsListIndex,
		data::StringListOperations operations,
		std::vector<data::StringListOperations>& _operationsList
	);

  private:
	String fString;
};

} // namespace code
} // namespace ham

#endif // HAM_CODE_LEAF_HPP
