/*
 * Copyright 2010-2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/Leaf.h"

#include <algorithm>
#include <limits>

#include "code/DumpContext.h"
#include "code/EvaluationContext.h"
#include "data/StringListOperations.h"


namespace ham {
namespace code {


Leaf::Leaf(const String& string)
	:
	fString(string)
{
}

Leaf::~Leaf()
{
}


StringList
Leaf::Evaluate(EvaluationContext& context)
{
	const char* string = fString.ToCString();
	return _EvaluateString(context, string, string + fString.Length(),
		&fString);
}


code::Node*
Leaf::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	return NULL;
}


void
Leaf::Dump(DumpContext& context) const
{
	context << "Leaf(\"" << fString << "\")\n";
}


/*static*/ StringList
Leaf::_EvaluateString(EvaluationContext& context, const char* stringStart,
	const char* stringEnd, const String* originalString)
{
	// The string to evaluate is a alternating sequence of literal strings and
	// variable expansion expressions. Each literal string can be considered a
	// single element string list and each variable expansion expression expands
	// to a string list as well. The product of the list of all string lists is
	// the result we want to compute. We proceed accordingly, i.e. split the
	// input string in literal strings and variable expansion expressions,
	// evaluate the latter as we go, and finally compute the string list
	// product. Recursive variable expansion expressions we evaluate using
	// recursion.
	StringListList resultFactors;

	const char* literalStringStart = stringStart;
	const char* stringRemainder = literalStringStart;

	while (stringRemainder != stringEnd) {
		// find the next "$("
		stringRemainder = std::find(stringRemainder, stringEnd, '$');
		if (stringRemainder == stringEnd)
			break;

		if (++stringRemainder == stringEnd)
			break;

		if (*stringRemainder != '(')
			continue;

		// Add the literal string segment before the current variable to the
		// result factors.
		if (literalStringStart < stringRemainder - 1) {
			resultFactors.push_back(StringList(
				String(literalStringStart,
					stringRemainder - 1 - literalStringStart)));
		}

		const char* variableStart = ++stringRemainder;

		// Find the matching closing ")". While at it also find the containing
		// special characters (":", "[", "]") at the top level.
		const char* colon = NULL;
		const char* openingBracket = NULL;
		const char* closingBracket = NULL;
		bool recursive = false;
		int matchCount = 1;
		while (matchCount != 0 && stringRemainder != stringEnd) {
			switch (*stringRemainder) {
				case '(':
					matchCount++;
					recursive = true;
					break;
				case ')':
					matchCount--;
					break;
				case ':':
					if (matchCount == 1 && colon == NULL)
						colon = stringRemainder;
					break;
				case '[':
					if (matchCount == 1 && openingBracket == NULL)
						openingBracket = stringRemainder;
					break;
				case ']':
					if (matchCount == 1 && closingBracket == NULL)
						closingBracket = stringRemainder;
					break;
				default:
					break;
			}
			stringRemainder++;
		}

		if (matchCount != 0) {
			// TODO: Syntax error!
			break;
		}

		// Evaluate the variable. If its value is empty, the end result will be
		// empty, too.
		StringList variableValue = _EvaluateVariableExpression(context,
			variableStart, stringRemainder - 1, colon, openingBracket,
			closingBracket, recursive);
		if (variableValue.IsEmpty())
			return variableValue;

		resultFactors.push_back(variableValue);

		literalStringStart = stringRemainder;
	}

	// If we haven't encountered any variable, just return the original string.
	if (resultFactors.empty()) {
		if (originalString != NULL)
			return StringList(*originalString);
		return StringList(String(stringStart, stringEnd - stringStart));
	}

	// Add the literal string segment after the last variable to the result
	// factors.
	if (literalStringStart != stringEnd) {
		resultFactors.push_back(StringList(
			String(literalStringStart, stringEnd - literalStringStart)));
	}

	// compute the result
	return StringList::Multiply(resultFactors);
}


