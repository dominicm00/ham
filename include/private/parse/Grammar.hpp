#ifndef HAM_PARSE_GRAMMAR_HPP
#define HAM_PARSE_GRAMMAR_HPP

#include "parse/ParseException.hpp"
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

template<unsigned char Error, typename Predicate, typename... Rule>
struct if_must_else_error
	: p::if_then_else<
		  Predicate,
		  p::sor<p::seq<Rule...>, p::raise_message<Error>>,
		  p::failure> {};

template<unsigned char Error, typename... Rule>
struct must_else_error : p::sor<p::seq<Rule...>, p::raise_message<Error>> {};

/**
 * Ham reserves the following special characters:
 * - $ ( )  - variables
 * - '      - single quoted strings
 * - "      - double quoted strings
 * - :      - rule separators
 * - ;      - statement separators
 * - { }    - block delimiters
 * - [ ]    - bracket expressions
 * - #      - comments
 * - |      - N/A
 *
 * When outside a quotation, these characters may only be used in accordance
 * with their special meaning, if they have any.
 */
struct SpecialChars
	: p::one<'$', '\'', '"', ':', ';', '{', '}', '(', ')', '[', ']', '#', '|'> {
};

/**
 * Identifier characters: [a-zA-Z0-9/\\_-]
 *
 * Identifiers are composed of id characters with embedded variable expressions.
 *
 * TODO: Support Unicode identifiers?
 */
struct Variable;
struct IdChar : p::sor<p::alnum, p::one<'/', '\\', '_', '-'>> {};
struct Identifier : p::plus<p::sor<Variable, IdChar>> {};

struct Whitespace : p::plus<p::space> {};

/** Leafs **/
struct Leaf;

// Characters
struct SpecialEscape : p::one<'a', 'b', 'f', 'n', 'r', 't', 'v'> {};
struct CharEscape : p::one<'$', '\'', '"'> {};

// A word is a series of printable characters. Cancel if at a special
// character or whitespace.
struct Word
	: p::plus<p::not_at<p::sor<SpecialChars, p::space, p::eolf>>, p::print> {};

/**
 * A quoted string is surrounded by quotes, and consists of printable characters
 * (excluding the quote) or character escapes. Escaped quotes are handled by
 * prioritizing escape sequences in matching. The quote is excluded from the
 * character matching. If a starting quote is matched, the quote must end.
 *
 * Parts matching the Nested rule are processed after escapes, but before
 * characters (used for variables).
 */
template<typename QuoteChar, typename Escape, typename Nested, typename Char>
struct Quote : p::if_must<
				   QuoteChar,
				   p::star<p::not_at<QuoteChar>, p::sor<Escape, Nested, Char>>,
				   QuoteChar> {};

// Single quotes don't accept escapes or nested variables
struct QuotedSingleContent
	: p::star<p::not_at<p::one<'\''>>, p::sor<p::print, p::space>> {};
struct QuotedSingle : p::seq<p::one<'\''>, QuotedSingleContent, p::one<'\''>> {
};

// Escape sequences are mandatory in double quotes. Double quotes use variables.
// Double quotes are parsed character by character.
struct QuotedChar : p::sor<p::print, p::space> {};
struct QuotedDouble
	: Quote<
		  p::one<'"'>,
		  // if character after $ is not ( it must be a valid escape sequence
		  p::seq<
			  p::one<'$'>,
			  p::if_must<
				  p::at<p::not_one<'('>>,
				  p::sor<CharEscape, SpecialEscape>>>,
		  // if at $( it must be a variable
		  p::if_must<p::at<p::string<'$', '('>>, Variable>,
		  QuotedChar> {};

/**
 * Separate each rule with whitespace
 */
template<typename... Rules>
struct Tokens : p::separated_seq<Whitespace, Rules...> {};

/**
 * Optionally separate each rule with Whitespace
 */
template<typename... Rules>
struct MaybeTokens : p::separated_seq<p::opt<Whitespace>, Rules...> {};

/**
 * subscript: '[' <identifier> ']'
 */
struct Subscript : if_must_else_error<
					   INVALID_SUBSCRIPT,
					   p::one<'['>,
					   Identifier,
					   p::one<']'>> {};

/**
 * variable_modifiers: (:<selectors*><replacer?>)*
 */
struct VariableSelector : p::alpha {};
struct VariableReplacer
	: p::seq<
		  VariableSelector,
		  if_must_else_error<INVALID_REPLACER_ARGUMENT, p::one<'='>, Leaf>> {};
struct VariableModSequence
	: p::seq<
		  p::star<VariableSelector, p::not_at<p::one<'='>>>,
		  p::opt<VariableReplacer>> {};
struct VariableModifiers : p::star<if_must_else_error<
							   MISSING_VARIABLE_MODIFIER,
							   p::one<':'>,
							   p::at<p::alpha>,
							   VariableModSequence>> {};

/**
 * variable: $(<identifier>[<subscript>][<variable_modifiers>])
 *
 * TODO: variable modifiers
 */
