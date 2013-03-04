/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_MAKE_DEBUG_OPTIONS_H
#define HAM_MAKE_DEBUG_OPTIONS_H


namespace ham {
namespace make {


class DebugOptions {
public:
								DebugOptions();

			bool				IsDryRun() const
									{ return fDryRun; }
			void				SetDryRun(bool dryRun)
									{ fDryRun = dryRun; }

			bool				IsPrintMakeTree() const
									{ return fPrintMakeTree; }
			void				SetPrintMakeTree(bool print)
									{ fPrintMakeTree = print; }

			bool				IsPrintActions() const
									{ return fPrintActions; }
			void				SetPrintActions(bool print)
									{ fPrintActions = print; }

			bool				IsPrintCommands() const
									{ return fPrintCommands; }
			void				SetPrintCommands(bool print)
									{ fPrintCommands = print; }

public:
			bool				fDryRun;
			bool				fPrintMakeTree;
			bool				fPrintActions;
			bool				fPrintCommands;
};


} // namespace make
} // namespace ham


#endif	// HAM_MAKE_DEBUG_OPTIONS_H
