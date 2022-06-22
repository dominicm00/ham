/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_MAKE_OPTIONS_H
#define HAM_MAKE_OPTIONS_H

#include "data/String.h"

namespace ham
{
namespace make
{

class Options
{
  public:
	Options();

	String RulesetFile() const { return fRulesetFile; }
	void SetRulesetFile(const String& fileName) { fRulesetFile = fileName; }

	String ActionsOutputFile() const { return fActionsOutputFile; }
	void SetActionsOutputFile(const String& fileName)
	{
		fActionsOutputFile = fileName;
	}

	bool IsDryRun() const { return fDryRun; }
	void SetDryRun(bool dryRun) { fDryRun = dryRun; }

	bool IsPrintMakeTree() const { return fPrintMakeTree; }
	void SetPrintMakeTree(bool print) { fPrintMakeTree = print; }

	bool IsPrintActions() const { return fPrintActions; }
	void SetPrintActions(bool print) { fPrintActions = print; }

	bool IsPrintCommands() const { return fPrintCommands; }
	void SetPrintCommands(bool print) { fPrintCommands = print; }

	int JobCount() const { return fJobCount; }
	void SetJobCount(int count) { fJobCount = count; }

	bool IsBuildFromNewest() const { return fBuildFromNewest; }
	void SetBuildFromNewest(bool buildFromNewest)
	{
		fBuildFromNewest = buildFromNewest;
	}

	bool IsQuitOnError() const { return fQuitOnError; }
	void SetQuitOnError(bool quitOnError) { fQuitOnError = quitOnError; }

  public:
	String fRulesetFile;
	String fActionsOutputFile;
	bool fDryRun;
	bool fPrintMakeTree;
	bool fPrintActions;
	bool fPrintCommands;
	int fJobCount;
	bool fBuildFromNewest;
	bool fQuitOnError;
};

} // namespace make
} // namespace ham

#endif // HAM_MAKE_OPTIONS_H
