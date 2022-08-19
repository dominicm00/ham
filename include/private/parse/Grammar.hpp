#ifndef HAM_PARSE_GRAMMAR_HPP
#define HAM_PARSE_GRAMMAR_HPP

#include "data/Types.hpp"
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

template<typename>
inline constexpr const char* error_message = nullptr;

// Paired rules
struct OpenParen : p::one<'('> {};
template<>
inline constexpr auto error_message<OpenParen> = "expected opening (";
struct CloseParen : p::one<')'> {};
template<>
inline constexpr auto error_message<CloseParen> = "expected closing )";

struct OpenCurlyBrace : p::one<'{'> {};
template<>
inline constexpr auto error_message<OpenCurlyBrace> = "expected opening {";
struct CloseCurlyBrace : p::one<'}'> {};
template<>
inline constexpr auto error_message<CloseCurlyBrace> = "expected closing }";

struct OpenSquareBracket : p::one<'['> {};
template<>
inline constexpr auto error_message<OpenSquareBracket> = "expected opening [";
struct CloseSquareBracket : p::one<']'> {};
template<>
inline constexpr auto error_message<CloseSquareBracket> = "expected closing ]";

struct OpenSingleQuote : p::one<'\''> {};
template<>
inline constexpr auto error_message<OpenSingleQuote> = "expected opening '";
struct CloseSingleQuote : p::one<'\''> {};
template<>
inline constexpr auto error_message<CloseSingleQuote> = "expected closing '";

struct OpenDoubleQuote : p::one<'"'> {};
template<>
inline constexpr auto error_message<OpenDoubleQuote> = "expected opening \"";
struct CloseDoubleQuote : p::one<'"'> {};
template<>
inline constexpr auto error_message<CloseDoubleQuote> = "expected closing \"";

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
 * - ! & |  - boolean expressions
 * - = + ?  - variable assignment
 *
 * When outside a quotation, these characters may only be used in accordance
 * with their special meaning, if they have any.
 */
struct SpecialChars : p::one<
						  '$',
						  '\'',
						  '"',
						  ':',
						  ';',
						  '{',
						  '}',
						  '(',
						  ')',
						  '[',
						  ']',
						  '#',
						  '!',
						  '&',
						  '|',
						  '=',
						  '+',
						  '?'> {};

/**
 * Identifier characters: [a-zA-Z0-9/\\_-]
 *
 * Identifiers are composed of id characters with embedded variable expressions.
 *
 * TODO: Support Unicode identifiers?
 */
struct Variable;
struct IdChar : p::sor<p::alnum, p::one<'/', '\\', '_', '-'>> {};
struct Identifier : p::plus<p::sor<IdChar, Variable>> {};
template<>
inline constexpr auto error_message<Identifier> = "expected identifier";

struct Whitespace : p::plus<p::space> {};
template<>
inline constexpr auto error_message<Whitespace> = "expected whitespace";

/** Leafs **/
struct Leaf;

// Characters
struct SpecialEscape : p::one<'a', 'b', 'f', 'n', 'r', 't', 'v'> {};
struct CharEscape : p::one<'$', '\'', '"'> {};

// A word is a series of printable characters. Cancel if at a special
// character or whitespace.
struct Word
	: p::plus<p::not_at<p::sor<SpecialChars, p::space, p::eolf>>, p::print> {};

// Single quotes don't accept escapes or nested variables
struct QuotedSingleContent
	: p::star<p::not_at<p::one<'\''>>, p::sor<p::print, p::space>> {};
struct QuotedSingle
	: p::seq<OpenSingleQuote, QuotedSingleContent, p::must<CloseSingleQuote>> {
};

struct QuotedChar : p::seq<p::not_at<p::one<'"'>>, p::sor<p::print, p::space>> {
};

struct QuotedEscape : p::sor<CharEscape, SpecialEscape> {};
template<>
inline constexpr auto error_message<QuotedEscape> =
	"unknown escape sequence after '$'";

