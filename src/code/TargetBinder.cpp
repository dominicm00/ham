/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "code/TargetBinder.h"

#include "code/EvaluationContext.h"
#include "data/Path.h"
#include "data/Target.h"


namespace ham {
namespace code {


static const String kLocateVariableName("LOCATE");
static const String kSearchVariableName("SEARCH");


/*static*/ void
TargetBinder::Bind(EvaluationContext& context, data::Target* target)
{
	using data::Path;

	if (target->IsBound())
		return;

	// If the target name is an absolute path, that's also the bound path (minus
	// the grist).
	data::StringPart targetPath(Path::RemoveGrist(target->Name()));
	if (Path::IsAbsolute(targetPath)) {
		target->SetBoundPath(String(targetPath));
		return;
	}

	// If the LOCATE variable is set on the target or globally, we prepend it to
	// the target name.
	data::VariableDomain* variables = target->Variables(false);

	const StringList* locatePaths = NULL;

	if (variables != NULL)
		locatePaths = variables->Lookup(kLocateVariableName);

	if (locatePaths == NULL || locatePaths->IsEmpty())
		locatePaths = context.LookupVariable(kLocateVariableName);

	if (locatePaths != NULL && !locatePaths->IsEmpty()) {
		// prepend the LOCATE path
		target->SetBoundPath(Path::Make(locatePaths->Head(), targetPath));
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
				target->SetBoundPath(path);
				return;
			}
		}
	}

	// Not found -- use the target name.
	target->SetBoundPath(String(targetPath));
}


}	// namespace code
}	// namespace ham
