#ifndef HAM_CODE_CONTEXT_HPP
#define HAM_CODE_CONTEXT_HPP

#include "data/Types.hpp"
#include "data/VariableScope.hpp"

#include <ostream>
#include <string>
#include <vector>

namespace ham::code
{

/**
 * Global settings that affect the creation/evaluation of the AST
 */
class GlobalContext {
  public:
	GlobalContext() = delete;
	GlobalContext(data::List targets, std::ostream& warning_stream)
		: targets(std::move(targets)),
		  warning_stream(warning_stream),
		  testing(false){};
	GlobalContext(
		data::List targets,
		std::ostream& warning_stream,
		bool testing
	)
		: targets(std::move(targets)),
		  warning_stream(warning_stream),
		  testing(testing){};

	// Contexts cannot be copied
	GlobalContext(const GlobalContext&) = delete;
	GlobalContext& operator=(const GlobalContext&) = delete;

	// Contexts can be moved
	GlobalContext(GlobalContext&&) = default;

	// Contexts cannot be overwritten
	GlobalContext& operator=(GlobalContext&) = delete;

  public:
	data::List targets;
	std::ostream& warning_stream;
	bool testing;
};

/**
 * Context for building the AST.
 */
class AstContext {
  public:
	AstContext() = delete;
	AstContext(GlobalContext& global_context)
		: global_context(global_context){};

	// Contexts cannot be copied
	AstContext(const AstContext&) = delete;
	AstContext& operator=(const AstContext&) = delete;

	// Contexts can be moved
	AstContext(AstContext&&) = default;

	// Contexts cannot be overwritten
	AstContext& operator=(AstContext&) = delete;

	operator GlobalContext&() { return global_context; }

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

	// Contexts cannot be copied
	EvaluationContext(const EvaluationContext&) = delete;
	EvaluationContext& operator=(const EvaluationContext&) = delete;

	// Contexts can be moved
	EvaluationContext(EvaluationContext&&) = default;

	// Contexts cannot be overwritten
	EvaluationContext& operator=(EvaluationContext&) = delete;

	operator GlobalContext&() const { return global_context; }

	EvaluationContext CreateSubscopeContext() const
	{
		return EvaluationContext{
			global_context,
			variable_scope.CreateSubscope()};
	};

  public:
	JumpCondition jump_condition;
	data::VariableScope variable_scope;
	GlobalContext& global_context;
};

} // namespace ham::code

#endif // HAM_CODE_CONTEXT_HPP
