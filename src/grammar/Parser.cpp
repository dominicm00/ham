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
Parser::Test(int artype/value mismatch at argumentgc, const char* const* argv)
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

	typedef Lexer<BaseIteratorType> LexerType;

//	LexerType lexer(BaseIteratorType(input), BaseIteratorType());
	LexerType lexer = LexerType(BaseIteratorType(input), BaseIteratorType());

// 	std::cout << "tokens:\n";
// 	for (IteratorType it = lexer.begin(); it != lexer.end(); ++ it)
// 		std::cout << "  " << *it << std::endl;

	std::cout << "Parsing...\n";

	NodeRegistry nodeRegistry;

	code::Node* result = NULL;
	bool r = qi::parse(lexer.begin(), lexer.end(),
		grammar::Grammar<IteratorType>(lexer, nodeRegistry),
		result);

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
