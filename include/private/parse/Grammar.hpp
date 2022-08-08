#ifndef HAM_PARSE_GRAMMAR_HPP
#define HAM_PARSE_GRAMMAR_HPP

#include "tao/pegtl.hpp"
#include "tao/pegtl/ascii.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"
#include "tao/pegtl/contrib/separated_seq.hpp"
#include "tao/pegtl/internal/pegtl_string.hpp"
#include "tao/pegtl/nothing.hpp"
#include "tao/pegtl/rules.hpp"

#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <type_traits>

namespace p = tao::pegtl;

namespace ham::parse
{

/* Helper rules excluded from the AST. */
namespace hidden
{
// General
struct special_chars;
struct id_chars;

// Strings
template<typename Quote, typename Escape, typename Nested>
struct quote;
struct word;
struct quoted_single;
struct quoted_double;

// Tokenization
struct whitespace;
template<typename... Rules>
struct tokens;
template<typename... Rules>
struct maybe_tokens;
struct rule_separator;

} // namespace hidden

/**
 * Ham reserves the following special characters:
 * - $      - variables
 * - '      - single quoted strings
 * - "      - double quoted strings
 * - :      - rule separators
 * - |      - N/A
 *
 * When outside a quotation, these characters may only be used in accordance
 * with their special meaning, if they have any.
 */
struct hidden::special_chars : p::one<'$', '\'', '"', '|', ':'> {};

/**
 * Identifier characters: [a-zA-Z0-9/\\_-]
 *
 * Identifiers are composed of id characters with embedded variable expressions.
 *
 * TODO: Support Unicode identifiers?
 */
struct variable;
struct id_char : p::sor<p::alnum, p::one<'/', '\\', '_', '-'>> {};
struct identifier : p::plus<p::sor<variable, id_char>> {};

/** Leafs **/
struct leaf;

// Characters
struct special_escape : p::one<'a', 'b', 'f', 'n', 'r', 't', 'v'> {};
struct char_escape : hidden::special_chars {};
struct string_char : p::sor<p::print, p::space> {};

// A word is a series of printable characters. Cancel if at a special
// character or whitespace.
struct hidden::word
	: p::plus<
		  p::not_at<p::sor<hidden::special_chars, p::space, p::eolf>>,
		  string_char> {};

/**
 * A quoted string is surrounded by quotes, and consists of printable characters
 * (excluding the quote) or character escapes. Escaped quotes are handled by
 * prioritizing escape sequences in matching. The quote is excluded from the
 * character matching. If a starting quote is matched, the quote must end.
 *
 * Parts matching the Nested rule are processed after escapes, but before
 * characters (used for variables).
 */
template<typename Quote, typename Escape, typename Nested>
struct hidden::quote
	: p::if_must<
		  Quote,
		  p::star<p::not_at<Quote>, p::sor<Escape, Nested, string_char>>,
		  Quote> {};

// Escape sequences are "optional" in single quotes. Single quotes ignore
// variables.
struct hidden::quoted_single
	: hidden::
		  quote<p::one<'\''>, p::seq<p::one<'\\'>, char_escape>, p::failure> {};

// Escape sequences are mandatory in double quotes. Double quotes use variables.
struct hidden::quoted_double
	: hidden::quote<
		  p::one<'"'>,
		  p::if_must<p::one<'\\'>, p::sor<char_escape, special_escape>>,
		  p::if_must<p::at<p::one<'$'>>, variable>> {};

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

/**
 * Subscript: "[" DynamicId "]"
 */
struct subscript : hidden::maybe_tokens<p::one<'['>, identifier, p::one<']'>> {
};

/**
 * Variable: "$(" DynamicId[ "[" Subscript "]" ][ ":" VariableModifiers ] ")"
 *
 * TODO: variable modifiers
 */
struct variable : p::if_must<
					  p::one<'$'>,
					  hidden::maybe_tokens<
						  p::one<'('>,
						  identifier,
						  p::opt<subscript>,
						  p::one<')'>>> {};

/**
 * Leaf: (Single-quoted string|Double-quoted string|Variable|Literal)+
 */
struct leaf : p::plus<p::sor<
				  hidden::quoted_single,
				  hidden::quoted_double,
				  variable,
				  hidden::word>> {};

/**
 * List: Leaf[ Leaf]*
 */
struct list : p::list<leaf, hidden::whitespace> {};

/**
 * RuleInvocation: DynamicId[ List[ ":" List]*]
 */
struct hidden::rule_separator
	: p::seq<hidden::whitespace, p::one<':'>, hidden::whitespace> {};

struct rule_invocation
	: p::seq<
		  identifier,
		  p::opt<hidden::whitespace, p::list<list, hidden::rule_separator>>> {};

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
struct statements : p::list<statement, hidden::whitespace> {};

/**
 * Selectors
 */
template<typename Rule>
struct true_selector : std::true_type {};

template<typename Rule>
using selector = p::parse_tree::selector<
	Rule,
	p::parse_tree::store_content::on<
		id_char,
		identifier,
		special_escape,
		char_escape,
		string_char,
		subscript,
		variable,
		leaf,
		rule_invocation,
		statement>,
	p::parse_tree::remove_content::on<statements, list>>;

} // namespace ham::parse

#endif // HAM_PARSE_GRAMMAR_HPP
