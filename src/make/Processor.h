/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_PROCESSOR_H
#define HAM_CODE_PROCESSOR_H


#include "code/EvaluationContext.h"
#include "data/MakeTarget.h"
#include "data/StringList.h"
#include "data/TargetPool.h"
#include "data/VariableDomain.h"


namespace ham {
namespace make {


using data::MakeTarget;
using data::MakeTargetSet;
using data::StringList;
using data::Target;
using data::TargetSet;


class Processor {
public:
								Processor();
								~Processor();

			void				SetCompatibility(
									behavior::Compatibility compatibility);
									// resets behavior as well
			void				SetBehavior(behavior::Behavior behavior);

			void				SetJambaseFile(const char* fileName);
			void				SetActionsOutputFile(const char* fileName);
			void				SetJobCount(int count);
			void				SetBuildFromNewest(bool buildFromNewest);
			void				SetDryRun(bool dryRun);
			void				SetQuitOnError(bool quitOnError);
			void				SetPrintMakeTree(bool printMakeTree);
			void				SetPrintActions(bool printActions);
			void				SetPrintCommands(bool printCommands);

			void				SetOutput(std::ostream& output);
			void				SetErrorOutput(std::ostream& output);

			void				SetPrimaryTargets(const StringList& targets);

			data::VariableDomain& GlobalVariables()
									{ return fGlobalVariables; }
			data::TargetPool&	Targets()
									{ return fTargets; }

			void				SetForceUpdateTargets(
									const StringList& targets);

			void				ProcessJambase();
			void				PrepareTargets();
			void				BuildTargets();

private:
			typedef std::map<Target*, MakeTarget*> MakeTargetMap;

private:
			MakeTarget*			_GetMakeTarget(Target* target, bool create);

			void				_PrepareTargetRecursively(
									MakeTarget* makeTarget,
									data::Time parentTime);
			void				_ScanForHeaders(MakeTarget* makeTarget);

			bool				_CollectMakableTargets(MakeTarget* makeTarget);
			void				_MakeTarget(MakeTarget* makeTarget);
			void				_TargetMade(MakeTarget* makeTarget,
									MakeTarget::MakeState state);

			void				_PrintMakeTreeBinding(
									const MakeTarget* makeTarget);
			void				_PrintMakeTreeState(
									const MakeTarget* makeTarget,
									data::Time parentTime);
			void				_PrintMakeTreeStep(const MakeTarget* makeTarget,
									const char* step, const char* state,
									const char* pattern, ...);

private:
			data::VariableDomain fGlobalVariables;
			data::TargetPool	fTargets;
			code::EvaluationContext fEvaluationContext;
			String				fJambaseFile;
			String				fActionsOutputFile;
			int					fJobCount;
			bool				fBuildFromNewest;
			bool				fDryRun;
			bool				fQuitOnError;
			bool				fPrintMakeTree;
			bool				fPrintActions;
			bool				fPrintCommands;
			StringList			fPrimaryTargetNames;
			MakeTargetSet		fPrimaryTargets;
			MakeTargetMap		fMakeTargets;
			data::Time			fNow;
			int					fMakeLevel;
			MakeTargetSet		fMakableTargets;
};


}	// namespace make
}	// namespace ham


#endif	// HAM_CODE_PROCESSOR_H