struct Variable : p::seq<
					  p::one<'$'>,
					  MaybeTokens<
						  p::one<'('>,
						  Identifier,
						  p::opt<Subscript>,
						  p::opt<VariableModifiers>,
						  p::one<')'>>> {};

struct RuleInvocation;
struct TargetRuleInvocation
	: Tokens<
		  TAO_PEGTL_STRING("on"),
		  must_else_error<TARGET_STATEMENT_MISSING_TARGET, Leaf>,
		  RuleInvocation> {};
struct BracketExpression
	: if_must_else_error<
		  INVALID_BRACKET_EXPRESSION,
		  p::one<'['>,
		  Whitespace,
		  p::sor<TargetRuleInvocation, RuleInvocation>,
		  Whitespace,
		  must_else_error<MISSING_CLOSING_SQUARE_BRACE, p::one<']'>>> {};

struct Leaf : p::sor<
				  BracketExpression,
				  p::plus<p::sor<QuotedSingle, QuotedDouble, Variable, Word>>> {
};

/**
 * list: <leaf>[ <leaf>]*
 */
struct List : p::list<Leaf, Whitespace> {};

/**
 * rule_invocation: <identifier>[ <list>[ : <list>]*]
 */
struct RuleSeparator : p::one<':'> {};

struct RuleInvocation
	: p::seq<
		  Identifier,
		  p::opt<
			  Whitespace,
			  p::list<p::sor<RuleSeparator, List>, Whitespace>>> {};

struct Statement;
struct StatementBlock;
struct EmptyBlock : p::success {};
struct BracketedBlock
	: if_must_else_error<
		  INVALID_BRACKET_BLOCK,
		  p::one<'{'>,
		  Whitespace,
		  p::sor<StatementBlock, EmptyBlock>,
		  p::opt<Whitespace>,
		  must_else_error<MISSING_CLOSING_CURLY_BRACE, p::one<'}'>>> {};

/**
 * rule_signature: rule <identifier> [<identifier> (: <identifier)*]
 */
struct RuleSignature
	: p::seq<
		  TAO_PEGTL_STRING("rule"),
		  Whitespace,
		  must_else_error<INVALID_IDENTIFIER, Identifier>,
		  p::opt<
			  Whitespace,
			  p::list<
				  Identifier,
				  p::seq<Whitespace, p::one<':'>, Whitespace>>>> {};

struct RuleDefinition
	: Tokens<
		  RuleSignature,
		  must_else_error<RULE_DEF_MISSING_BLOCK, BracketedBlock>> {};

// action escape chars
struct ActionEscape : p::one<'$', '}'> {};
struct ActionString : p::plus<p::not_one<'$', '}'>> {};
struct ActionDefinition
	: p::seq<
		  // action(s)
		  TAO_PEGTL_STRING("action"),
		  p::opt<p::one<'s'>>,
		  Whitespace,

		  Identifier,
		  Whitespace,

		  p::one<'{'>,
		  p::opt<Whitespace>,

		  p::star<p::sor<
			  // check escape sequence
			  p::seq<p::one<'$'>, ActionEscape>,
			  // otherwise $ must be a variable
			  if_must_else_error<
				  MISSING_VARIABLE_EXPRESSION_OR_ESCAPE,
				  p::at<p::one<'$'>>,
				  Variable>,
			  // otherwise capture the string
			  ActionString>>,

		  must_else_error<MISSING_CLOSING_CURLY_BRACE, p::one<'}'>>> {};

/**
 * Conditions
 */
struct Condition;
struct ConditionLeaf;
struct LeafComparator : p::sor<
							p::string<'<', '='>,
							p::string<'>', '='>,
							p::string<'!', '='>,
							p::string<'i', 'n'>,
							p::string<'='>,
							p::string<'<'>,
							p::string<'>'>> {};
struct LogicalAnd : p::string<'&', '&'> {};
struct LogicalOr : p::string<'|', '|'> {};
struct LogicalNot : p::one<'!'> {};

struct BoolExpression : p::seq<
							Leaf,
							p::opt<p::if_must<
								p::seq<Whitespace, LeafComparator>,
								Whitespace,
								Leaf>>> {};

struct ConditionLeaf
	: p::seq<
		  p::opt<LogicalNot, Whitespace>,
		  p::sor<
			  if_must_else_error<
				  MISSING_GROUPED_CONDITIONAL,
				  p::one<'('>,
				  Whitespace,
				  Condition,
				  Whitespace,
				  must_else_error<MISSING_CLOSING_PAREN, p::one<')'>>>,
			  BoolExpression>> {};

template<
	typename Cond,
	unsigned char MissingConditional,
	unsigned char MixedOp,
	typename Op,
	typename OtherOp>
struct BoolOp
	: p::seq<
		  must_else_error<MissingConditional, ConditionLeaf>,
		  p::opt<
			  Whitespace,
			  p::if_then_else<p::at<OtherOp>, p::raise_message<MixedOp>, Op>,
			  Whitespace,
			  Cond>> {};

