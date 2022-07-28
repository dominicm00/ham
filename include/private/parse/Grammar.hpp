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

namespace ham::parse
{

/* Helper rules excluded from the AST. */
namespace hidden
{

// Strings
struct hex_escape;
struct oct_escape;
struct char_escape;
struct escape;
struct token_character;
struct character;

// Tokenization
struct whitespace;
template<typename... Rules>
struct tokens;
template<typename... Rules>
struct maybe_tokens;
struct rule_separator;

// Objects
struct word;

} // namespace hidden

template<typename Rule>
struct selector : std::false_type {};

/**
 * Identifier: [a-zA-Z0-9]+
 *
 * TODO: Support Unicode identifiers?
 */
struct identifier : p::plus<p::alnum> {};

template<>
struct selector<identifier> : std::true_type {};

/**
 * Number: [0-9]+
 */
struct number : p::plus<p::digit> {};

template<>
struct selector<number> : std::true_type {};

/**
 * String: A series of printable characters/escape sequences either between two
 * unescaped double quotes, or with no whitespace.
 */

// Escape characters
struct hidden::hex_escape : p::if_must<p::one<'x', 'X'>, p::xdigit, p::xdigit> {
};
struct hidden::oct_escape : p::if_must<p::odigit, p::odigit, p::odigit> {};
struct hidden::char_escape
	: p::one<'a', 'b', 'f', 'n', 'r', 't', 'v', '\\', '\'', '"'> {};
struct hidden::escape
	: p::if_must<
		  p::one<'\\'>,
		  p::sor<hidden::char_escape, hidden::hex_escape, hidden::oct_escape>> {
};

// Characters for tokens
struct hidden::token_character
	: p::sor<hidden::escape, p::seq<p::not_at<p::space>, p::print>> {};

// Characters for quoted strings (_TokenCharacter + space or tab)
struct hidden::character : p::sor<p::blank, hidden::token_character> {};

// If it starts with a quote, then treat it as a quoted string, otherwise it is
// a token.
struct string : p::if_must_else<
					p::one<'"'>,
					p::until<p::one<'"'>, hidden::character>,
					hidden::token_character> {};

template<>
struct selector<string> : std::true_type {};

/**
 * Whitespace: space+
 */
struct hidden::whitespace : p::plus<p::space> {};

/**
 * Tokens: Separate each rule with Whitespace
 */
template<typename... Rules>
struct hidden::tokens : p::separated_seq<hidden::whitespace, Rules...> {};

/**
 * MaybeTokens: Optionally separate each rule with Whitespace
 */
template<typename... Rules>
struct hidden::maybe_tokens
	: p::separated_seq<p::opt<hidden::whitespace>, Rules...> {};

struct evaluable_num;
struct evaluable_string;

/**
 * Subscript: "[" Number[ "-"[ Number]] "]"
 */
struct end_subscript
	: p::seq<p::one<'-'>, p::opt<p::opt<hidden::whitespace>, evaluable_num>> {};

template<>
struct selector<end_subscript> : std::true_type {
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

struct subscript : hidden::maybe_tokens<
					   p::one<'['>,
					   evaluable_num,
					   p::opt<end_subscript>,
					   p::one<']'>> {};

template<>
struct selector<subscript> : std::true_type {};

/**
 * Variable: "$(" Variable|Identifier[ "[" Subscript "]" ][ ":" PathSelectors ]
 * ")"
 */
struct variable : hidden::maybe_tokens<
					  p::string<'$', '('>,
					  identifier,
					  p::opt<subscript>,
					  p::one<')'>> {
	static void transform(std::unique_ptr<p::parse_tree::node>& node) {}
};

template<>
struct selector<variable> : std::true_type {};

struct evaluable_num : p::sor<variable, number> {};
struct evaluable_string : p::sor<variable, string> {};

/**
 * Word: Variable|Identifier
 */
struct hidden::word : p::sor<variable, identifier> {};

/**
 * List: Word[ Word]*
 */
struct list : p::seq<hidden::word, p::star<hidden::whitespace, hidden::word>> {
};

template<>
struct selector<list> : std::true_type {};

/**
 * RuleInvocation: Identifier[ List[ ":" List]*]
 */
struct hidden::rule_separator
	: p::seq<hidden::whitespace, p::one<':'>, hidden::whitespace> {};

struct rule_invocation
	: p::seq<
		  identifier,
		  p::opt<
			  hidden::whitespace,
			  p::separated_seq<hidden::rule_separator, list>>> {};

template<>
struct selector<rule_invocation> : std::true_type {};

/**
 * Statement: return List ;|RuleInvocation ;
 *
 * TODO: Should end-of-line be enforced?
 */
struct statement
	: p::sor<
		  hidden::tokens<TAO_PEGTL_STRING("return"), list, p::one<';'>>,
		  hidden::tokens<rule_invocation, p::one<';'>>> {};

/**
 * Statements: Statement[ Statement]*
 */
struct statements : p::separated_seq<hidden::whitespace, statement> {};

} // namespace ham::parse

#endif // HAM_PARSE_GRAMMAR_HPP
