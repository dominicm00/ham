/*
 * Copyright 2022, Dominic Martinez, dom@dominicm.dev.
 * Distributed under the terms of the MIT License.
 */

#include "tests/RulesetTest.hpp"

#include "ruleset/HamRuleset.hpp"
#include "ruleset/JamRuleset.hpp"

#include <exception>
#include <filesystem>
#include <fstream>
#include <string>

namespace ham::tests
{

void
_CheckRuleset(const std::string& pathString,
			  const std::string& path2String,
			  const std::string& ruleset)
{
	std::filesystem::path path{pathString};
	std::ifstream file{path};
	if (!file.is_open()) {
		// Attempt to use secondary path
		// XXX: This is a quick hack so the tester can be run both in the root
		// and src directories.
		std::filesystem::path path2{path2String};
		file = std::ifstream(path2);
		if (!file.is_open()) {
			throw std::runtime_error("Could not open ruleset file.");
		}
	}

	std::string fileLine;
	std::string rulesetLine;
	std::istringstream rulesetStream{ruleset};
	unsigned int lines{0};
	while (std::getline(file, fileLine)) {
		// Skip empty and commented lines.
		if (fileLine.empty() || fileLine.front() == '#')
			continue;

		// All other lines should be included.
		HAM_TEST_ADD_INFO(
			HAM_TEST_VERIFY(std::getline(rulesetStream, rulesetLine)),
			"ruleset ended unexpectedly at line %d",
			lines)

		HAM_TEST_EQUAL(fileLine, rulesetLine)
	}

	// Ruleset should not have additional, non-empty lines.
	if (std::getline(rulesetStream, rulesetLine)) {
		HAM_TEST_VERIFY(rulesetLine.empty())
	}
}

void
RulesetTest::JamRuleset()
{
	_CheckRuleset("src/ruleset/JamRuleset.ham",
				  "ruleset/JamRuleset.ham",
				  ruleset::kJamRuleset);
}

void
RulesetTest::HamRuleset()
{
	_CheckRuleset("src/ruleset/HamRuleset.ham",
				  "ruleset/HamRuleset.ham",
				  ruleset::kHamRuleset);
}

} // namespace ham::tests
