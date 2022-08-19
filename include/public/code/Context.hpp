#ifndef HAM_CODE_CONTEXT_HPP
#define HAM_CODE_CONTEXT_HPP

#include "data/Types.hpp"
#include "data/VariableScope.hpp"

#include <string>
#include <vector>

namespace ham::code
{

/**
 * Global settings that affect the creation/evaluation of the AST
 */
class GlobalContext {
  public:
	data::List targets;
};

/**
 * Context for building the AST.
 */
class AstContext {
  public:
	void EnterLoop()
	{
		loop_depth++;
		block_depth++;
	}

	void EnterBlock() { block_depth++; }

  public:
	unsigned int loop_depth;
	unsigned int block_depth;
	GlobalContext& global_context;
};

/**
 * Context for evaluating the AST
 */
class EvaluationContext {
  public:
	enum JumpCondition {
		NONE,
		BREAK,
		CONTINUE,
		RETURN,
		EXIT
	};

  public:
	EvaluationContext() = delete;
	// TODO: initialize global variables
	EvaluationContext(GlobalContext& global_context);
	EvaluationContext(
		GlobalContext& global_context,
		data::VariableScope&& variable_scope
	)
		: global_context(global_context),
		  variable_scope(std::move(variable_scope)),
		  jump_condition(NONE){};

  public:
	JumpCondition jump_condition;
	data::VariableScope variable_scope;
	GlobalContext& global_context;
};

} // namespace ham::code

#endif // HAM_CODE_CONTEXT_HPP
