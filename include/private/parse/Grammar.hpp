#ifndef HAM_PARSE_GRAMMAR_HPP
#define HAM_PARSE_GRAMMAR_HPP

#include "tao/pegtl.hpp"
#include "tao/pegtl/ascii.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"
#include "tao/pegtl/contrib/separated_seq.hpp"
#include "tao/pegtl/internal/pegtl_string.hpp"
#include "tao/pegtl/rules.hpp"

#include <cstddef>
#include <initializer_list>
#include <memory>
#include <string_view>
#include <type_traits>

namespace p = tao::pegtl;

namespace ham::code
{

template<typename Rule>
struct Selector : std::false_type {};

/**
 * Identifier: [a-zA-Z0-9]+
 *
 * TODO: Support Unicode identifiers?
 */
struct Identifier : p::plus<p::alnum> {};

template<>
struct Selector<Identifier> : std::true_type {};

/**
 * Number: [0-9]+
 */
struct Number : p::plus<p::digit> {};

template<>
struct Selector<Number> : std::true_type {};

/**
 * String: A series of printable characters/escape sequences either between two
 * unescaped double quotes, or with no whitespace.
 */

// Escape characters
struct _HexEscape : p::if_must<p::one<'x', 'X'>, p::xdigit, p::xdigit> {};
struct _OctEscape : p::if_must<p::odigit, p::odigit, p::odigit> {};
struct _CharEscape
	: p::one<'a', 'b', 'f', 'n', 'r', 't', 'v', '\\', '\'', '"'> {};
struct _Escape
	: p::if_must<p::one<'\\'>, p::sor<_CharEscape, _HexEscape, _OctEscape>> {};

// Characters for tokens
struct _TokenCharacter
	: p::sor<_Escape, p::seq<p::not_at<p::space>, p::print>> {};

// Characters for quoted strings (_TokenCharacter + space or tab)
struct _Character : p::sor<p::blank, _TokenCharacter> {};

// If it starts with a quote, then treat it as a quoted string, otherwise it is
// a token.
struct String : p::if_must_else<
					p::one<'"'>,
					p::until<p::one<'"'>, _Character>,
					_TokenCharacter> {};

template<>
struct Selector<String> : std::true_type {};

/**
 * Whitespace: space+
 */
struct _Whitespace : p::plus<p::space> {};

/**
 * Tokens: Separate each rule with Whitespace
 */
template<typename... Rules>
struct _Tokens : p::separated_seq<_Whitespace, Rules...> {};

/**
 * MaybeTokens: Optionally separate each rule with Whitespace
 */
template<typename... Rules>
struct _MaybeTokens : p::separated_seq<p::opt<_Whitespace>, Rules...> {};

struct NumericallyEvaluable;
struct StringEvaluable;

/**
 * Subscript: "[" Number[ "-"[ Number]] "]"
 */
struct EndSubscript
	: p::seq<p::one<'-'>, p::opt<p::opt<_Whitespace>, NumericallyEvaluable>> {};

template<>
struct Selector<EndSubscript> : std::true_type {
	static void transform(std::unique_ptr<p::parse_tree::node>& node)
	{
		if (node->children.size() == 1) {
			// Replace with number if present
			node = std::move(node->children[0]);
		} else {
			// Otherwise remove content
			node->remove_content();
		}
	}
};

struct Subscript : _MaybeTokens<
					   p::one<'['>,
					   NumericallyEvaluable,
					   p::opt<EndSubscript>,
					   p::one<']'>> {};

template<>
struct Selector<Subscript> : std::true_type {};

/**
 * Variable: "$(" Variable|Identifier[ "[" Subscript "]" ][ ":" PathSelectors ]
 * ")"
 */
struct Variable : _MaybeTokens<
					  p::string<'$', '('>,
					  Identifier,
					  p::opt<Subscript>,
					  p::one<')'>> {
	static void transform(std::unique_ptr<p::parse_tree::node>& node) {}
};

template<>
struct Selector<Variable> : std::true_type {};

struct NumericallyEvaluable : p::sor<Variable, Number> {};
struct StringEvaluable : p::sor<Variable, String> {};

/**
 * Word: Variable|Identifier
 */
struct _Word : p::sor<Variable, Identifier> {};

/**
 * List: Word[ Word]*
 */
struct List : p::seq<_Word, p::star<_Whitespace, _Word>> {};

template<>
struct Selector<List> : std::true_type {};

/**
 * RuleInvocation: Identifier[ List[ ":" List]*]
 */
struct _RuleSeparator : p::seq<_Whitespace, p::one<':'>, _Whitespace> {};

struct RuleInvocation
	: p::seq<
		  Identifier,
		  p::opt<_Whitespace, p::separated_seq<_RuleSeparator, List>>> {};

template<>
struct Selector<RuleInvocation> : std::true_type {};

/**
 * Statement: return List ;|RuleInvocation ;
 *
 * TODO: Should end-of-line be enforced?
 */
struct Statement : p::sor<
					   _Tokens<TAO_PEGTL_STRING("return"), List, p::one<';'>>,
					   _Tokens<RuleInvocation, p::one<';'>>> {};

/**
 * Statements: Statement[ Statement]*
 */
struct Statements : p::separated_seq<_Whitespace, Statement> {};

} // namespace ham::code

#endif // HAM_PARSE_GRAMMAR_HPP