struct ConditionConjunction : BoolOp<
								  ConditionConjunction,
								  MISSING_CONDITIONAL_AFTER_CONJUNCTION,
								  DISJUNCTION_IN_CONJUNCTION,
								  LogicalAnd,
								  LogicalOr> {};

struct ConditionDisjunction : BoolOp<
								  ConditionDisjunction,
								  MISSING_CONDITIONAL_AFTER_DISJUNCTION,
								  CONJUNCTION_IN_DISJUNCTION,
								  LogicalOr,
								  LogicalAnd> {};

struct Condition : p::seq<
					   ConditionLeaf,
					   p::opt<
						   Whitespace,
						   p::sor<
							   Tokens<LogicalAnd, ConditionConjunction>,
							   Tokens<LogicalOr, ConditionDisjunction>>>> {};

struct RearrangeUnaryOperator : p::parse_tree::apply<RearrangeUnaryOperator> {
	template<typename Node, typename... States>
	static void transform(std::unique_ptr<Node>& n, States&&... st)
	{
		if (n->children.size() == 2) {
			// Make operands child of operator if present
			auto operand = std::move(n->children.back());
			n = std::move(n->children[0]);
			n->children.emplace_back(std::move(operand));
		} else {
			// Otherwise replace with first child
			n = std::move(n->children[0]);
		}
	}
};

struct RearrangeBinaryOperator : p::parse_tree::apply<RearrangeBinaryOperator> {
	template<typename Node, typename... States>
	static void transform(std::unique_ptr<Node>& n, States&&... st)
	{
		if (n->children.size() == 3) {
			// Make operands child of operator if present
			auto left = std::move(n->children.front());
			auto right = std::move(n->children.back());
			n = std::move(n->children[1]);
			n->children.emplace_back(std::move(left));
			n->children.emplace_back(std::move(right));
		} else {
			// Otherwise replace with first child
			n = std::move(n->children[0]);
		}
	}
};

/**
 * Control flow
 */
struct IfStatement
	: p::seq<
		  TAO_PEGTL_STRING("if"),
		  Whitespace,
		  must_else_error<IF_MISSING_CONDITION, Condition>,
		  Whitespace,
		  must_else_error<IF_MISSING_BLOCK, BracketedBlock>,
		  p::opt<
			  Whitespace,
			  TAO_PEGTL_STRING("else"),
			  Whitespace,
			  must_else_error<ELSE_MISSING_BLOCK, BracketedBlock>>> {};

struct WhileLoop : Tokens<
					   TAO_PEGTL_STRING("while"),
					   must_else_error<WHILE_MISSING_CONDITION, Condition>,
					   must_else_error<WHILE_MISSING_BLOCK, BracketedBlock>> {};

struct ForLoop : Tokens<
					 TAO_PEGTL_STRING("for"),
					 must_else_error<FOR_MISSING_IDENTIFIER, Identifier>,
					 must_else_error<FOR_MISSING_IN, TAO_PEGTL_STRING("in")>,
					 must_else_error<FOR_MISSING_LEAF, Leaf>,
					 must_else_error<FOR_MISSING_BLOCK, BracketedBlock>> {};

struct Definition : p::sor<RuleDefinition, ActionDefinition> {};
struct ControlFlow : p::sor<IfStatement, WhileLoop, ForLoop> {};
struct RuleStatement : Tokens<RuleInvocation, p::one<';'>> {};
struct TargetStatement
	: Tokens<
		  TAO_PEGTL_STRING("on"),
		  must_else_error<TARGET_STATEMENT_MISSING_TARGET, Leaf>,
		  p::sor<ControlFlow, RuleStatement>> {};
struct StatementBlock
	: p::list<
		  p::sor<Definition, ControlFlow, TargetStatement, RuleStatement>,
		  Whitespace> {};

struct HamGrammar : p::must<p::seq<StatementBlock, p::eolf>> {};

/**
 * Selectors
 */
template<typename Rule>
struct AllSelector : std::true_type {};

template<typename Rule>
using Selector = p::parse_tree::selector<
	Rule,
	p::parse_tree::store_content::on<
		ActionDefinition,
		ActionEscape,
		ActionString,
		CharEscape,
		EmptyBlock,
		ForLoop,
		IdChar,
		Identifier,
		IfStatement,
		Leaf,
		LeafComparator,
		List,
		LogicalAnd,
		LogicalNot,
		LogicalOr,
		QuotedChar,
		QuotedDouble,
		QuotedSingleContent,
		RuleDefinition,
		RuleInvocation,
		RuleSeparator,
		RuleSignature,
		SpecialEscape,
		StatementBlock,
		Subscript,
		TargetRuleInvocation,
		TargetStatement,
		Variable,
		VariableReplacer,
		VariableSelector,
		WhileLoop,
		Word>,
	RearrangeBinaryOperator::on<
		BoolExpression,
		ConditionConjunction,
		ConditionDisjunction,
		Condition>,
	RearrangeUnaryOperator::on<ConditionLeaf>>;

} // namespace ham::parse

#endif // HAM_PARSE_GRAMMAR_HPP
