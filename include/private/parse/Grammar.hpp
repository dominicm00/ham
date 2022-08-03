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
struct grouping_chars;

// Strings
template<typename Quote, typename Escape, typename Nested>
struct quote;
struct literal;
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
 * When unescaped, these characters may only be used in accordance with their
 * special meaning, if they have any. When escaped, they can be used in words.
 * They can never be used in identifiers.
 *
 * The following special characters are allowed in words (and possibly
 * expressions), but not identifiers:
 * - ( )
 * - [ ]
 * - { }
 * - < >
 *
 * In words, these characters may have special meaning at runtime, but do not
 * have meaning to the parser.
 */
struct hidden::special_chars : p::one<'$', '\'', '"', '|', ':'> {};
struct hidden::grouping_chars : p::one<'(', ')', '{', '}', '[', ']', '<', '>'> {
};

/**
 * Identifiers are consecutive characters that are not whitespace or symbols
 * reserved by Ham.
 *
 * TODO: Support Unicode identifiers?
 */
struct identifier
	: p::plus<
		  p::not_at<
			  p::sor<p::space, hidden::special_chars, hidden::grouping_chars>>,
		  p::print> {};

/**
 * Integer: [0-9]+
 */
struct integer : p::plus<p::digit> {};

/** Words **/
struct variable;
struct word;

// Characters
struct special_escape : p::one<'a', 'b', 'f', 'n', 'r', 't', 'v'> {};
struct char_escape : hidden::special_chars {};
struct string_char : p::sor<p::print, p::space> {};

// A literal is a series of printable characters. Cancel if at a special
// character or whitespace.
struct hidden::literal
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

struct evaluable_num;

/**
 * Subscript: "[" Number[ "-"[ Number]] "]"
 */
struct end_subscript
	: p::seq<p::one<'-'>, p::opt<p::opt<hidden::whitespace>, evaluable_num>> {};

struct subscript : hidden::maybe_tokens<
					   p::one<'['>,
					   evaluable_num,
					   p::opt<end_subscript>,
					   p::one<']'>> {};

/**
 * Variable: "$(" Variable|Identifier[ "[" Subscript "]" ][ ":" PathSelectors ]
 * ")"
 */
struct variable : p::if_must<
					  p::one<'$'>,
					  hidden::maybe_tokens<
						  p::one<'('>,
						  p::sor<variable, identifier>,
						  p::opt<subscript>,
						  p::one<')'>>> {};

struct evaluable_num : p::sor<variable, integer> {};

/**
 * Word: (Single-quoted string|Double-quoted string|Variable|Literal)+
 */
struct word : p::plus<p::sor<
				  hidden::quoted_single,
				  hidden::quoted_double,
				  variable,
				  hidden::literal>> {};

/**
 * List: Word[ Word]*
 */
struct list : p::list<word, hidden::whitespace> {};

/**
 * RuleInvocation: Identifier[ List[ ":" List]*]
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
		identifier,
		integer,
		special_escape,
		char_escape,
		string_char,
		evaluable_num,
		subscript,
		variable,
		word,
		rule_invocation,
		statement>,
	p::parse_tree::remove_content::on<statements, list>,
	p::parse_tree::fold_one::on<end_subscript>>;

} // namespace ham::parse

#endif // HAM_PARSE_GRAMMAR_HPP
