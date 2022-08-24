#ifndef HAM_CODE_VARIABLE_HPP
#define HAM_CODE_VARIABLE_HPP

#include "defs/CodeDefs.hpp"

#include "code/HamNodes.hpp"
#include "code/Identifier.hpp"

namespace ham::code
{
class Variable : public BasicNode<Variable> {
  public:
	Variable(AstContext& ast_context, PegtlNode&& pegtl_node);

	data::List Evaluate(EvaluationContext& eval_context) const override;
	[[nodiscard]] std::string String() const override;
	[[nodiscard]] NodeDump Dump() const override;

  private:
	[[nodiscard]] data::List
	DoSubscript(EvaluationContext& eval_context, const std::string&, const data::List&)
		const;

	std::string content;
	std::unique_ptr<Node> id;
	std::unique_ptr<Node> subscript;
};

} // namespace ham::code

#endif // HAM_CODE_VARIABLE_HPP
