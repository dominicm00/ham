#include "HamConfig.hpp"
#include "parse/Parser.hpp"

#include <iostream>
#include <ostream>

int
main(int argc, char* argv[])
{
	if (argc != 2)
		return 1;

	ham::parse::Parser::Parse(std::string{argv[1]});
	return 0;
}
