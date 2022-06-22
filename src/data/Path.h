/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_PATH_H
#define HAM_DATA_PATH_H

#include "behavior/Behavior.h"
#include "data/StringBuffer.h"
#include "data/StringPart.h"

namespace ham
{
namespace data
{

class FileStatus;

class Path
{
  public:
	class Parts;

  public:
	static bool IsAbsolute(const StringPart& path);
	static StringPart RemoveGrist(const StringPart& path);
	static String Make(const StringPart& head, const StringPart& tail);
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
 */
struct Path::Parts {
  public:
	Parts() {}
	Parts(const StringPart& path) { SetTo(path); }

	void SetTo(const StringPart& path);
	void GetPath(StringBuffer& buffer,
				 const behavior::Behavior& behavior) const;
	inline String ToPath(const behavior::Behavior& behavior) const;

	bool IsAbsolute() const;

	const StringPart& Grist() const { return fGrist; }
	void SetGrist(const StringPart& grist) { fGrist = grist; }
	void UnsetGrist() { fGrist.Unset(); }

	const StringPart& Root() const { return fRoot; }
	void SetRoot(const StringPart& root) { fRoot = root; }
	void UnsetRoot() { fRoot.Unset(); }

	const StringPart& Directory() const { return fDirectory; }
	void SetDirectory(const StringPart& directory) { fDirectory = directory; }
	void UnsetDirectory() { fDirectory.Unset(); }

	const StringPart& BaseName() const { return fBaseName; }
	void SetBaseName(const StringPart& baseName) { fBaseName = baseName; }
	void UnsetBaseName() { fBaseName.Unset(); }

	const StringPart& Suffix() const { return fSuffix; }
	void SetSuffix(const StringPart& suffix) { fSuffix = suffix; }
	void UnsetSuffix() { fSuffix.Unset(); }

	const StringPart& ArchiveMember() const { return fArchiveMember; }
	void SetArchiveMember(const StringPart& archiveMember)
	{
		fArchiveMember = archiveMember;
	}
	void UnsetArchiveMember() { fArchiveMember.Unset(); }

  private:
	StringPart fGrist;
	StringPart fRoot;
	StringPart fDirectory;
	StringPart fBaseName;
	StringPart fSuffix;
	StringPart fArchiveMember;
};

/*static*/ inline bool
Path::IsAbsolute(const StringPart& path)
{
	// TODO: Platform dependent!
	return !path.IsEmpty() && path.Start()[0] == '/';
}

String
Path::Parts::ToPath(const behavior::Behavior& behavior) const
{
	StringBuffer buffer;
	GetPath(buffer, behavior);
	return buffer;
}

} // namespace data
} // namespace ham

#endif // HAM_DATA_PATH_H