template<typename Escape>
struct EscapeOrVariable : p::if_then_else<
							  p::at<p::string<'$', '('>>,
							  Variable,
							  p::seq<p::one<'$'>, p::must<Escape>>> {};

// Double quotes are parsed character by character and include escape sequences
struct QuotedDouble
	: p::seq<
		  OpenDoubleQuote,
		  p::star<p::sor<EscapeOrVariable<QuotedEscape>, QuotedChar>>,
		  p::must<CloseDoubleQuote>> {};

/**
 * Optionally separate each rule with Whitespace
 */
template<typename... Rules>
struct MaybeTokens : p::separated_seq<p::opt<Whitespace>, Rules...> {};

/**
 * subscript: '[' <identifier> ']'
 */
struct SubscriptContents : p::seq<Identifier> {};
template<>
inline constexpr auto error_message<SubscriptContents> = "invalid subscript";

struct Subscript
	: p::if_must<OpenSquareBracket, SubscriptContents, CloseSquareBracket> {};

/**
 * variable_modifiers: (:<selectors*><replacer?>)*
 */
struct VariableSelector : p::alpha {};

struct hVariableSelector : p::alpha {};
template<>
inline constexpr auto error_message<hVariableSelector> =
	"expected variable modifier after ':'";

struct VariableReplacerArg : p::seq<Leaf> {};
template<>
inline constexpr auto error_message<VariableReplacerArg> =
	"expected argument after '='";

struct VariableReplacer
	: p::seq<VariableSelector, p::one<'='>, p::must<VariableReplacerArg>> {};
struct VariableModSequence
	: p::seq<
		  p::star<VariableSelector, p::not_at<p::one<'='>>>,
		  p::opt<VariableReplacer>> {};
struct VariableModifiers : p::star<
							   p::one<':'>,
							   p::at<p::must<hVariableSelector>>,
							   VariableModSequence> {};

/**
 * variable: $(<identifier>[<subscript>][<variable_modifiers>])
 */
struct VariableContents : MaybeTokens<
							  OpenParen,
							  p::must<Identifier>,
							  p::opt<Subscript>,
							  p::opt<VariableModifiers>,
							  p::must<CloseParen>> {};
template<>
inline constexpr auto error_message<VariableContents> =
	"expected variable expression after '$'";

struct Variable : p::seq<p::one<'$'>, p::must<VariableContents>> {};

struct RuleInvocation;

struct TargetRuleTarget : p::seq<Leaf> {};
template<>
inline constexpr auto error_message<TargetRuleTarget> =
	"expected target after 'on'";

struct TargetRuleInvocation
	: p::seq<
		  TAO_PEGTL_STRING("on"),
		  Whitespace,
		  p::must<TargetRuleTarget, Whitespace, RuleInvocation>> {};

struct BracketExpressionContents
	: p::sor<TargetRuleInvocation, RuleInvocation> {};
template<>
inline constexpr auto error_message<BracketExpressionContents> =
	"expected (target) rule invocation in '[ ]'";

struct BracketExpression : p::seq<
							   OpenSquareBracket,
							   p::opt<Whitespace>,
							   p::must<BracketExpressionContents>,
							   p::opt<Whitespace>,
							   p::must<CloseSquareBracket>> {};

struct Leaf : p::sor<
				  BracketExpression,
				  p::plus<p::sor<QuotedSingle, QuotedDouble, Variable, Word>>> {
};
template<>
inline constexpr auto error_message<Leaf> = "expected leaf";

/**
 * list: <leaf>[ <leaf>]*
 */
struct List : p::list<Leaf, Whitespace> {};
template<>
inline constexpr auto error_message<List> = "expected list";

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
template<>
inline constexpr auto error_message<RuleInvocation> =
	"expected rule invocation";

struct Statement;
struct StatementBlock;
struct EmptyBlock : p::success {};
struct BracketedBlock
	: p::seq<
		  OpenCurlyBrace,
		  p::must<Whitespace>,
		  p::sor<p::seq<StatementBlock, p::must<Whitespace>>, EmptyBlock>,
		  p::must<CloseCurlyBrace>> {};
