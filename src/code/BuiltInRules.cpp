/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/BuiltInRules.h"

#include "code/EvaluationContext.h"
#include "code/Rule.h"
#include "code/RuleInstructions.h"


namespace ham {
namespace code {


struct EchoInstructions : RuleInstructions {
public:
	virtual StringList Evaluate(EvaluationContext& context,
		const StringListList& parameters)
	{
		std::ostream& output = context.Output();

		if (!parameters.empty()) {
			const StringList& list = parameters[0];
			size_t count = list.Size();
			for (size_t i = 0; i < count; i++) {
				if (i > 0)
					output << ' ';
				output << list.ElementAt(i);
			}

			if (context.GetBehavior().GetEchoTrailingSpace()
					== behavior::Behavior::ECHO_TRAILING_SPACE) {
				output << ' ';
			}
		}

		output << std::endl;
		return StringList::False();
	}
};


void
BuiltInRules::RegisterRules(RulePool& rulePool)
{
	_AddRule(rulePool, "echo", new EchoInstructions);
	_AddRule(rulePool, "Echo", new EchoInstructions);
	_AddRule(rulePool, "ECHO", new EchoInstructions);
}


void
BuiltInRules::_AddRule(RulePool& rulePool, const char* name,
	RuleInstructions* instructions)
{
	Rule& rule = rulePool.LookupOrCreate(name);
	rule.SetInstructions(instructions);
}


}	// namespace code
}	// namespace ham
