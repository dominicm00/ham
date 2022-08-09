#ifndef HAM_PARSE_GRAMMAR_HPP
#define HAM_PARSE_GRAMMAR_HPP

#include "tao/pegtl.hpp"
#include "tao/pegtl/ascii.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"
#include "tao/pegtl/contrib/separated_seq.hpp"
#include "tao/pegtl/internal/pegtl_string.hpp"
#include "tao/pegtl/nothing.hpp"
#include "tao/pegtl/rules.hpp"
#include "tao/pegtl/type_list.hpp"

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

// Blocks
struct bracketed_block;

} // namespace hidden

/**
 * Ham reserves the following special characters:
 * - $      - variables
 * - '      - single quoted strings
 * - "      - double quoted strings
 * - :      - rule separators
 * - ;      - statement separators
 * - { }    - block delimiters
 * - |      - N/A
 *
 * When outside a quotation, these characters may only be used in accordance
 * with their special meaning, if they have any.
 */
struct hidden::special_chars : p::one<'$', '\'', '"', ':', ';', '|', '{', '}'> {
};

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

struct hidden::whitespace : p::plus<p::space> {};

/** Leafs **/
struct leaf;

// Characters
struct special_escape : p::one<'a', 'b', 'f', 'n', 'r', 't', 'v'> {};
struct char_escape : p::one<'$', '\'', '"'> {};
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

// Single quotes don't accept escapes or nested variables
struct hidden::quoted_single
	: hidden::quote<p::one<'\''>, p::failure, p::failure> {};

// Escape sequences are mandatory in double quotes. Double quotes use variables.
struct hidden::quoted_double
	: hidden::quote<
		  p::one<'"'>,
		  // if character after $ is not ( it must be a valid escape sequence
		  p::seq<
			  p::one<'$'>,
			  p::if_must<
				  p::at<p::not_one<'('>>,
				  p::sor<char_escape, special_escape>>>,
		  // if at $( it must be a variable
		  p::if_must<p::at<p::string<'$', '('>>, variable>> {};

/**
 * Separate each rule with whitespace
 */
template<typename... Rules>
struct hidden::tokens : p::separated_seq<hidden::whitespace, Rules...> {};

/**
 * Optionally separate each rule with Whitespace
 */
template<typename... Rules>
struct hidden::maybe_tokens
	: p::separated_seq<p::opt<hidden::whitespace>, Rules...> {};

/**
 * subscript: '[' <identifier> ']'
 */
struct subscript : hidden::maybe_tokens<p::one<'['>, identifier, p::one<']'>> {
};

/**
 * variable: $(<identifier>[<subscript>][<variable_modifiers>])
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
 * leaf: (<single-quoted string>|<double-quoted string>|<variable>|<word>)+
 */
struct leaf : p::plus<p::sor<
				  hidden::quoted_single,
				  hidden::quoted_double,
				  variable,
				  hidden::word>> {};

/**
 * list: <leaf>[ <leaf>]*
 */
struct list : p::list<leaf, hidden::whitespace> {};

/**
 * rule_invocation: <identifier>[ <list>[ : <list>]*]
 */
struct rule_separator : p::one<':'> {};

struct rule_invocation
	: p::seq<
		  identifier,
		  p::opt<
			  hidden::whitespace,
			  p::list<p::sor<rule_separator, list>, hidden::whitespace>>> {};

/**
 * statement: <rule_invocation>
 */
struct statement : p::sor<rule_invocation> {};

struct statement_block;
struct hidden::bracketed_block
	: p::sor<
		  hidden::tokens<p::one<'{'>, p::opt<statement_block>, p::one<'}'>>,
		  hidden::tokens<p::one<'{'>, p::one<'}'>>> {};

/**
 * rule_signature: rule <identifier> [<identifier> (: <identifier)*]
 */
struct rule_signature : p::seq<
							TAO_PEGTL_STRING("rule"),
							hidden::whitespace,
							identifier,
							p::opt<
								hidden::whitespace,
								p::list<
									identifier,
									p::seq<
										hidden::whitespace,
										p::one<':'>,
										hidden::whitespace>>>> {};

struct rule_definition
	: hidden::tokens<rule_signature, hidden::bracketed_block> {};

// action escape chars
struct action_escape : p::one<'$', '}'> {};
struct action_string : p::plus<p::not_one<'$', '}'>> {};
struct action_definition : p::seq<
							   // action(s)
							   TAO_PEGTL_STRING("action"),
							   p::opt<p::one<'s'>>,
							   hidden::whitespace,

							   identifier,
							   hidden::whitespace,

							   p::one<'{'>,
							   p::opt<hidden::whitespace>,

							   p::star<p::sor<
								   // check escape sequence
								   p::seq<p::one<'$'>, action_escape>,
								   // otherwise $ must be a variable
								   p::if_must<p::at<p::one<'$'>>, variable>,
								   // otherwise capture the string
								   action_string>>,

							   p::must<p::one<'}'>>> {};

struct statement_block : p::list<
							 p::sor<
								 hidden::tokens<statement, p::one<';'>>,
								 rule_definition,
								 action_definition>,
							 hidden::whitespace> {};

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
		rule_signature,
		action_escape,
		action_string>,
	p::parse_tree::remove_content::on<
		statement_block,
		list,
		rule_separator,
		rule_definition,
		action_definition>>;

} // namespace ham::parse

#endif // HAM_PARSE_GRAMMAR_HPP
