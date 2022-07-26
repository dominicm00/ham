#include "HamConfig.hpp"
#include "parse/Parser.hpp"

#include <iostream>
#include <ostream>

int
main(int argc, char* argv[])
{
	std::cout << "Version " << HAM_VERSION_MAJOR << "." << HAM_VERSION_MINOR
			  << std::endl;

	if (argc != 2)
		return 1;

	// Start a parsing run of argv[1] with the string
	// variable 'name' as additional argument to the
	// action; then print what the action put there.
	Parser::Parse(std::string{argv[1]});
	return 0;
}
