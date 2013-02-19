/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/TargetBinder.h"

#include "code/EvaluationContext.h"
#include "data/MakeTarget.h"
#include "data/Path.h"


namespace ham {
namespace code {


static const String kLocateVariableName("LOCATE");
static const String kSearchVariableName("SEARCH");


/*static*/ void
TargetBinder::Bind(EvaluationContext& context, const data::Target* target,
	String& _boundPath)
{
	using data::Path;

	// If the target name is an absolute path, that's also the bound path (minus
	// the grist).
	data::StringPart targetPath(Path::RemoveGrist(target->Name()));
	if (Path::IsAbsolute(targetPath)) {
		_boundPath = targetPath;
		return;
	}

	// If the LOCATE variable is set on the target or globally, we prepend it to
	// the target name.
	const data::VariableDomain* variables = target->Variables();

	const StringList* locatePaths = NULL;

	if (variables != NULL)
		locatePaths = variables->Lookup(kLocateVariableName);

	if (locatePaths == NULL || locatePaths->IsEmpty())
		locatePaths = context.LookupVariable(kLocateVariableName);

	if (locatePaths != NULL && !locatePaths->IsEmpty()) {
		// prepend the LOCATE path
		_boundPath = Path::Make(locatePaths->Head(), targetPath);
		return;
	}

	// If SEARCH is set on the target or globally, we iterate through the list,
	// composing a path with each element, until we've found a path that refers
	// to an existing entry.
	const StringList* searchPaths = NULL;

	if (variables != NULL)
		searchPaths = variables->Lookup(kSearchVariableName);

	if (searchPaths == NULL || searchPaths->IsEmpty())
		searchPaths = context.LookupVariable(kSearchVariableName);

	if (searchPaths != NULL && !searchPaths->IsEmpty()) {
		size_t pathCount = searchPaths->Size();
		for (size_t i = 0; i < pathCount; i++) {
			// prepend the LOCATE path
			String path = Path::Make(searchPaths->ElementAt(i), targetPath);
			if (Path::Exists(path.ToCString())) {
				_boundPath = path;
				return;
			}
		}
	}

	// Not found -- use the target name.

	_boundPath = targetPath;
}


/*static*/ void
TargetBinder::Bind(EvaluationContext& context, data::MakeTarget* target)
{
	if (target->IsBound())
		return;

	String boundPath;
	Bind(context, target->GetTarget(), boundPath);
	target->SetBoundPath(boundPath);
}


}	// namespace code
}	// namespace ham
