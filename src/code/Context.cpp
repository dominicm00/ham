#include "code/Context.hpp"

namespace ham::code
{

EvaluationContext::EvaluationContext(GlobalContext& global_context)
	: global_context(global_context),
	  variable_scope(),
	  jump_condition(NONE){
		  // TODO: Initialize global variables
	  };

}
