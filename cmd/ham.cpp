#include "HamConfig.hpp"
#include "parse/Parser.hpp"

#include <cstring>
#include <iostream>
#include <ostream>

using namespace ham;

int
main(int argc, char* argv[])
{
	if (argc != 3)
		return 1;

	parse::Parser::Debug debug;
	const char* debugInput = argv[1];
	if (std::strcmp(debugInput, "none") == 0) {
		debug = parse::Parser::NONE;
	} else if (std::strcmp(debugInput, "xdot") == 0) {
		debug = parse::Parser::XDOT;
	} else if (std::strcmp(debugInput, "trace") == 0) {
		debug = parse::Parser::TRACE;
	} else if (std::strcmp(debugInput, "grammar") == 0) {
		debug = parse::Parser::GRAMMAR;
	} else {
		std::cerr << "Invalid debug selection" << std::endl;
		exit(1);
	}

	ham::parse::Parser::Parse(std::string{argv[2]}, debug);
	return 0;
}
