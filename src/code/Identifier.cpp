#include "code/Context.hpp"
#include "code/HamNodes.hpp"
#include "code/Node.hpp"
#include "data/Types.hpp"
#include "parse/Grammar.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"

#include <cassert>
#include <functional>
#include <memory>
#include <variant>
#include <vector>

namespace ham::code
{

Identifier::Identifier(AstContext& ast_context, PegtlNode&& pegtl_node)
	: BasicNode<Identifier>(ast_context, pegtl_node),
	  content(pegtl_node->string())
{
	assert(pegtl_node->is_type<parse::Identifier>());

	for (PegtlNode& child : pegtl_node->children) {
		if (child->is_type<parse::IdString>()) {
			id_parts.push_back(child->string());
		} else if (child->is_type<parse::Variable>()) {
			id_parts.push_back(
				std::make_unique<Variable>(ast_context, std::move(child))
			);
		} else {
			assert(false);
		}
	}
}

data::List
Identifier::Evaluate(EvaluationContext& eval_context) const
{
	std::string id;
	for (const IdPart& id_part : id_parts) {
		std::visit(
			overloaded{
				[&id](const std::string& literal) { id += literal; },
				[&id, &eval_context, this](const std::unique_ptr<Variable>& var)
				{
			data::List list{var->Evaluate(eval_context)};
			if (list.size() > 1) {
				std::stringstream err_msg{};
				err_msg << "variable " << *var << " evaluated to "
						<< list.size()
						<< " elements, but variables in identifiers "
						   "must have 0-1 elements.";
				throw Error(err_msg.str());
			}
				}},
			id_part);
	}

	return {id};
}

NodeDump
Identifier::Dump() const
{
	std::vector<std::reference_wrapper<const Node>> children;
	for (const IdPart& id_part : id_parts) {
		if (const auto* var =
				std::get_if<std::unique_ptr<Variable>>(&id_part)) {
			children.push_back(std::cref(**var));
		}
	}

	return {.type = type, .content = content, .children = std::move(children)};
}

std::ostream&
operator<<(std::ostream& os, const Identifier& id)
{
	return os << id.content;
}

} // namespace ham::code
