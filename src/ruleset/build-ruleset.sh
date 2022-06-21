#!/bin/sh

{ copyright=$(cat) ; } <<'EOF'
/*
 * Copyright 2022, Dominic Martinez, dom@dominicm.dev.
 * Distributed under the terms of the MIT License.
 */
EOF

{ header_template=$(cat) ; }<<'EOF'
#ifndef HAM_RULESET_ruleset_upper_H
#define HAM_RULESET_ruleset_upper_H

#include <string>

namespace ham
{
namespace ruleset
{

extern const std::string ruleset_variable;

} // namespace ruleset
} // namespace ham

#endif // HAM_RULESET_ruleset_upper_H
EOF

file="$1"
ruleset_file="$2"

ruleset_path=${2%%.*}
ruleset=${ruleset_path##*/}
ruleset_variable=k${ruleset}
header_file=ruleset/${ruleset}.h
suffix=${1##*.}

created=0

if [ "$suffix" = "h" ]; then
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
			| sed -e "s/ruleset_upper/${ruleset_upper}/" \
				  -e "s/ruleset_variable/${ruleset_variable}/" ;
	} > "$file"

	created=1
fi

if [ "$suffix" = "cpp" ]; then
	# create data file
	{
		printf "%s\n" "$copyright" ;
		printf "#include \"%s\"\n\n" "$header_file" ;
		printf "#include <string>\n\n" ;
		printf "const std::string ham::ruleset::%s =\n" "$ruleset_variable" ;
		sed -e '/^$/d' \
			-e '/^#/d' \
			-e 's/\\/\\\\/g' \
			-e 's/"/\\"/g' \
			-e 's/^/  "/' \
			-e 's/$/\\n"/' \
			< "$ruleset_file" ;
		printf ";\n" ;
	} > "$file"

	created=1
fi

if [ $created -eq 0 ]; then
	echo "Invalid file suffix."
	exit 1
fi
