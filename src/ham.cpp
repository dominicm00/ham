/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include <string.h>
#include <unistd.h>

#include <map>

#include "make/MakeException.h"
#include "make/Processor.h"
#include "util/TextFileException.h"


using namespace ham;


static void
print_usage(const char* programName, bool error)
{
	FILE* out = error ? stderr : stdout;
	fprintf(out, "Usage: %s [ <options> ] [ <target> ... ]\n", programName);
	fprintf(out, "Options:\n");
	fprintf(out, "  -a, --all\n");
	fprintf(out, "      Build all targets, even the ones that are "
		"up-to-date.\n");
	fprintf(out, "  -c <version>, --compatibility <version>\n");
	fprintf(out, "      Behave compatible to <version>, which is either of "
		"\"jam\" (plain Jam\n");
	fprintf(out, "      2.5), \"boost\" (Boost.Jam), or \"ham\" (Ham, the "
		"default).\n");
	fprintf(out, "  -d <option>\n");
	fprintf(out, "      Enable/set a debug option. Options are:\n");
	fprintf(out, "      a     -  Print the actions\n");
	fprintf(out, "      c     -  Print the causes\n");
	fprintf(out, "      d     -  Print the dependencies\n");
	fprintf(out, "      m     -  Print the make tree\n");
	fprintf(out, "      x     -  Print the make commands\n");
	fprintf(out, "      0..9  -  Set debug level.\n");
	fprintf(out, "  -f <file>, --jambase <file>\n");
	fprintf(out, "      Execute <file> instead of the built-in Jambase.\n");
	fprintf(out, "  -g, --from-newest\n");
	fprintf(out, "      Build from the newest sources first.\n");
	fprintf(out, "  -h, --help\n");
	fprintf(out, "      Print this usage message.\n");
	fprintf(out, "  -j <jobs>, --jobs <jobs>\n");
	fprintf(out, "      Use up to <jobs> number of concurrent shell "
		"processes.\n");
	fprintf(out, "  -n, --dry-run\n");
	fprintf(out, "      Print actions and commands, but don't run them.\n");
	fprintf(out, "  -o <file>, --actions <file>\n");
	fprintf(out, "      Write the actions to file <file>.\n");
	fprintf(out, "  -q, --quit-on-error\n");
	fprintf(out, "      Quit as soon as making a target fails.\n");
	fprintf(out, "  -s <variable>=<value>, --set <variable>=<value>\n");
	fprintf(out, "      Set variable <variable> to <value>, overriding the "
		"environmental variable.\n");
	fprintf(out, "  -t <target>, --target <target>\n");
	fprintf(out, "      Rebuild target <target>, even if it is up-to-date.\n");
	fprintf(out, "  -v, --version\n");
	fprintf(out, "      Print the Ham version and exit.\n");
}


static void
print_usage_end_exit(const char* programName, bool error)
{
	print_usage(programName, error);
	exit(error ? 1 : 0);
}


static bool
set_variable(std::map<data::String, data::String>& variables,
	const char* variable)
{
	const char* equalSign = strchr(variable, '=');
	if (equalSign == NULL)
		return false;

	variables[data::String(variable, equalSign - variable)]
		= data::String(equalSign + 1);
	return true;
}


