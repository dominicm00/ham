/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_PATH_HPP
#define HAM_DATA_PATH_HPP

#include "behavior/Behavior.hpp"

#include <string_view>

namespace ham::data
{

class FileStatus;

class Path
{
  public:
	class Parts;

  public:
	static bool IsAbsolute(std::string_view path);
	static std::string_view RemoveGrist(std::string_view path);
	static std::string Make(std::string_view head, std::string_view tail);
	static bool Exists(std::string_view path);
	static bool GetFileStatus(std::string_view path, FileStatus& _status);
};

/**
 * Represents a Ham path in terms of the following parts:
 * - Grist: prefix enclosed in <> that differentiates targets with the same
 *   name.
 * - Root: initially empty, absolute root if given path is relative.
 * - Directory: given directory path to target.
 * - Base name: "file" name.
 * - Suffix: final file suffix.
 * - Archive member: a suffix enclosed in () that sets the archive target.
 *
 * Below is an example of a Ham path:
 \verbatim
 <grist>path/to/basename.suffix(member)

 # If root was set to /some/root, then the final path would be:
 <grist>/some/root/path/to/basename.suffix(member)
 \endverbatim
 */
class Path::Parts
{
  public:
	Parts() {}
	Parts(std::string_view path) { SetTo(path); }

	void SetTo(std::string_view path);
	std::string ToPath(const behavior::Behavior& behavior) const;

	bool IsAbsolute() const;

	std::string_view Grist() const { return fGrist; }
	void SetGrist(std::string_view grist) { fGrist = grist; }
	void UnsetGrist() { fGrist = {}; }

	std::string_view Root() const { return fRoot; }
	void SetRoot(std::string_view root) { fRoot = root; }
	void UnsetRoot() { fRoot = {}; }

	std::string_view Directory() const { return fDirectory; }
	void SetDirectory(std::string_view directory) { fDirectory = directory; }
	void UnsetDirectory() { fDirectory = {}; }

	std::string_view BaseName() const { return fBaseName; }
	void SetBaseName(std::string_view baseName) { fBaseName = baseName; }
	void UnsetBaseName() { fBaseName = {}; }

	std::string_view Suffix() const { return fSuffix; }
	void SetSuffix(std::string_view suffix) { fSuffix = suffix; }
	void UnsetSuffix() { fSuffix = {}; }

	std::string_view ArchiveMember() const { return fArchiveMember; }
	void SetArchiveMember(std::string_view archiveMember)
	{
		fArchiveMember = archiveMember;
	}
	void UnsetArchiveMember() { fArchiveMember = {}; }

  private:
	std::string_view fGrist;
	std::string_view fRoot;
	std::string_view fDirectory;
	std::string_view fBaseName;
	std::string_view fSuffix;
	std::string_view fArchiveMember;
};

/*static*/ inline bool
Path::IsAbsolute(std::string_view path)
{
	// TODO: Platform dependent!
	return !path.IsEmpty() && path.Start()[0] == '/';
}

} // namespace ham::data

#endif // HAM_DATA_PATH_HPP
