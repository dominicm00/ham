/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "grammar/Spirit.h"
	// precompiled -- must be first

#include "grammar/Parser.h"

#include <fstream>

#include "code/DumpContext.h"
#include "code/Node.h"
#include "grammar/Grammar.h"
#include "grammar/Iterator.h"
#include "grammar/NodeRegistry.h"
#include "grammar/Skipper.h"


using grammar::Parser;


Parser::Parser()
{
}


void
Parser::Test(int argc, const char* const* argv)
{
#if 0
// 	if (argc < 2) {
// 		printf("Usage:...\n");
// 		return;
// 	}
//
// 	std::ifstream input(argv[1]);
	std::ifstream input("testdata/test1");
	if (input.fail()) {
		printf("Failed to open \"%s\"n", argv[1]);
		return;
	}
#else
	std::istream& input = std::cin;
#endif

	std::noskipws(input);

	using grammar::BaseIteratorType;
	using grammar::IteratorType;

	// convert input iterator to forward iterator, usable by spirit parser
#if 1
	IteratorType begin = boost::spirit::make_default_multi_pass(
		BaseIteratorType(input));
	IteratorType end;

#else
//const char* kTestString = "SubDir HAM_TOP src ;";
//const char* kTestString = "1 in x y z = a = B";
//const char* kTestString = "! x < 1 in a = B";
//const char* kTestString = "! [ myRule x : y z ] < 1 in a = B";
const char* kTestString =
"{ }\n"
"include foo ;\n"
"for myVar in $(bar) {\n"
"	if $(a) in $(something) $(else) {\n"
"		break ;\n"
"	}\n"
"	if $(foo) {\n"
"		continue ab ;\n"
"		while $(a) < $(b) {\n"
"			return c d ef ;\n"
"		}\n"
"	} else {\n"
"		jumptoeof xyz ;\n"
"	}\n"
"}\n"
"switch $(HAIKU_DISTRO_COMPATIBILITY) {\n"
"	case official	: {\n"
"	}\n"
"}\n"
/*
"switch $(HAIKU_DISTRO_COMPATIBILITY) {\n"
"	case official	: {\n"
"		HAIKU_DEFINES += HAIKU_DISTRO_COMPATIBILITY_OFFICIAL ;\n"
"		HAIKU_INCLUDE_TRADEMARKS = \"\" ;\n"
"	}\n"
"	case compatible	: {\n"
"		HAIKU_DEFINES += HAIKU_DISTRO_COMPATIBILITY_COMPATIBLE ;\n"
"		HAIKU_INCLUDE_TRADEMARKS = \"\" ;\n"
"	}\n"
"	case \"default\"	: {\n"
"		HAIKU_DEFINES += HAIKU_DISTRO_COMPATIBILITY_DEFAULT ;\n"
"		HAIKU_INCLUDE_TRADEMARKS = ;\n"
"	}\n"
"	case *			: Exit \"Invalid value for HAIKU_DISTRO_COMPATIBILITY:\"\n"
"						$(HAIKU_DISTRO_COMPATIBILITY) ;\n"
"}\n"
*/
;
	typedef const char* IteratorType;
	IteratorType begin = kTestString;
	IteratorType end = begin + strlen(begin);
#endif

	std::cout << "Parsing: \"";
	for (IteratorType it = begin; it != end; ++it)
		std::cout << *it;
	std::cout << "\" ...\n";

	NodeRegistry nodeRegistry;

	code::Node* result = NULL;
	bool r = qi::phrase_parse(begin, end,
		grammar::Grammar<IteratorType, grammar::Skipper<IteratorType> >(
			nodeRegistry),
		grammar::Skipper<IteratorType>(), result);

	if (r && begin == end) {
		std::cout << "-------------------------\n";
		std::cout << "Parsing succeeded\n";
		code::DumpContext dumpContext;
		result->Dump(dumpContext);
		std::cout << "-------------------------\n";
		nodeRegistry.Dump();

		struct CountVisitor : code::NodeVisitor {
			size_t count;

			CountVisitor()
				:
				count(0)
			{
			}

			virtual bool VisitNode(code::Node* node)
			{
				count++;
				return false;
			}
		} visitor;

		result->Visit(visitor);
		std::cout << "Counted " << visitor.count << " nodes in tree\n";

		delete result;
	} else {
		data::String rest(begin, end);
		std::cout << "-------------------------\n";
		std::cout << "Parsing failed\n";
		std::cout << "stopped at: \"" << rest << "\"\n";
		std::cout << "-------------------------\n";
		nodeRegistry.Dump();
		nodeRegistry.DeleteNodes();
	}
}
