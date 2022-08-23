#include "code/Context.hpp"
#include "code/HamNodes.hpp"
#include "code/Node.hpp"
#include "data/Types.hpp"
#include "parse/Grammar.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"

#include <cassert>
#include <cstddef>
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace ham::code
{

/**
 * Construct a variable expression. No error conditions.
 */
Variable::Variable(AstContext& ast_context, PegtlNode&& pegtl_node)
	: BasicNode<Variable>(ast_context, pegtl_node),
	  content(pegtl_node->string())
{
	assert(pegtl_node->is_type<parse::Variable>());

	// A variable consists of an identifier, and optionally, a subscript and
	// variable modifier sequence.
	// TODO: support variable modifiers
	assert(pegtl_node->children.size() > 0);
	id = CreateNode<Identifier>(
		ast_context,
		std::move(pegtl_node->children.front())
	);

	if (pegtl_node->children.size() > 1) {
		PegtlNode sub_wrapper = std::move(pegtl_node->children[1]);
		assert(sub_wrapper->is_type<parse::Subscript>());
		assert(sub_wrapper->children.size() == 1);
		subscript = CreateNode<Identifier>(
			ast_context,
			std::move(sub_wrapper->children.front())
		);
	}
}

data::List
Variable::Evaluate(EvaluationContext& eval_context) const
{
	assert(id);
	data::List id_list = id->Evaluate(eval_context);
	assert(id_list.size() == 1);
	std::string& id_str = id_list.at(0);

	auto var_opt_content = eval_context.variable_scope.Find(id_str);

	if (!var_opt_content) {
		std::stringstream err_msg;
		err_msg << id_str << " was accessed in " << String()
				<< " but was never assigned a value";
		Warning(eval_context, err_msg.str());
	}

	const data::List& var_content = var_opt_content.value();

	if (subscript) {
		data::List sub_list = subscript->Evaluate(eval_context);
		assert(sub_list.size() == 1);
		std::string& sub_str = sub_list.at(0);

		return DoSubscript(eval_context, sub_str, var_content);
	} else {
		return var_content;
	}
}

std::string
Variable::String() const
{
	return content;
}

NodeDump
Variable::Dump() const
{
	std::vector<std::reference_wrapper<const Node>> children;
	assert(id);
	children.push_back(*id);

	if (subscript)
		children.push_back(*subscript);

	return {.type = type, .content = content, .children = std::move(children)};
}

data::List
Variable::DoSubscript(
	EvaluationContext& eval_context,
	const std::string& subscript_str,
	const data::List& content
) const
{
	auto invalidSubscriptError = [&subscript_str, this]()
	{
		std::stringstream err_msg;
		err_msg << "failed to convert " << subscript_str
				<< " to a valid subscript.";
		return Error(err_msg.str());
	};

	auto zeroError = [&subscript_str, this]()
	{
		std::stringstream err_msg;
		err_msg << subscript_str
				<< " contains a 0 index. Hint: Ham arrays start at index 1.";
		return Error(err_msg.str());
	};

	auto outOfBoundsWarning = [&eval_context, &subscript_str, &content, this]()
	{
		std::stringstream err_msg;
		err_msg << "the subscript " << subscript_str
				<< " is out of range for a list of size " << content.size();
		Warning(eval_context, err_msg.str());
	};

	auto parseNum = [invalidSubscriptError](const std::string& str)
	{
		try {
			std::size_t pos;
			int n = std::stoi(str, &pos);
			if (pos != str.size())
				throw invalidSubscriptError();
			return n;

		} catch (std::invalid_argument) {
			throw invalidSubscriptError();
		}
	};

	auto separator = subscript_str.find('-');

	// prevent negative numbers from being parsed by only allowing one '-'
	if (subscript_str.find('-', separator + 1) != std::string::npos) {
		throw invalidSubscriptError();
	}

	// start/end are Ham indexed; array starts at 1 and end is inclusive.
	// NOTE: Ham only _warns_ on out of bound accesses.
	std::size_t start;
	std::size_t end;
	if (separator == std::string::npos) {
		// [<integer>]
		start = parseNum(subscript_str);
		end = start;
	} else if (separator == subscript_str.size() - 1) {
		// [<integer>-]
		start = parseNum(subscript_str.substr(0, subscript_str.size() - 1));
		end = content.size();
	} else {
		start = parseNum(subscript_str.substr(0, separator));
		end = parseNum(subscript_str.substr(separator + 1));
	}

	if (start == 0 || end == 0) {
		throw zeroError();
	}

	if (start > content.size() || end > content.size() || start > end) {
		outOfBoundsWarning();
		return {};
	}

	assert(start > 0 && end > 0);

	// Ham uses indexes that start at 1 and are inclusive at the end. All
	// subscripts should be converted to start at 0 and have an exclusive
	// end (i.e. subtract 1 from start, keep end).
	return data::List(content.begin() + start - 1, content.begin() + end);
}

} // namespace ham::code
