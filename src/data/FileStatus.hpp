/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_FILE_STATUS_HPP
#define HAM_DATA_FILE_STATUS_HPP

#include "data/Time.hpp"

namespace ham
{
namespace data
{

class FileStatus
{
  public:
	enum Type { NONE, FILE, DIRECTORY, SYMLINK, OTHER };

  public:
	FileStatus();
	FileStatus(Type type, const Time& lastModifiedTime);

	bool Exists() const { return fType != NONE; }
	Type GetType() const { return fType; }
	const Time& LastModifiedTime() const { return fLastModifiedTime; }

  private:
	Type fType;
	Time fLastModifiedTime;
};

} // namespace data
} // namespace ham

#endif // HAM_DATA_FILE_STATUS_HPP
