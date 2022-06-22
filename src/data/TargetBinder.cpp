/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "data/TargetBinder.h"

#include "data/FileStatus.h"
#include "data/Path.h"
#include "data/Target.h"
#include "data/VariableDomain.h"

namespace ham
{
namespace data
{

static const String kLocateVariableName("LOCATE");
static const String kSearchVariableName("SEARCH");

/**
 * Bind a target to a filesystem path and get the file status.
 *
 * \param[in] globalVariables Global variable domain.
 * \param[in] target Target to bind.
 * \param[out] _boundPath Filesystem path of target.
 * \param[out] _fileStatus File status of bound path.
 */
/*static*/ void
TargetBinder::Bind(const VariableDomain& globalVariables,
				   const Target* target,
				   String& _boundPath,
				   FileStatus& _fileStatus)
{
	// If the target name is an absolute path, that's also the bound path (minus
	// the grist).
	// TODO: This isn't a valid path since it doesn't strip member archives.
	StringPart targetPath(Path::RemoveGrist(target->Name()));
	if (Path::IsAbsolute(targetPath)) {
		_boundPath = targetPath;
		Path::GetFileStatus(_boundPath.ToCString(), _fileStatus);
		return;
	}

	// If the LOCATE variable is set on the target or globally, we prepend it to
	// the target name.
	const VariableDomain* variables = target->Variables();

	const StringList* locatePaths = nullptr;

	if (variables != nullptr)
		locatePaths = variables->Lookup(kLocateVariableName);

	if (locatePaths == nullptr || locatePaths->IsEmpty())
		locatePaths = globalVariables.Lookup(kLocateVariableName);

	if (locatePaths != nullptr && !locatePaths->IsEmpty()) {
		// prepend the LOCATE path
		_boundPath = Path::Make(locatePaths->Head(), targetPath);
		Path::GetFileStatus(_boundPath.ToCString(), _fileStatus);
		return;
	}

	// If SEARCH is set on the target or globally, we iterate through the list,
	// composing a path with each element, until we've found a path that refers
	// to an existing entry.
	const StringList* searchPaths = nullptr;

	if (variables != nullptr)
		searchPaths = variables->Lookup(kSearchVariableName);

	if (searchPaths == nullptr || searchPaths->IsEmpty())
		searchPaths = globalVariables.Lookup(kSearchVariableName);

	if (searchPaths != nullptr && !searchPaths->IsEmpty()) {
		size_t pathCount = searchPaths->Size();
		for (size_t i = 0; i < pathCount; i++) {
			// prepend the LOCATE path
			String path = Path::Make(searchPaths->ElementAt(i), targetPath);
			if (Path::GetFileStatus(path.ToCString(), _fileStatus)) {
				_boundPath = path;
				return;
			}
		}
	}

	// Not found -- use the target name.
	_boundPath = targetPath;
	Path::GetFileStatus(_boundPath.ToCString(), _fileStatus);
}

} // namespace data
} // namespace ham