/*static*/ StringList
Leaf::_EvaluateVariableExpression(EvaluationContext& context,
	const char* variableStart, const char* variableEnd, const char* colon,
	const char* openingBracket, const char* closingBracket, bool recursive)
{
	// The syntax is:
	//
	// expansion			:= variableName [ "[" elementRange "]" ]
	//							( ":" variableModifiers )*
	// elementRange			:= elementIndex [ "-" [ elementIndex ] ]
	// variableModifiers	:= variableSelector* [ variableSubstitution ]
	// variableSelector		:= "B" | "S" | "M" | "D" | "P" | "G" | "U" | "L"
	// variableSubstitution	:= variableSubstitutor "=" value
	// variableSubstitutor	:= "G" | "D" | "B" | "S" | "M" | "R" | "E" | "J"
	//
	// variableName, elementRange and variableModifiers are subject to variable
	// expansion. Each can result in a string list with more than one element.
	// The usual list multiplication rules apply, i.e.:
	// <variable1 range1 modifiers1> <variable1 range1 modifiers2> ...
	// <variable1 range2 modifiers1> ... <variable2 range1 modifiers1> ...
	//
	// Note: We're more lenient than jam, allowing ':' as a separator after
	// path part selectors. E.g. we allow "$(foo:G:B)", which jam considers
	// invalid syntax and ignores ":B".

	const char* variableNameEnd = variableEnd;

	if (colon != NULL) {
		// Ignore brackets after the colon.
		if (openingBracket != NULL && colon < openingBracket)
			openingBracket = NULL;
		if (closingBracket != NULL && colon < closingBracket)
			closingBracket = NULL;

		variableNameEnd = colon;
	}

	if (openingBracket != NULL || closingBracket != NULL) {
		// If we only have a closing bracket, consider the expression invalid
		// and return an empty list.
		if (openingBracket == NULL)
			return StringList();

		// If the closing bracket is missing, we use the next "natural
		// boundary", i.e. the colon or the variable end.
		if (closingBracket == NULL)
			closingBracket = colon != NULL ? colon : variableEnd;

		variableNameEnd = openingBracket;
	}

	if (variableStart == variableNameEnd)
		return StringList();

	// Handle the common cases first, i.e. no recursive expansion.
	if (!recursive) {
		StringList variableValue = context.LookupVariable(
			String(variableStart, variableNameEnd - variableStart));

		// range subscripts
		// The logical implementation would be to just limit variableValue to
		// the sublist specified by the subscripts. But Jam implements them
		// differently: The first subscript is applied, but from the second
		// subscript a maximum list size is computed, which is applied only
		// after the "E=..." operation has been applied.
		size_t maxSize;
		if (openingBracket != NULL) {
			size_t firstIndex;
			size_t endIndex;
			if (!_ParseSubscripts(openingBracket + 1, closingBracket,
					firstIndex, endIndex)) {
				return StringList();
			}

			if (firstIndex > 0)
				variableValue = variableValue.SubList(firstIndex, endIndex);

			maxSize = endIndex > firstIndex ? endIndex - firstIndex : 0;
		} else
			maxSize = std::numeric_limits<size_t>::max();

		// colon
		if (colon != NULL) {
			data::StringListOperations operations;
			for (;;) {
				const char* colonEnd = std::find(colon + 1, variableEnd, ':');
				operations.Parse(colon + 1, colonEnd);
				if (colonEnd == variableEnd)
					break;

				colon = colonEnd;
			}

			if (operations.HasOperations()) {
				variableValue = operations.Apply(variableValue, maxSize,
					context.GetBehavior());
			}
		} else if (maxSize < variableValue.Size())
			variableValue = variableValue.SubList(0, maxSize);

		return variableValue;
	}

// TODO:...
	return StringList();
}


/*static*/ bool
Leaf::_ParseSubscripts(const char* start, const char* end, size_t& _firstIndex,
	size_t& _endIndex)
{
	// TODO: Since Jam doesn't do much sanity checking of what it parses, its
	// behavior is weird for invalid input. We don't copy all of that behavior
	// yet.

	// Jam subscripts are 1-based and the end subscript is inclusive. We convert
	// to 0-based indices and an exclusive end index.

	// first subscript
	char* numberEnd;
	long firstIndex = strtol(start, &numberEnd, 10);
	if (numberEnd == start || numberEnd > end)
		return false;

	_firstIndex = firstIndex > 0 ? firstIndex - 1 : 0;

	// last subscript (optional)
	if (numberEnd == end || *numberEnd != '-') {
		_endIndex = _firstIndex + 1;
		return true;
	}

	const char* lastStart = numberEnd + 1;
	if (lastStart == end) {
		_endIndex = std::numeric_limits<size_t>::max();
		return true;
	}

	long endIndex = strtol(lastStart, &numberEnd, 10);
	if (numberEnd == lastStart || numberEnd > end)
		return false;

	// Emulate Jam behavior: Since jam computes a size from the second subscript
	// before checking the validity of the first subscript, we are off by as
	// much as we adjusted the first subscript, if compute the size afterwards.
	// Compensate for that.
	if (firstIndex < 1)
		endIndex += 1 - firstIndex;

	_endIndex = endIndex >= 0 ? endIndex : 0;
	return true;
}


}	// namespace code
}	// namespace ham
