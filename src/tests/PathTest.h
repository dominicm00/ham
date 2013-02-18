/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TESTS_PATH_TEST_H
#define HAM_TESTS_PATH_TEST_H


#include "test/TestFixture.h"


namespace ham {
namespace tests {


class PathTest : public test::TestFixture {
public:
			void				IsAbsolute();
			void				RemoveGrist();
			void				Make();
			void				Exists();
			void				PartsConstructor();
			void				PartsSetters();
			void				PartsPath();
			void				PartsIsAbsolute();

	// declare tests
	HAM_ADD_TEST_CASES(PathTest, 8,
		IsAbsolute, RemoveGrist, Make, Exists, PartsConstructor, PartsSetters,
		PartsPath, PartsIsAbsolute)
};


} // namespace tests
} // namespace ham


#endif // HAM_TESTS_PATH_TEST_H
