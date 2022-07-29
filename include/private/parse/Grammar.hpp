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
// Strings
template<typename Quote, typename Escape>
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

// Objects
struct word;

} // namespace hidden

/**
 * Identifier: [a-zA-Z0-9]+
 *
 * TODO: Support Unicode identifiers?
 */
struct identifier : p::plus<p::alnum> {};

/**
 * Number: [0-9]+
 */
struct integer : p::plus<p::digit> {};

/**
 * String: A series of printable characters/escape sequences either between two
 * unescaped quotes, or with no whitespace.
 */

// Characters
struct special_escape : p::one<'a', 'b', 'f', 'n', 'r', 't', 'v'> {};
struct char_escape : p::one<'\\', '\'', '"'> {};
struct string_char : p::sor<p::print, p::space> {};

// A literal is a series of consecutive characters, excluding quotes
struct hidden::literal
	: p::plus<p::not_at<p::sor<p::space, p::one<'\'', '"'>>>, string_char> {};

// A quoted string is surrounded by quotes, and consists of printable characters
// (excluding the quote) or character escapes. Escaped quotes are handled by
// prioritizing escape sequences in matching. The quote is excluded from the
// character matching. If a starting quote is matched, the quote must end.
template<typename Quote, typename Escape>
struct hidden::quote
	: p::if_must<
		  Quote,
		  p::star<p::not_at<Quote>, p::sor<Escape, string_char>>,
		  Quote> {};

// Escape sequences are "optional" in single quotes
struct hidden::quoted_single
	: hidden::quote<p::one<'\''>, p::seq<p::one<'\\'>, char_escape>> {};
// Escape sequences are mandatory in double quotes
struct hidden::quoted_double
	: hidden::quote<
		  p::one<'"'>,
		  p::if_must<p::one<'\\'>, p::sor<char_escape, special_escape>>> {};

struct string : p::plus<p::sor<
					hidden::literal,
					hidden::quoted_double,
					hidden::quoted_single>> {};

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

struct subscript : hidden::maybe_tokens<
					   p::one<'['>,
					   evaluable_num,
					   p::opt<end_subscript>,
					   p::one<']'>> {};

/**
 * Variable: "$(" Variable|Identifier[ "[" Subscript "]" ][ ":" PathSelectors ]
 * ")"
 */
struct variable : hidden::maybe_tokens<
					  p::string<'$', '('>,
					  identifier,
					  p::opt<subscript>,
					  p::one<')'>> {};

struct evaluable_num : p::sor<variable, integer> {};
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
		string,
		evaluable_num,
		evaluable_string,
		subscript,
		variable,
		rule_invocation,
		statement>,
	p::parse_tree::remove_content::on<statements, list>,
	p::parse_tree::fold_one::on<end_subscript>>;

} // namespace ham::parse

#endif // HAM_PARSE_GRAMMAR_HPP
