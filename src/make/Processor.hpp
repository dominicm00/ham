/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_PROCESSOR_HPP
#define HAM_CODE_PROCESSOR_HPP

#include "code/EvaluationContext.hpp"
#include "data/RuleActions.hpp"
#include "data/StringList.hpp"
#include "data/TargetContainers.hpp"
#include "data/TargetPool.hpp"
#include "data/VariableDomain.hpp"
#include "make/MakeTarget.hpp"
#include "make/Options.hpp"

#include <map>
#include <memory>
#include <vector>

namespace ham::make
{

using data::StringList;
using data::Target;
using data::TargetSet;

class Command;
class TargetBuildInfo;

using MakeTargetMap = std::map<Target*, MakeTarget*>;
using CommandList = std::vector<Command*>;
using CommandMap = std::map<Target*, CommandList>;
using TargetBuildInfoSet = std::set<TargetBuildInfo*>;

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

	/**
	 * Set targets to build. Should be "all" if user did not specify a specific
	 * target.
	 *
	 * \param[in] targets
	 */
	void SetPrimaryTargets(const StringList& targets);

	data::VariableDomain& GlobalVariables() { return fGlobalVariables; }
	data::TargetPool& Targets() { return fTargets; }

	/**
	 * Set targets that should be updated regardless of their status.
	 *
	 * \param[in] targets List of target strings to force update
	 */
	void SetForceUpdateTargets(const StringList& targets);

	/**
	 * Processes the base ruleset, and by extension, project build files (the
	 * ruleset should include the Jamfile in the current directory). If a
	 * ruleset file was specified in make::Options, then it is used. Otherwise,
	 * a default built-in file is selected based on the compatibility mode.
	 *
	 * This method must be called before PrepareTargets and BuildTargets.
	 */
	void ProcessRuleset();

	/**
	 * Prepare targets for building by:
	 * - Binding targets to a filesystem path
	 * - Recursively determining a targets dependencies
	 * - Determine the fate of all targets and their dependencies
	 *
	 * This method must be called before BuildTargets.
	 */
	void PrepareTargets();

	/**
	 * Collects targets set to build by PrepareTargets and builds them in the
	 * shell specified by $JAMSHELL.
	 */
	void BuildTargets();

  private:
	MakeTarget* _GetMakeTarget(Target* target, bool create);
	MakeTarget* _GetMakeTarget(const String& targetName, bool create);
	bool _IsPseudoTarget(const MakeTarget* makeTarget) const;

	/**
	 * Binds and sets a tentative fate for a target and all of its dependencies.
	 */
	void _PrepareTargetRecursively(MakeTarget* makeTarget);

	/**
	 * Updates MakeTarget::MAKE_IF_NEEDED targets if one of their dependents is
	 * going to be made.
	 *
	 * \param[in] makeTarget
	 * \param[in] parentTime
	 * \param[in] makeParent
	 */
	void _SealTargetFateRecursively(
		MakeTarget* makeTarget,
		data::Time parentTime,
		bool makeParent
	);

	/**
	 * Bind a target to a filesystem path, or do nothing if it is already bound.
	 *
	 * \param[in] makeTarget
	 */
	void _BindTarget(MakeTarget* makeTarget);

	/**
	 * Scans target with the kHeaderScanVariableName egrep pattern. If matches
	 * are found, apply the rule kHeaderRuleVariableName to the target with the
	 * matches of the egrep pattern as sources. This is generally used to add
	 * dependencies to header files.
	 *
	 * \param[in] makeTarget
	 */
	void _ScanForHeaders(MakeTarget* makeTarget);

	/**
	 * Sets the MakeTarget::MakeState of a target and all its transitive
	 * dependencies.
	 *
	 * \param[in] makeTarget
	 *
	 * \return true if any targets need to be made, false otherwise
	 */
	bool _CollectMakableTargets(MakeTarget* makeTarget);

	/**
	 * Make commands for a certain target.
	 *
	 * \param[in] target
	 */
	CommandList _MakeCommands(Target* target);

	/**
	 * Returns the build info for a target, or nullptr if there are no pending
	 * actions.
	 *
	 * \param[in] makeTarget
	 */
	TargetBuildInfo* _MakeTarget(MakeTarget* makeTarget);

	/**
	 * Indicate a target has completed building with some MakeState, and
	 * propagate the change to dependents.
	 *
	 * \param[in] makeTarget target that has completed
	 * \param[in] state completed state, cannot be MakeTarget::Pending
	 *
	 * \return the number of targets skipped as a result of the completion.
	 */
	size_t _TargetMade(MakeTarget* makeTarget, MakeTarget::MakeState state);

	/**
	 * Create a list of bound paths from an actions targets. Depending on action
	 * modifiers, may exclude certain targets.
	 *
	 * \param[in] actionsCall actions call to bind
	 * \param[in] isSources whether or not to bind the sources of the action
	 * call \param[out] boundTargets list to append bound targets to
	 */
	void _BindActionsTargets(
		data::RuleActionsCall* actionsCall,
		bool isSources,
		StringList& boundTargets
	);

	/**
	 * Create a runnable Command from an actions call.
	 *
	 * \param[in] actionsCall
	 * \param[out] commands CommandList to add actions to
	 */
	void
	_BuildCommands(data::RuleActionsCall* actionsCall, CommandList& commands);

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
	void _PrintWarning(std::string warning);

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
