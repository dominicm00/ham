/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_PROCESSOR_HPP
#define HAM_CODE_PROCESSOR_HPP

#include "code/EvaluationContext.hpp"
#include "data/StringList.hpp"
#include "data/TargetPool.hpp"
#include "data/VariableDomain.hpp"
#include "make/MakeTarget.hpp"
#include "make/Options.hpp"

namespace ham::make
{

using data::StringList;
using data::Target;
using data::TargetSet;

class Command;
class TargetBuildInfo;

class Processor
{
  public:
	Processor();
	~Processor();

	void SetOptions(const Options& options);

	void SetCompatibility(behavior::Compatibility compatibility);
	// resets behavior as well
	void SetBehavior(behavior::Behavior behavior);

	void SetOutput(std::ostream& output);
	void SetErrorOutput(std::ostream& output);

	void SetPrimaryTargets(const StringList& targets);

	data::VariableDomain& GlobalVariables() { return fGlobalVariables; }
	data::TargetPool& Targets() { return fTargets; }

	void SetForceUpdateTargets(const StringList& targets);

	void ProcessRuleset();
	void PrepareTargets();
	void BuildTargets();

  private:
	typedef std::map<const Target*, MakeTarget*> MakeTargetMap;
	typedef std::map<data::RuleActionsCall*, Command*> CommandMap;
	typedef std::set<TargetBuildInfo*> TargetBuildInfoSet;

  private:
	MakeTarget* _GetMakeTarget(const Target* target, bool create);
	MakeTarget* _GetMakeTarget(const String& targetName, bool create);
	bool _IsPseudoTarget(const MakeTarget* makeTarget) const;

	void _PrepareTargetRecursively(MakeTarget* makeTarget);
	void _SealTargetFateRecursively(
		MakeTarget* makeTarget,
		data::Time parentTime,
		bool makeParent
	);
	void _BindTarget(MakeTarget* makeTarget);
	void _ScanForHeaders(MakeTarget* makeTarget);

	bool _CollectMakableTargets(MakeTarget* makeTarget);
	TargetBuildInfo* _MakeTarget(MakeTarget* makeTarget);
	size_t _TargetMade(MakeTarget* makeTarget, MakeTarget::MakeState state);
	Command* _BuildCommand(data::RuleActionsCall* actionsCall);

	void _PrintMakeTreeBinding(const MakeTarget* makeTarget);
	void
	_PrintMakeTreeState(const MakeTarget* makeTarget, data::Time parentTime);
	void _PrintMakeTreeStep(
		const MakeTarget* makeTarget,
		const char* step,
		const char* state,
		const char* pattern,
		...
	);

  private:
	data::VariableDomain fGlobalVariables;
	data::TargetPool fTargets;
	code::EvaluationContext fEvaluationContext;
	Options fOptions;
	StringList fPrimaryTargetNames;
	MakeTargetSet fPrimaryTargets;
	MakeTargetMap fMakeTargets;
	data::Time fNow;
	int fMakeLevel;
	MakeTargetSet fMakableTargets;
	CommandMap fCommands;
	TargetBuildInfoSet fTargetBuildInfos;
	size_t fTargetsToUpdateCount;
};

} // namespace ham::make

#endif // HAM_CODE_PROCESSOR_HPP
