/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_PATH_HPP
#define HAM_DATA_PATH_HPP

#include "behavior/Behavior.hpp"

#include <string>
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
	static bool Exists(const char* path);
	static bool GetFileStatus(const char* path, FileStatus& _status);
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
 *
 * For performance, this class uses string views to decompose and manipulate an
 * existing string. Path::Parts must not outlive the scope of any strings it is
 * passed.
 */
class Path::Parts
{
  public:
	Parts() {}
	Parts(std::string_view path);

	std::string ToPath(const behavior::Behavior& behavior) const;

	bool IsAbsolute() const;

	std::string Grist() const { return std::string{fGrist}; }
	void SetGrist(std::string_view grist) { fGrist = grist; }
	void UnsetGrist() { fGrist = {}; }

	std::string Root() const { return std::string{fRoot}; }
	void SetRoot(std::string_view root) { fRoot = root; }
	void UnsetRoot() { fRoot = {}; }

	std::string Directory() const { return std::string{fDirectory}; }
	void SetDirectory(std::string_view directory) { fDirectory = directory; }
	void UnsetDirectory() { fDirectory = {}; }

	std::string BaseName() const { return std::string{fBaseName}; }
	void SetBaseName(std::string_view baseName) { fBaseName = baseName; }
	void UnsetBaseName() { fBaseName = {}; }

	std::string Suffix() const { return std::string{fSuffix}; }
	void SetSuffix(std::string_view suffix) { fSuffix = suffix; }
	void UnsetSuffix() { fSuffix = {}; }

	std::string ArchiveMember() const { return std::string{fArchiveMember}; }
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
	return !path.empty() && path.front() == '/';
}

} // namespace ham::data

#endif // HAM_DATA_PATH_HPP
