#include "code/Identifier.hpp"

#include "code/Context.hpp"
#include "code/HamNodes.hpp"
#include "code/Node.hpp"
#include "data/Types.hpp"
#include "parse/Grammar.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"

#include <cassert>
#include <cctype>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace ham::code
{

/**
 * Construct an identifier. No error conditions.
 */
Identifier::Identifier(AstContext& ast_context, PegtlNode&& pegtl_node)
	: BasicNode<Identifier>(ast_context, pegtl_node),
	  content(pegtl_node->string())
{
	assert(pegtl_node->is_type<parse::Identifier>());

	// An identifier is an alternating series of id strings and variables.
	// Because the variables are dynamic, the identifier has to be recalculated
	// on each evaluation.
	//
	// Store children, and make sure only id strings and variables are present.
	for (PegtlNode& child : pegtl_node->children) {
		if (child->is_type<parse::IdString>()) {
			id_parts.push_back(child->string());
		} else if (child->is_type<parse::Variable>()) {
			id_parts.push_back(
				CreateNode<Variable>(ast_context, std::move(child))
			);
		} else {
			assert(false);
		}
	}
}

/**
 * Identifier evaluation needs to ensure that variables:
 * - Have exactly one element
 * - Consist only of permitted characters
 */
data::List
Identifier::Evaluate(EvaluationContext& eval_context) const
{
	std::string id;

	auto appendLiteral = [&id](const std::string& literal) { id += literal; };

	auto appendIdVar =
		[&id, &eval_context, this](const std::unique_ptr<Node>& var)
	{
		data::List list{var->Evaluate(eval_context)};

		if (list.size() != 1) {
			std::stringstream err_msg{};
			err_msg << "Variable " << var->String() << " evaluated to "
					<< list.size()
					<< " elements, but variables in identifiers "
					   "must have exactly 1 element";
			throw Error(err_msg.str());
		}

		const std::string& var_str = list.at(0);
		for (char c : var_str) {
			if (!IsIdChar(c)) {
				std::stringstream err_msg{};
				err_msg
					<< "Identifiers must consist of [a-zA-Z0-9/\\\\_-], but "
					   "variable "
					<< var->String() << " evaluated to " << var_str
					<< " which has the invalid character " << c;
				throw Error(err_msg.str());
			}
		}

		id += var_str;
	};

	for (const IdPart& id_part : id_parts) {
		std::visit(overloaded{appendLiteral, appendIdVar}, id_part);
	}

	return {id};
}

std::string
Identifier::String() const
{
	return content;
}

NodeDump
Identifier::Dump() const
{
	std::vector<std::reference_wrapper<const Node>> children;
	for (const IdPart& id_part : id_parts) {
		if (const auto* var = std::get_if<std::unique_ptr<Node>>(&id_part)) {
			children.push_back(std::cref(**var));
		}
	}

	return {.type = type, .content = content, .children = std::move(children)};
}

bool
Identifier::IsIdChar(unsigned char c) const
{
	return std::isalnum(c) || c == '/' || c == '\\' || c == '_' || c == '-';
}

} // namespace ham::code
