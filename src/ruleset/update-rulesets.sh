#!/bin/sh

{ copyright=$(cat) ; } <<'EOF'
/*
 * Copyright 2022, Dominic Martinez, dom@dominicm.dev.
 * Distributed under the terms of the MIT License.
 */
EOF

{ header_template=$(cat) ; }<<'EOF'
#ifndef HAM_RULESET_rulesetu_H
#define HAM_RULESET_rulesetu_H

#include <string>

namespace ham
{
namespace ruleset
{

extern const std::string rulesetc;

} // namespace ruleset
} // namespace ham

#endif // HAM_RULESET_rulesetu_H
EOF

# rulesets
set -- HamRuleset JamRuleset

for ruleset; do
	file="${ruleset}.cpp"
	header="${ruleset}.h"

	# get UPPER_CASE version of ruleset
	ruleset_upper=$(echo "$ruleset" \
						| sed -e 's/\([A-Z]\)/-\1/g' \
							  -e 's/^-//' \
						| tr '-' '_' \
						| tr '[:lower:]' '[:upper:]')

	# create header file
	{
		printf "%s\n" "$copyright" ;
		printf "%s" "$header_template" \
			| sed -e "s/rulesetu/${ruleset_upper}/" \
				  -e "s/rulesetc/${ruleset}/" ;
	} > "$header"

	# create data file
	{
		printf "%s\n" "$copyright" ;
		printf "#include \"ruleset/%s\"\n\n" "$header" ;
		printf "#include <string>\n\n" ;
		printf "const std::string ham::ruleset::%s =\n" "$ruleset" ;
	} > "$file"

    sed -e '/^$/d' \
		-e '/^#/d' \
		-e 's/\\/\\\\/g' \
		-e 's/"/\\"/g' \
		-e 's/^/  "/' \
		-e 's/$/\\n"/' \
		< "$ruleset" >> "$file"
	printf ";\n" >> "$file"
done
