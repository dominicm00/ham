/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include <iterator>
#include <iostream>
#include <limits>
#include <list>
#include <stdexcept>
#include <string>

#include "parser/Lexer.h"


int
main(int argc, const char* const* argv)
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


	typedef std::istream_iterator<char> BaseIteratorType;
	typedef parser::Lexer<BaseIteratorType> LexerType;

	LexerType lexer;
	lexer.Init(BaseIteratorType(input), BaseIteratorType());

 	std::cout << "tokens:\n";
 	while (lexer.CurrentToken() != parser::TOKEN_EOF) {
 		std::cout << "  " << lexer.CurrentToken() << std::endl;
		lexer.NextToken();
	}

	return 0;
}
