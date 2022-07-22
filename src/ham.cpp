/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Copyright 2022, Dominic Martinez, dom@dominicm.dev.
 * Distributed under the terms of the MIT License.
 */

#include "make/MakeException.hpp"
#include "make/Options.hpp"
#include "make/Processor.hpp"
#include "util/OptionIterator.hpp"
#include "util/TextFileException.hpp"

#include <iostream>
#include <map>
#include <ostream>
#include <string.h>
#include <unistd.h>

using namespace ham;

static void
print_usage(const char* programName, bool error)
{
	std::ostream out(error ? std::cerr.rdbuf() : std::cout.rdbuf());
	out << "Usage: " << programName
		<< " [ <options> ] [ <target> ... ]\n"
		   "Options:\n"
		   "  -a, --all\n"
		   "      Build all targets, even the ones that are up-to-date.\n"
		   "  -c <version>, --compatibility <version>\n"
		   "      Behave compatible to <version>, which is one of:\n"
		   "      - \"ham\" (Ham, the default)\n"
		   "      - \"jam\" (Perforce Jam 2.5)\n"
		   "      - \"boost\" (Boost.Build)\n"
		   "  -d <option>, --debug <option>\n"
		   "      Enable/set a debug option. Options are:\n"
		   "      a     -  Print the actions\n"
		   "      c     -  Print the causes\n"
		   "      d     -  Print the dependencies\n"
		   "      m     -  Print the make tree\n"
		   "      x     -  Print the make commands\n"
		   "      0..9  -  Set debug level.\n"
		   "  -f <file>, --ruleset <file>\n"
		   "      Execute <file> instead of the built-in ruleset.\n"
		   "  -g, --from-newest\n"
		   "      Build from the newest sources first.\n"
		   "  -h, --help\n"
		   "      Print this usage message.\n"
		   "  -j <jobs>, --jobs <jobs>\n"
		   "      Use up to <jobs> number of concurrent shell processes.\n"
		   "  -k, --keep-going\n"
		   "      Keep going when target fails. "
		   "Default in -cjam and -cboost mode.\n"
		   "  -n, --dry-run\n"
		   "      Print actions and commands, but don't run them.\n"
		   "  -o <file>, --output-actions <file>\n"
		   "      Write the actions to <file>.\n"
		   "  -q, --quit-on-error\n"
		   "      Quit immediately when a target fails. Default in -cham "
		   "mode.\n"
		   "  -s <variable>=<value>, --set <variable>=<value>\n"
		   "      Set variable <variable> to <value>, overriding the "
		   "environmental variable.\n"
		   "  -t <target>, --target <target>\n"
		   "      Rebuild target <target>, even if it is up-to-date.\n"
		   "  -v, --version\n"
		   "      Print the Ham version and exit.\n"
		<< std::endl;
}

[[noreturn]] static void
print_usage_end_exit(const char* programName, bool error)
{
	print_usage(programName, error);
	exit(error ? 1 : 0);
}

[[noreturn]] static void
unimplemented_cli(const char* unimplementedOption, const char* issueLink)
{
	std::cerr << "Error: " << unimplementedOption
			  << " is not implemented in this version of Ham.\n"
				 "Check the implementation status at:\n"
			  << issueLink << std::endl;
	exit(1);
}

static bool
set_external_variable(
	std::map<data::String, data::StringList>& variables,
	const char* variable
)
{
	const char* equalSign = strchr(variable, '=');
	if (equalSign == nullptr)
		return false;

	variables[data::String(variable, equalSign - variable)] =
		data::StringList{data::String{equalSign + 1}};
	return true;
}

