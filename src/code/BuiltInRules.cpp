/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Copyright 2022, Dominic Martinez, dom@dominicm.dev.
 * Distributed under the terms of the MIT License.
 */

#include "code/BuiltInRules.hpp"

#include "code/EvaluationContext.hpp"
#include "code/Rule.hpp"
#include "code/RuleInstructions.hpp"
#include "data/RegExp.hpp"
#include "data/StringBuffer.hpp"
#include "data/TargetPool.hpp"

#include <dirent.h>

namespace ham::code
{

static void
echo_string_list_list(
	EvaluationContext& context,
	const StringListList& parameters
)
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

class EchoInstructions : public RuleInstructions
{
  public:
	StringList Evaluate(
		EvaluationContext& context,
		const StringListList& parameters
	) override
	{
		echo_string_list_list(context, parameters);
		return StringList::False();
	}
};

class ExitInstructions : public RuleInstructions
{
  public:
	StringList Evaluate(
		EvaluationContext& context,
		const StringListList& parameters
	) override
	{
		echo_string_list_list(context, parameters);
		context.SetJumpCondition(JUMP_CONDITION_EXIT);
		return StringList::False();
	}
};

class MatchInstructions : public RuleInstructions
{
  public:
	StringList Evaluate(EvaluationContext&, const StringListList& parameters)
		override
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

			for (size_t k = 0; k < stringCount; k++) {
				String string = strings.ElementAt(k);
				RegExp::MatchResult match = regExp.Match(string.ToCString());
				if (!match.HasMatched())
					continue;

				size_t matchCount = match.GroupCount();
				for (size_t l = 0; l < matchCount; l++) {
					result.Append(string.SubString(
						match.GroupStartOffsetAt(l),
						match.GroupEndOffsetAt(l)
					));
				}
			}
		}

		return result;
	}
};

class GlobInstructions : public RuleInstructions
{
  public:
	StringList Evaluate(EvaluationContext&, const StringListList& parameters)
		override
	{
		using data::RegExp;

		if (parameters.size() < 2)
			return StringList::False();

		StringList directories = parameters[0];
		size_t directoryCount = directories.Size();
		StringList patterns = parameters[1];
		size_t patternCount = patterns.Size();

		// prepare the regexp up front
		std::vector<RegExp> regExps;

		for (size_t k = 0; k < patternCount; k++) {
			RegExp regExp(
				patterns.ElementAt(k).ToCString(),
				RegExp::PATTERN_TYPE_WILDCARD
			);
			regExps.push_back(regExp);
		}
		size_t regExpCount = regExps.size();

		// iterate through all directories
		StringList result;
		for (size_t i = 0; i < directoryCount; i++) {
			// open the directory and iterate through all directory entries
			String directory = directories.ElementAt(i);
			if (directory.IsEmpty())
				continue;

			DIR* dir = opendir(directory.ToCString());
			if (dir == nullptr)
				continue;

			try {
				while (struct dirent* dirEntry = readdir(dir)) {
					const char* entryName = dirEntry->d_name;
					size_t entryNameLength = strlen(entryName);

					// check, if any of the patterns matches
					bool matches = false;
					for (size_t k = 0; k < regExpCount; k++) {
						RegExp::MatchResult match = regExps[k].Match(entryName);
						if (!match.HasMatched() || match.StartOffset() != 0
							|| match.EndOffset() != entryNameLength) {
							continue;
						}

						matches = true;
						break;
					}

					// Append the entry's path to the result list, if it matches
					// any pattern.
					if (matches) {
						data::StringBuffer path;
						path += directory;
						// TODO: path delimiter!
						path += '/';
						path += entryName;
						result.Append(path);
					}
				}
			} catch (...) {
				closedir(dir);
				throw;
			}

			closedir(dir);
		}

		return result;
	}
};

