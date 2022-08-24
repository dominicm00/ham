#ifndef HAM_CODE_IDENTIFIER_HPP
#define HAM_CODE_IDENTIFIER_HPP

#include "defs/CodeDefs.hpp"

#include "code/HamNodes.hpp"
#include "code/Variable.hpp"

namespace ham::code
{
class Identifier : public BasicNode<Identifier> {
	using IdPart = std::variant<std::string, std::unique_ptr<Node>>;

  public:
	Identifier(AstContext& ast_context, PegtlNode&& pegtl_node);

	data::List Evaluate(EvaluationContext& eval_context) const;
	std::string String() const;
	NodeDump Dump() const;

  private:
	bool IsIdChar(unsigned char) const;

  private:
	std::string content;
	std::vector<IdPart> id_parts;
};

} // namespace ham::code

#endif // HAM_CODE_IDENTIFIER_HPP