int
main(int argc, const char* const* argv)
{
	const char* programName = argc >= 1 ? argv[0] : "ham";

	// get standard variables
	std::map<data::String, data::String> variables;
	set_variable(variables, "UNIX=true");
	set_variable(variables, "OS=LINUX");
	set_variable(variables, "OSPLAT=X86");
	set_variable(variables, "JAMVERSION=2.5-haiku-20111222");

	// import environment
// TODO: Platform specific!
	for (size_t i = 0; environ[i] != NULL; i++)
		set_variable(variables, environ[i]);

	// parse arguments
	const char* jambaseFile = NULL;
	const char* actionsOutputFile = NULL;
	behavior::Compatibility compatibility = behavior::COMPATIBILITY_HAM;
	bool explicitCompatibility = false;
	bool buildFromNewest = false;
	int jobCount = 1;
	bool dryRun = false;
	bool quitOnError = false;
	data::StringList forceUpdateTargets;

	int argi = 1;
	while (argi < argc) {
		const char* arg = argv[argi];
		if (arg[0] != '-')
			break;

		argi++;
		arg++;

		if (*arg == '-') {
			// long ("--") option -- map to short option
			if (strcmp(arg, "-all") == 0)
				arg = "a";
			if (strcmp(arg, "-compatibility") == 0)
				arg = "c";
			else if (strcmp(arg, "-jambase") == 0)
				arg = "f";
			else if (strcmp(arg, "-from-newest") == 0)
				arg = "g";
			else if (strcmp(arg, "-help") == 0)
				arg = "h";
			else if (strcmp(arg, "-jobs") == 0)
				arg = "j";
			else if (strcmp(arg, "-dry-run") == 0)
				arg = "n";
			else if (strcmp(arg, "-actions") == 0)
				arg = "o";
			else if (strcmp(arg, "-quit-on-error") == 0)
				arg = "q";
			else if (strcmp(arg, "-set") == 0)
				arg = "s";
			else if (strcmp(arg, "-target") == 0)
				arg = "t";
			else if (strcmp(arg, "-version") == 0)
				arg = "v";
			else
				print_usage_end_exit(programName, true);
		}

		// short ("-") option(s)
		for (; *arg != '\0'; arg++) {
			switch (*arg) {
				case 'c':
				{
					if (argi == argc)
						print_usage_end_exit(programName, true);

					const char* compatibilityString = argv[argi++];
					if (strcmp(compatibilityString, "jam") == 0) {
						compatibility = behavior::COMPATIBILITY_JAM;
					} else if (strcmp(compatibilityString, "boost") == 0) {
						compatibility = behavior::COMPATIBILITY_BOOST_JAM;
					} else if (strcmp(compatibilityString, "ham") == 0) {
						compatibility = behavior::COMPATIBILITY_HAM;
					} else {
						fprintf(stderr, "Error: Invalid argument for "
							"compatibility option: \"%s\"\n",
							compatibilityString);
						exit(1);
					}
					explicitCompatibility = true;
					break;
				}

				case 'd':
// TODO: Debug options!
//					if (argi == argc)
//						print_usage_end_exit(programName, true);
//					testDataDirectory = argv[argi++];
//					break;

				case 'f':
					if (argi == argc)
						print_usage_end_exit(programName, true);
					jambaseFile = argv[argi++];
					break;

				case 'g':
					buildFromNewest = true;
					break;

				case 'h':
					print_usage_end_exit(programName, false);

				case 'j':
				{
					if (argi == argc)
						print_usage_end_exit(programName, true);
					char* end;
					jobCount = strtol(argv[argi++], &end, 0);
					if (*end != '\0')
						print_usage_end_exit(programName, true);
					break;
				}

				case 'n':
					dryRun = true;
					break;

				case 'o':
					if (argi == argc)
						print_usage_end_exit(programName, true);
					actionsOutputFile = argv[argi++];
					break;

				case 'q':
					quitOnError = true;
					break;

				case 's':
				{
					if (argi == argc || !set_variable(variables, argv[argi++]))
						print_usage_end_exit(programName, true);
					break;
				}

				case 't':
					if (argi == argc)
						print_usage_end_exit(programName, true);

					forceUpdateTargets.Append(String(argv[argi++]));
					break;

				case 'v':
					printf("Ham 0.1, Copyright 2010-2013 Ingo Weinhold.");
					exit(0);

				default:
					print_usage_end_exit(programName, true);
			}
		}
	}

	// get targets to be made
	StringList primaryTargets;
	for (; argi < argc; argi++)
		primaryTargets.Append(String(argv[argi]));

	// "all" is the default target, if none is given.
	if (primaryTargets.IsEmpty())
		primaryTargets.Append("all");

	make::Processor processor;
// TODO: Add environmental variables!

	// set explicitly specified variables
	for (std::map<data::String, data::String>::iterator it = variables.begin();
		it != variables.end(); ++it) {
		data::StringList value;
		value.Append(it->second);
		processor.GlobalVariables().Set(it->first, value);
	}

	// Set compatibility. If not specified explicitly, infer from the program
	// name.
	if (!explicitCompatibility) {
		const char* slash = strrchr(programName, '/');
		const char* baseName = slash != NULL ? slash + 1 : programName;
		if (strcmp(baseName, "jam") == 0)
			compatibility = behavior::COMPATIBILITY_JAM;
		else if (strcmp(baseName, "bjam") == 0)
			compatibility = behavior::COMPATIBILITY_BOOST_JAM;
	}

	processor.SetCompatibility(compatibility);

	// set other options
	processor.SetPrimaryTargets(primaryTargets);
	if (jambaseFile != NULL)
		processor.SetJambaseFile(jambaseFile);
	processor.SetBuildFromNewest(buildFromNewest);
	processor.SetJobCount(jobCount);
	processor.SetDryRun(dryRun);
	if (actionsOutputFile != NULL)
		processor.SetActionsOutputFile(actionsOutputFile);
	processor.SetQuitOnError(quitOnError);

	try {
		// execute the jam code
		processor.ProcessJambase();

		// Set the targets that shall be made, even if up-to-date.
		processor.SetForceUpdateTargets(forceUpdateTargets);

		// prepare the targets
		processor.PrepareTargets();
	} catch (make::MakeException& exception) {
		fprintf(stderr, "%s.\n", exception.Message());
	} catch (util::TextFileException& exception) {
		const util::TextFilePosition& position = exception.Position();
		fprintf(stderr, "%s:%zu:%zu: %s.\n", position.FileName(),
			position.Line() + 1, position.Column() + 1, exception.Message());
	}

// TODO: Catch exceptions...

	return 0;
}