template<>
inline constexpr auto error_message<BracketedBlock> =
	"expected a '{ }' statement block";

/**
 * rule_signature: rule <identifier> [<identifier> (: <identifier)*]
 */
struct RuleSignature
	: p::seq<
		  TAO_PEGTL_STRING("rule"),
		  Whitespace,
		  p::must<Identifier>,
		  p::opt<
			  Whitespace,
			  p::list<
				  Identifier,
				  p::seq<Whitespace, p::one<':'>, p::must<Whitespace>>>>> {};

struct RuleDefinition
	: p::seq<RuleSignature, p::opt<Whitespace>, p::must<BracketedBlock>> {};

// action escape chars
struct ActionEscape : p::one<'$', '}'> {};
template<>
inline constexpr auto error_message<ActionEscape> =
	"unknown escape sequence after '$'";

struct ActionString : p::plus<p::not_one<'$', '}'>> {};
struct ActionDefinition
	: p::seq<
		  // action(s)
		  TAO_PEGTL_STRING("action"),
		  p::opt<p::one<'s'>>,
		  Whitespace,

		  p::must<Identifier, Whitespace, OpenCurlyBrace>,
		  p::opt<Whitespace>,

		  p::star<p::sor<EscapeOrVariable<ActionEscape>, ActionString>>,

		  p::must<CloseCurlyBrace>> {};

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
template<>
inline constexpr auto error_message<BoolExpression> =
	"expected a leaf or comparison";

struct ConditionLeaf : p::seq<
						   p::opt<LogicalNot, p::opt<Whitespace>>,
						   p::sor<
							   p::seq<
								   OpenParen,
								   p::opt<Whitespace>,
								   p::must<Condition>,
								   p::opt<Whitespace>,
								   p::must<CloseParen>>,
							   p::must<BoolExpression>>> {};

template<typename Cond, typename Err, typename Op, typename OtherOp>
struct BoolOp : p::seq<
					ConditionLeaf,
					p::opt<
						p::opt<Whitespace>,
						p::if_then_else<p::at<OtherOp>, Err, Op>,
						p::opt<Whitespace>,
						p::must<Cond>>> {};

struct ConditionConjunction
	: BoolOp<
		  ConditionConjunction,
		  TAO_PEGTL_RAISE_MESSAGE(
			  "cannot use disjunction (||) in conjunction (&&)"
		  ),
		  LogicalAnd,
		  LogicalOr> {};
template<>
inline constexpr auto error_message<ConditionConjunction> =
	"expected a condition after &&";

struct ConditionDisjunction
	: BoolOp<
		  ConditionDisjunction,
		  TAO_PEGTL_RAISE_MESSAGE(
			  "cannot use conjunction (&&) inside disjunction (||)"
		  ),
		  LogicalOr,
		  LogicalAnd> {};
template<>
inline constexpr auto error_message<ConditionDisjunction> =
	"expected a condition after ||";

struct Condition : p::seq<
					   ConditionLeaf,
					   p::opt<
						   Whitespace,
						   p::sor<
							   p::seq<
								   LogicalAnd,
								   p::opt<Whitespace>,
								   p::must<ConditionConjunction>>,
							   p::seq<
								   LogicalOr,
								   p::opt<Whitespace>,
								   p::must<ConditionDisjunction>>>>> {};
template<>
inline constexpr auto error_message<Condition> = "expected a condition";

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
struct IfStatement : p::seq<
						 TAO_PEGTL_STRING("if"),
						 Whitespace,
						 p::must<Condition, Whitespace, BracketedBlock>,
						 p::opt<
							 p::opt<Whitespace>,
							 TAO_PEGTL_STRING("else"),
							 p::opt<Whitespace>,
							 p::must<BracketedBlock>>> {};

struct WhileLoop : p::seq<
					   TAO_PEGTL_STRING("while"),
					   Whitespace,
					   p::must<Condition, Whitespace, BracketedBlock>> {};

struct ForIn : p::string<'i', 'n'> {};
template<>
inline constexpr auto error_message<ForIn> =
	"expected 'in' after for identifier";

