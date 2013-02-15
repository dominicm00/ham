/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/BuiltInRules.h"

#include "code/EvaluationContext.h"
#include "code/Rule.h"
#include "code/RuleInstructions.h"
#include "data/RegExp.h"


namespace ham {
namespace code {


static void
echo_string_list_list(EvaluationContext& context,
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
}


struct EchoInstructions : RuleInstructions {
public:
	virtual StringList Evaluate(EvaluationContext& context,
		const StringListList& parameters)
	{
		echo_string_list_list(context, parameters);
		return StringList::False();
	}
};


struct ExitInstructions : RuleInstructions {
public:
	virtual StringList Evaluate(EvaluationContext& context,
		const StringListList& parameters)
	{
		echo_string_list_list(context, parameters);
		context.SetJumpCondition(JUMP_CONDITION_EXIT);
		return StringList::False();
	}
};


struct MatchInstructions : RuleInstructions {
public:
	virtual StringList Evaluate(EvaluationContext& context,
		const StringListList& parameters)
	{
		using data::RegExp;

		if (parameters.size() < 2)
			return StringList::False();

		StringList expressions = parameters[0];
		size_t expressionCount = expressions.Size();
		StringList strings = parameters[1];
		size_t stringCount = strings.Size();

		StringList result;
		for (size_t i = 0; i < expressionCount; i++) {
			RegExp regExp(expressions.ElementAt(i).ToCString());
			if (!regExp.IsValid()) {
// TODO: Throw exception!
				continue;
			}

			for (size_t k = 0; k < stringCount; k++) {
				String string = strings.ElementAt(k);
				RegExp::MatchResult match = regExp.Match(string.ToCString());
				if (!match.HasMatched())
					continue;

				size_t matchCount = match.GroupCount();
				for (size_t l = 0; l < matchCount; l++) {
					result.Append(
						string.SubString(match.GroupStartOffsetAt(l),
							match.GroupEndOffsetAt(l)));
				}
			}
		}

		return result;
	}
};


void
BuiltInRules::RegisterRules(RulePool& rulePool)
{
	_AddRule(rulePool, "echo", new EchoInstructions);
	_AddRule(rulePool, "Echo", new EchoInstructions);
	_AddRule(rulePool, "ECHO", new EchoInstructions);
	_AddRule(rulePool, "exit", new ExitInstructions);
	_AddRule(rulePool, "Exit", new ExitInstructions);
	_AddRule(rulePool, "EXIT", new ExitInstructions);
	_AddRule(rulePool, "match", new MatchInstructions);
	_AddRule(rulePool, "Match", new MatchInstructions);
	_AddRule(rulePool, "MATCH", new MatchInstructions);
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
