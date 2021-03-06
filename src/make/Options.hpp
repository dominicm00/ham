/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Copyright 2022, Dominic Martinez, dom@dominicm.dev.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_MAKE_OPTIONS_HPP
#define HAM_MAKE_OPTIONS_HPP

#include "data/String.hpp"

namespace ham::make
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

	bool IsPrintQuietActions() const { return fPrintQuietActions; }
	void SetPrintQuietActions(bool print) { fPrintQuietActions = print; }

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
	bool fPrintQuietActions;
	bool fPrintCommands;
	int fJobCount;
	bool fBuildFromNewest;
	bool fQuitOnError;
};

} // namespace ham::make

#endif // HAM_MAKE_OPTIONS_HPP