struct ForLoop : p::seq<
					 TAO_PEGTL_STRING("for"),
					 Whitespace,
					 p::must<
						 Identifier,
						 Whitespace,
						 ForIn,
						 Whitespace,
						 Leaf,
						 Whitespace,
						 BracketedBlock>> {};

struct Semicolon : p::one<';'> {};
template<>
inline constexpr auto error_message<Semicolon> = "expected ';'";

struct Definition : p::sor<RuleDefinition, ActionDefinition> {};
struct Scope : p::sor<BracketedBlock, IfStatement, WhileLoop, ForLoop> {};
struct RuleStatement
	: p::seq<RuleInvocation, p::opt<Whitespace>, p::must<Semicolon>> {};

struct TargetStatementInvocation : p::sor<Scope, RuleStatement> {};
template<>
inline constexpr auto error_message<TargetStatementInvocation> =
	"expected block, for/while/if, or rule invocation after 'on'";

struct TargetStatement
	: p::seq<
		  TAO_PEGTL_STRING("on"),
		  Whitespace,
		  p::must<Leaf, Whitespace, TargetStatementInvocation>> {};

struct AssignmentOperator
	: p::sor<p::string<'+', '='>, p::string<'?', '='>, p::string<'='>> {};
template<>
inline constexpr auto error_message<AssignmentOperator> =
	"expected assignment with =, +=, or ?=";

struct LocalVariableModifier : TAO_PEGTL_STRING("local") {};
struct TargetVariableModifier
	: p::seq<
		  p::string<'o', 'n'>,
		  Whitespace,
		  // Error only if an assignment operator is next, otherwise it might be
		  // a rule
		  p::if_then_else<
			  p::at<AssignmentOperator>,
			  TAO_PEGTL_RAISE_MESSAGE("expected target(s) after 'on'"),
			  List>> {};

struct VariableAssignment
	: p::seq<
		  p::if_then_else<
			  p::seq<LocalVariableModifier, Whitespace>,
			  // If "local" is present, we can expect a variable assignment.
			  p::seq<
				  p::must<Identifier>,
				  p::opt<Whitespace>,
				  // Check assignment is not both scope and target local.
				  p::if_then_else<
					  p::at<TargetVariableModifier>,
					  TAO_PEGTL_RAISE_MESSAGE("assignment cannot be scope and "
											  "target local simultaneously"),
					  p::success>,
				  p::must<AssignmentOperator>>,
			  // Otherwise it might be a rule, so we only know after the
			  // operator.
			  p::seq<
				  Identifier,
				  p::opt<Whitespace>,
				  p::opt<TargetVariableModifier, p::opt<Whitespace>>,
				  AssignmentOperator>>,
		  // Now it is definitely a variable assignment
		  p::opt<Whitespace>,
		  // Can assign to empty list
		  p::opt<List>,
		  p::opt<Whitespace>,
		  p::must<Semicolon>> {};

struct StatementBlock
	: p::list<
		  p::sor<Definition, Scope, TargetStatement, RuleStatement>,
		  Whitespace> {};

struct HamGrammar : p::seq<StatementBlock, p::eolf> {};
template<>
inline constexpr auto error_message<HamGrammar> =
	"error encountered at end of file";

/**
 * Parse control types
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
		AssignmentOperator,
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
		VariableAssignment,
		VariableReplacer,
		VariableSelector,
		WhileLoop,
		LocalVariableModifier,
		Word>,
	RearrangeBinaryOperator::on<
		BoolExpression,
		ConditionConjunction,
		ConditionDisjunction,
		Condition>,
	RearrangeUnaryOperator::on<ConditionLeaf>>;

struct Error {
	template<typename Rule>
	static constexpr bool raise_on_failure = false;
	template<typename Rule>
	static constexpr auto message = error_message<Rule>;
};

template<typename Rule>
using Control = p::must_if<Error>::control<Rule>;

} // namespace ham::parse

#endif // HAM_PARSE_GRAMMAR_HPP