int
main(int argc, const char* const* argv)
{
	const char* programName = argc >= 1 ? argv[0] : "ham";

	// get standard variables
	std::map<data::String, data::StringList> variables;
	// TODO: Properly detect platform!
	variables["UNIX"] = {"true"};
	variables["OS"] = {"LINUX"};
	variables["OSPLAT"] = {"X86"};
	variables["JAMVERSION"] = {"2.5-haiku-20220722"};

	// import environment
	// TODO: Platform specific!
	// TODO: Split up PATH-type variables
	for (size_t i = 0; environ[i] != nullptr; i++) {
		set_external_variable(variables, environ[i]);
	}

	// parse arguments
	std::string rulesetFile;
	bool rulesetFileSpecified = false;
	std::string actionsOutputFile;
	bool actionsOutputFileSpecified = false;
	behavior::Compatibility compatibility = behavior::COMPATIBILITY_HAM;
	bool compatibilitySpecified = false;
	bool buildFromNewest = false;
	int jobCount = 1;
	bool dryRun = false;
	bool quitOnError = false;
	bool printMakeTree = false;
	bool printActions = true;
	bool printQuietActions = false;
	bool printCommands = false;
	bool debugSpecified = false;
	data::StringList forceUpdateTargets;

	util::OptionIterator optionIterator(
		argc,
		argv,
		util::OptionSpecification()
			.Add('a', "--all")
			.Add('c', "--compatibility", true)
			.Add('d', "--debug", true)
			.Add('f', "--ruleset", true)
			.Add('g', "--from-newest")
			.Add('h', "--help")
			.Add('j', "--jobs", true)
			.Add('k', "--keep-going")
			.Add('n', "--dry-run")
			.Add('o', "--output-actions", true)
			.Add('q', "--quit-on-error")
			.Add('s', "--set", true)
			.Add('t', "--target", true)
			.Add('v', "--version")
	);

	while (optionIterator.HasNext()) {
		// short ("-") option(s)
		std::string argument;
		switch (optionIterator.Next(argument)) {
			case 'a':
				unimplemented_cli(
					"Building all targets",
					"https://github.com/dominicm00/ham/issues/35"
				);
			case 'c': {
				if (argument == "jam") {
					compatibility = behavior::COMPATIBILITY_JAM;
				} else if (argument == "boost") {
					// TODO: This is disabled because Boost.Build was
					// revamped since this compatibility was written, and
					// current compatibility status is unknown.
					//
					// compatibility = behavior::COMPATIBILITY_BOOST_JAM;
					unimplemented_cli(
						"Boost.Build compatibility",
						"https://github.com/dominicm00/ham/issues/32"
					);
				} else if (argument == "ham") {
					compatibility = behavior::COMPATIBILITY_HAM;
				} else {
					std::cerr << "Error: Invalid argument for "
								 "compatibility option: "
							  << argument << std::endl;
					exit(1);
				}
				compatibilitySpecified = true;
				break;
			}

			case 'd': {
				if (argument.empty())
					print_usage_end_exit(programName, true);

				// The first '-d' encountered clears all default debug options.
				if (!debugSpecified) {
					printActions = false;
					debugSpecified = true;
				}

				for (size_t i = 0; i < argument.length(); i++) {
					char debugOption = argument[i];
					switch (debugOption) {
						case 'a':
							printActions = true;
							printQuietActions = true;
							break;
						case 'c':
							unimplemented_cli(
								"Displaying causes",
								"https://github.com/dominicm00/ham/issues/33"
							);
						case 'd':
							unimplemented_cli(
								"Displaying dependency tree",
								"https://github.com/dominicm00/ham/issues/34"
							);
						case 'm':
							printMakeTree = true;
							break;
						case 'x':
							printCommands = true;
							break;
						default:
							if (!isdigit(debugOption))
								print_usage_end_exit(programName, true);

							bool allOptionsSupported = true;
							switch (debugOption - '0') {
									// TODO: Support all debug levels correctly!
								case 9:
									allOptionsSupported = false;
									[[fallthrough]];
								case 8:
									allOptionsSupported = false;
									[[fallthrough]];
								case 7:
									allOptionsSupported = false;
									[[fallthrough]];
								case 6:
									allOptionsSupported = false;
									[[fallthrough]];
								case 5:
									allOptionsSupported = false;
									[[fallthrough]];
								case 4:
									printCommands = true;
									[[fallthrough]];
								case 3:
									printMakeTree = true;
									[[fallthrough]];
								case 2:
								case 1:
									printActions = true;
									[[fallthrough]];
								case 0:
									break;
							}

							if (!allOptionsSupported) {
								std::cerr << "Warning: not all selected debug "
											 "options are implemented in this "
											 "version of Ham. See "
											 "https://github.com/dominicm00/"
											 "ham/issues/31 for more details."
										  << std::endl;
							}
					}
				}
				break;
			}

			case 'f':
				rulesetFile = argument;
				rulesetFileSpecified = true;
				break;

			case 'g':
				buildFromNewest = true;
				break;

			case 'h':
				print_usage_end_exit(programName, false);

			case 'j': {
				char* end;
				jobCount = strtol(argument.c_str(), &end, 0);
				if (*end != '\0')
					print_usage_end_exit(programName, true);
				break;
			}

			case 'k':
				quitOnError = false;
				break;

			case 'n':
				dryRun = true;
				break;

			case 'o':
				actionsOutputFile = argument;
				actionsOutputFileSpecified = true;
				break;

			case 'q':
				quitOnError = true;
				break;

			case 's': {
				if (!set_external_variable(variables, argument.c_str()))
					print_usage_end_exit(programName, true);
				break;
			}

			case 't':
				forceUpdateTargets.Append(String(argument.c_str()));
				break;

			case 'v':
				std::cout << "Ham 0.1\n"
							 "Copyright 2010-2013 Ingo Weinhold.\n"
							 "Copyright 2022      Dominic Martinez."
						  << std::endl;
				exit(0);

			default:
				print_usage_end_exit(programName, true);
		}
	}

	if (optionIterator.ErrorOccurred())
		print_usage_end_exit(programName, true);

	// get targets to be made
	StringList primaryTargets;
	for (int i = optionIterator.Index(); i < argc; i++)
		primaryTargets.Append(String(argv[i]));

	// "all" is the default target, if none is given.
	if (primaryTargets.IsEmpty())
		primaryTargets.Append("all");

	// Set JAM_TARGETS
	variables["JAM_TARGETS"] = primaryTargets;

	// dry-run implies printing commands, unless debug options have been
	// specified explicitly.
	if (dryRun && !debugSpecified)
		printCommands = true;

	make::Processor processor{};

	// set explicitly specified variables
	for (std::map<data::String, data::StringList>::iterator it =
			 variables.begin();
		 it != variables.end();
		 ++it) {
		processor.GlobalVariables().Set(it->first, it->second);
	}

	// Set compatibility. If not specified explicitly, infer from the program
	// name.
	if (!compatibilitySpecified) {
		const char* slash = strrchr(programName, '/');
		const char* baseName = slash != nullptr ? slash + 1 : programName;
		if (strcmp(baseName, "jam") == 0)
			compatibility = behavior::COMPATIBILITY_JAM;
		else if (strcmp(baseName, "bjam") == 0)
			// TODO: Boost jam is no longer called bjam.
			compatibility = behavior::COMPATIBILITY_BOOST_JAM;
	}

	processor.SetCompatibility(compatibility);

	// set other options
	make::Options options;
	if (rulesetFileSpecified)
		options.SetRulesetFile(rulesetFile.c_str());
	options.SetBuildFromNewest(buildFromNewest);
	options.SetJobCount(jobCount);
	options.SetDryRun(dryRun);
	options.SetPrintMakeTree(printMakeTree);
	options.SetPrintActions(printActions);
	options.SetPrintQuietActions(printQuietActions);
	options.SetPrintCommands(printCommands);
	if (actionsOutputFileSpecified)
		options.SetActionsOutputFile(actionsOutputFile.c_str());
	options.SetQuitOnError(quitOnError);
	processor.SetOptions(options);

	processor.SetPrimaryTargets(primaryTargets);

	try {
		// execute the jam code
		processor.ProcessRuleset();

		// Set the targets that shall be made, even if up-to-date.
		processor.SetForceUpdateTargets(forceUpdateTargets);

		// prepare the targets
		processor.PrepareTargets();

		// build the targets
		processor.BuildTargets();
	} catch (make::MakeException& exception) {
		std::cerr << exception.Message() << std::endl;
	} catch (util::TextFileException& exception) {
		const util::TextFilePosition& position = exception.Position();
		std::cerr << position.FileName() << ":" << position.Line() + 1 << ":"
				  << position.Column() + 1 << ":" << exception.Message() << "."
				  << std::endl;
	}

	// TODO: Catch exceptions...

	return 0;
}
