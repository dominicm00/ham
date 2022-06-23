/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "data/Path.hpp"

#include "data/FileStatus.hpp"

#include <sys/stat.h>

namespace ham::data
{

static const char*
find_grist_end(const StringPart& path)
{
	const char* remainder = path.Start();
	const char* pathEnd = path.End();

	if (*remainder != '<')
		return nullptr;

	remainder = std::find(remainder + 1, pathEnd, '>');
	if (remainder == pathEnd)
		return nullptr;

	return remainder + 1;
}

// #pragma mark - Path

/*static*/ StringPart
Path::RemoveGrist(const StringPart& path)
{
	if (const char* gristEnd = find_grist_end(path))
		return StringPart(gristEnd, path.End());
	return path;
}

/**
 * Concatenates `head` onto `tail`, if `tail` is not already an absolute path.
 *
 * \param[in] head The string to use as the prefix.
 * \param[in] tail The string to use as the suffix.
 */
String
Path::Make(const StringPart& head, const StringPart& tail)
{
	if (head.IsEmpty())
		return String(tail);
	if (tail.IsEmpty())
		return String(head);
	if (IsAbsolute(tail))
		return String(tail);

	// TODO: Path separator!
	StringBuffer buffer;
	buffer += head;
	if (head.End()[-1] != '/')
		buffer += '/';
	buffer += tail;
	return buffer;
}

/*static*/ bool
Path::Exists(const char* path)
{
	FileStatus status;
	return GetFileStatus(path, status);
}

/*static*/ bool
Path::GetFileStatus(const char* path, FileStatus& _status)
{
	// TODO: Platform specific!
	struct stat st;
	if (lstat(path, &st) != 0) {
		_status = FileStatus();
		return false;
	}

	FileStatus::Type type;
	if (S_ISREG(st.st_mode))
		type = FileStatus::FILE;
	else if (S_ISDIR(st.st_mode))
		type = FileStatus::DIRECTORY;
	else if (S_ISLNK(st.st_mode))
		type = FileStatus::SYMLINK;
	else
		type = FileStatus::OTHER;

	_status = FileStatus(type, Time(st.st_mtim.tv_sec, st.st_mtim.tv_nsec));
	return true;
}

// #pragma mark - Path::Parts

bool
Path::Parts::IsAbsolute() const
{
	return Path::IsAbsolute(fDirectory) || Path::IsAbsolute(fRoot);
}

/**
 * Decomposes a string into Path::Parts.
 *
 * \param[in] path String representation of a Path.
 */
void
Path::Parts::SetTo(const StringPart& path)
{
	// TODO: This is platform dependent!

	const char* remainder = path.Start();
	const char* pathEnd = path.End();

	// grist
	if (const char* gristEnd = find_grist_end(path)) {
		fGrist.SetTo(remainder, gristEnd);
		remainder = gristEnd;
	} else
		fGrist.Unset();

	// root
	fRoot.Unset();

	// directory path
	if (const char* lastSlash = strrchr(remainder, '/')) {
		fDirectory.SetTo(remainder,
						 lastSlash == remainder ? remainder + 1 : lastSlash);
		remainder = lastSlash + 1;
	} else
		fDirectory.Unset();

	// archive member
	const char* archiveMemberStart = nullptr;
	if (remainder != pathEnd && pathEnd[-1] == ')')
		archiveMemberStart = strchr(remainder, '(');
	if (archiveMemberStart != nullptr)
		fArchiveMember.SetTo(archiveMemberStart + 1, pathEnd - 1);
	else
		fArchiveMember.Unset();

	// suffix
	const char* fileNameEnd =
		archiveMemberStart != nullptr ? archiveMemberStart : pathEnd;
	typedef std::reverse_iterator<const char*> ReverseStringIterator;
	const char* lastDot = std::find(ReverseStringIterator(fileNameEnd),
									ReverseStringIterator(remainder),
									'.')
							  .base()
		- 1;
	if (lastDot != remainder - 1) {
		fSuffix.SetTo(lastDot, fileNameEnd);
		fileNameEnd = lastDot;
	} else
		fSuffix.Unset();

	// base name
	fBaseName.SetTo(remainder, fileNameEnd);
}

/**
 * Insert the string representation of the path into `buffer`.
 *
 * \deprecated This method is dangerous because we don't know the string length
 * ahead of time. It will be replaced with a variant that returns a StringBuffer
 * directly.
 *
 * \param[out] buffer   Buffer where path will be inserted.
 * \param[in]  behavior Compatibility behavior to emulate.
 */
void
Path::Parts::GetPath(StringBuffer& buffer,
					 const behavior::Behavior& behavior) const
{
	// TODO: This is platform dependent!

	if (!fGrist.IsEmpty()) {
		if (fGrist.Start()[0] != '<')
			buffer += '<';
		buffer += fGrist;
		if (fGrist.End()[-1] != '>')
			buffer += '>';
	}

	// Use root only, if the directory part isn't absolute and if the root isn't
	// ".".
	if (!fRoot.IsEmpty() && (fRoot.Length() > 1 || fRoot.Start()[0] != '.')
		&& !Path::IsAbsolute(fDirectory)) {
		buffer += fRoot;
		if (behavior.GetPathRootReplacerSlash()
				== behavior::Behavior::PATH_ROOT_REPLACER_SLASH_ALWAYS
			|| fRoot.End()[-1] != '/') {
			buffer += '/';
		}
	}

	if (!fDirectory.IsEmpty()) {
		buffer += fDirectory;
		if (fDirectory != StringPart("/", 1)
			&& (!fBaseName.IsEmpty() || !fSuffix.IsEmpty())) {
			buffer += '/';
		}
	}

	buffer += fBaseName;
	buffer += fSuffix;

	if (!fArchiveMember.IsEmpty()) {
		buffer += '(';
		buffer += fArchiveMember;
		buffer += ')';
	}
}

} // namespace ham::data