template<bool kIncludes>
class DependsInstructions : public RuleInstructions
{
  public:
	StringList Evaluate(
		EvaluationContext& context,
		const StringListList& parameters
	) override
	{
		if (parameters.size() < 2)
			return StringList::False();

		// resolve the dependencies
		data::TargetSet dependencies;
		const StringList& dependencyNames = parameters[1];
		size_t dependencyCount = dependencyNames.Size();
		for (size_t i = 0; i < dependencyCount; i++) {
			data::Target* target =
				context.Targets().LookupOrCreate(dependencyNames.ElementAt(i));
			dependencies.Append(target);
		}

		// add the dependencies to the targets
		const StringList& targetNames = parameters[0];
		size_t targetCount = targetNames.Size();
		for (size_t i = 0; i < targetCount; i++) {
			data::Target* target =
				context.Targets().LookupOrCreate(targetNames.ElementAt(i));
			if (kIncludes)
				target->AddIncludes(dependencies);
			else
				target->AddDependencies(dependencies);
		}

		return StringList::False();
	}
};

template<uint32_t kFlags>
class AddTargetFlagsRule : public RuleInstructions
{
  public:
	StringList Evaluate(
		EvaluationContext& context,
		const StringListList& parameters
	) override
	{
		if (!parameters.empty()) {
			const StringList& targetNames = parameters[0];
			size_t targetCount = targetNames.Size();
			for (size_t i = 0; i < targetCount; i++) {
				data::Target* target =
					context.Targets().LookupOrCreate(targetNames.ElementAt(i));
				target->AddFlags(kFlags);
			}
		}
		return StringList::False();
	}
};

/*static*/ void
BuiltInRules::RegisterRules(RulePool& rulePool)
{
	_AddRuleConsumeReference(
		rulePool,
		"echo",
		new EchoInstructions,
		"Echo",
		"ECHO"
	);
	_AddRuleConsumeReference(
		rulePool,
		"exit",
		new ExitInstructions,
		"Exit",
		"EXIT"
	);
	_AddRuleConsumeReference(
		rulePool,
		"glob",
		new GlobInstructions,
		"Glob",
		"GLOB"
	);
	_AddRuleConsumeReference(
		rulePool,
		"match",
		new MatchInstructions,
		"Match",
		"MATCH"
	);
	_AddRuleConsumeReference(
		rulePool,
		"depends",
		new DependsInstructions<false>,
		"Depends",
		"DEPENDS"
	);
	_AddRuleConsumeReference(
		rulePool,
		"includes",
		new DependsInstructions<true>,
		"Includes",
		"INCLUDES"
	);

#define HAM_ADD_TARGET_FLAG_RULE(name, alias1, alias2, flag) \
	_AddRuleConsumeReference(                                \
		rulePool,                                            \
		name,                                                \
		new AddTargetFlagsRule<data::Target::flag>,          \
		alias1,                                              \
		alias2                                               \
	)

	HAM_ADD_TARGET_FLAG_RULE("always", "Always", "ALWAYS", BUILD_ALWAYS);
	HAM_ADD_TARGET_FLAG_RULE("leaves", "Leaves", "LEAVES", DEPENDS_ON_LEAVES);
	HAM_ADD_TARGET_FLAG_RULE("nocare", "NoCare", "NOCARE", IGNORE_IF_MISSING);
	HAM_ADD_TARGET_FLAG_RULE("notfile", "NotFile", "NOTFILE", NOT_A_FILE);
	HAM_ADD_TARGET_FLAG_RULE("noupdate", "NoUpdate", "NOUPDATE", DONT_UPDATE);
	HAM_ADD_TARGET_FLAG_RULE("temporary", "Temporary", "TEMPORARY", TEMPORARY);

#undef HAM_ADD_TARGET_FLAG_RULE
}

/*static*/ void
BuiltInRules::_AddRuleConsumeReference(
	RulePool& rulePool,
	const char* name,
	RuleInstructions* instructions,
	const char* alias1,
	const char* alias2
)
{
	util::Reference<RuleInstructions> instructionsReference(instructions, true);
	_AddRule(rulePool, name, instructions);

	if (alias1 != nullptr)
		_AddRule(rulePool, alias1, instructions);

	if (alias2 != nullptr)
		_AddRule(rulePool, alias2, instructions);
}

/*static*/ void
BuiltInRules::_AddRule(
	RulePool& rulePool,
	const char* name,
	RuleInstructions* instructions
)
{
	Rule& rule = rulePool.LookupOrCreate(name);
	rule.SetInstructions(instructions);
}

} // namespace ham::code
