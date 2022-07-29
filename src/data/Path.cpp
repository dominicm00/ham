/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "data/Path.hpp"

#include "data/FileStatus.hpp"

#include <string>
#include <string_view>
#include <sys/stat.h>

namespace ham::data
{

/*static*/ std::string_view
Path::RemoveGrist(std::string_view path)
{
	if (path.front() != '<')
		return path;

	auto end = path.find('>');

	if (end == std::string::npos)
		return path;

	return path.substr(end + 1);
}

/**
 * Concatenates `head` onto `tail`, if `tail` is not already an absolute path.
 *
 * \param[in] head The string to use as the prefix.
 * \param[in] tail The string to use as the suffix.
 */
std::string
Path::Make(std::string_view head, std::string_view tail)
{
	if (head.empty())
		return std::string{tail};
	if (tail.empty())
		return std::string{head};
	if (IsAbsolute(tail))
		return std::string{tail};

	// TODO: Path separator!
	std::string buffer;
	buffer += head;
	if (head.back() != '/')
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
Path::Parts::Parts(std::string_view path)
{
	// TODO: This is platform dependent!
	std::string_view remainder{path};

	if (remainder.empty())
		return;

	// grist
	if (remainder.front() == '<') {
		const auto gristEnd = remainder.find('>');
		if (gristEnd != std::string::npos) {
			fGrist = remainder.substr(0, gristEnd + 1);
			remainder = remainder.substr(gristEnd + 1);
		}
	}

	// directory path
	const auto lastSlash = remainder.rfind('/');
	if (lastSlash != std::string::npos) {
		fDirectory = remainder.substr(0, lastSlash + 1);
		remainder = remainder.substr(lastSlash + 1);
	}

	// archive member
	auto archiveMemberStart = std::string::npos;
	if (!remainder.empty() && remainder.back() == ')')
		archiveMemberStart = remainder.find('(');
	if (archiveMemberStart != std::string::npos) {
		fArchiveMember = remainder.substr(archiveMemberStart);
		remainder = remainder.substr(0, archiveMemberStart);
	}

	// suffix

	// TODO: Like Jam, we count hidden files like .hidden as having a suffix.
	// This should be compatibility behavior.
	const auto lastDot = remainder.rfind('.');
	if (lastDot != std::string::npos)
		fSuffix = remainder.substr('.');

	// base name
	fBaseName = remainder.substr(0, lastDot);
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
std::string
Path::Parts::ToPath(const behavior::Behavior& behavior) const
{
	std::string buffer;
	// TODO: This is platform dependent!

	if (!fGrist.empty()) {
		if (fGrist.front() != '<')
			buffer += '<';
		buffer += fGrist;
		if (fGrist.back() != '>')
			buffer += '>';
	}

	// Use root only, if the directory part isn't absolute and if the root isn't
	// ".".
	if (!fRoot.empty() && (fRoot.length() > 1 || fRoot.front() != '.')
		&& !Path::IsAbsolute(fDirectory)) {
		buffer += fRoot;
		if (behavior.GetPathRootReplacerSlash()
				== behavior::Behavior::PATH_ROOT_REPLACER_SLASH_ALWAYS
			|| fRoot.back() != '/') {
			buffer += '/';
		}
	}

	if (!fDirectory.empty()) {
		buffer += fDirectory;
		if (fDirectory != "/" && (!fBaseName.empty() || !fSuffix.empty())) {
			buffer += '/';
		}
	}

	buffer += fBaseName;
	buffer += fSuffix;

	if (!fArchiveMember.empty()) {
		buffer += '(';
		buffer += fArchiveMember;
		buffer += ')';
	}

	return buffer;
}

} // namespace ham::data
