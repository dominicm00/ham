#ifndef HAM_PARSE_GRAMMAR_HPP
#define HAM_PARSE_GRAMMAR_HPP

#include "tao/pegtl.hpp"
#include "tao/pegtl/ascii.hpp"
#include "tao/pegtl/contrib/parse_tree.hpp"
#include "tao/pegtl/internal/pegtl_string.hpp"
#include "tao/pegtl/rules.hpp"

#include <initializer_list>
#include <memory>

namespace p = tao::pegtl;

namespace ham::code
{

/**
 * Identifier: [a-zA-Z0-9]+
 *
 * TODO: Support Unicode identifiers?
 */
struct Identifier : p::plus<p::alnum> {};

/**
 * Whitespace: space+
 */
struct Whitespace : p::plus<p::space> {};

/**
 * Tokens: Separate each rule with Whitespace
 */
template<typename Rule, typename... Rules>
struct Tokens : p::seq<Rule, p::seq<Whitespace, Rules>...> {};

/**
 * Separate: Rule[Separator Rule]*
 */
template<typename Rule, typename Separator>
struct Separate : p::seq<Rule, p::star<Separator, Rule>> {};

/**
 * Variable: "$(" Variable|Identifier[ "[" Subscript "]" ][ ":" PathSelectors ]
 * ")"
 */
struct Variable : p::seq<
					  p::string<'$', '('>,
					  p::opt<Whitespace>,
					  Identifier,
					  p::opt<Whitespace>,
					  p::one<')'>> {};

/**
 * Word: Variable|Identifier
 */
struct Word : p::sor<Variable, Identifier> {};

/**
 * List: Word[ Word]*
 */
struct List : Separate<Word, Whitespace> {};

/**
 * Rule invocation: Identifier[ List[ ":" List]*]
 */
struct _RuleSeparator : p::seq<Whitespace, p::one<':'>, Whitespace> {};

struct RuleInvocation
	: p::seq<Identifier, p::opt<Whitespace, Separate<List, _RuleSeparator>>> {};

/**
 * Statement: return List ;|RuleInvocation ;
 *
 * TODO: Should end-of-line be enforced?
 */
struct Statement : p::sor<
					   Tokens<TAO_PEGTL_STRING("return"), List, p::one<';'>>,
					   Tokens<RuleInvocation, p::one<';'>>> {};

/**
 * Statements: Statement[ Statement]*
 */
struct Statements : Separate<Statement, Whitespace> {};

template<typename Rule>
using Selector = tao::pegtl::parse_tree::selector<
	Rule,
	tao::pegtl::parse_tree::store_content::
		on<Identifier, Variable, List, RuleInvocation, Statement, Statements>>;

} // namespace ham::code

#endif // HAM_PARSE_GRAMMAR_HPP
