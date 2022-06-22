/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_TEST_RUNNABLE_TEST_HPP
#define HAM_TEST_RUNNABLE_TEST_HPP

#include <memory>
#include <vector>

#include "test/Test.hpp"
#include "test/TestEnvironment.hpp"

namespace ham::test
{

class RunnableTest : public Test
{
  public:
	RunnableTest(const std::string& name, bool isJammable = false);

	bool IsJammable() const { return fIsJammable; }

	int CountTestCases() const { return fTestCaseNames.size(); }
	std::string TestCaseAt(int index, bool fullyQualified = false) const;
	int IndexOfTestCase(const std::string& testCase) const;

	virtual void* CreateFixture(TestEnvironment* environment) = 0;
	virtual void DeleteFixture(TestEnvironment* environment, void* fixture) = 0;
	virtual uint32_t TestCaseCompatibility(int index,
										   bool& _supportedByHam,
										   uint32_t& _skipMask);
	virtual void RunTestCase(TestEnvironment* environment,
							 void* fixture,
							 int index) = 0;

  protected:
	typedef std::vector<std::string> TestCaseNameList;

  protected:
	TestCaseNameList fTestCaseNames;
	bool fIsJammable;
};

template<typename TestFixture>
class GenericRunnableTest : public RunnableTest
{
  public:
	GenericRunnableTest()
		: RunnableTest(TestFixture::TestFixtureName())
	{
		AddTestCaseVisitor visitor(this);
		TestFixture::VisitTestCases(visitor);
	}

	virtual void* CreateFixture(TestEnvironment* /*environment*/)
	{
		return new TestFixture;
	}

	virtual void DeleteFixture(TestEnvironment* /*environment*/, void* fixture)
	{
		delete (TestFixture*)fixture;
	}

	virtual void RunTestCase(TestEnvironment* /*environment*/,
							 void* _fixture,
							 int index)
	{
		TestCaseMethod method = fTestCaseMethods[index];
		TestFixture* fixture = (TestFixture*)_fixture;

		try {
			InitTestCase(fixture, 0);
			(fixture->*method)();
		} catch (...) {
			CleanupTestCase(fixture, 0);
			throw;
		}

		CleanupTestCase(fixture, 0);
	}

  private:
	typedef void (TestFixture::*TestCaseMethod)();
	typedef std::vector<TestCaseMethod> TestCaseMethodList;

	struct AddTestCaseVisitor {
		AddTestCaseVisitor(GenericRunnableTest<TestFixture>* test)
			: fTest(test)
		{
		}

		void Visit(const char* name, TestCaseMethod method)
		{
			fTest->_AddTestCase(name, method);
		}

	  private:
		GenericRunnableTest<TestFixture>* fTest;
	};

  private:
	void _AddTestCase(const std::string& name, void (TestFixture::*method)())
	{
		fTestCaseMethods.push_back(method);
		fTestCaseNames.push_back(name);
	}

	template<typename Type>
	//	static void InitTestCase(Type* fixture, char[sizeof(&
	//Type::InitTestCase)])
	static void InitTestCase(Type* fixture, decltype(&Type::InitTestCase))
	{
		fixture->InitTestCase();
	}

	template<typename Type>
	static void InitTestCase(Type* /*fixture*/, ...)
	{
	}

	template<typename Type>
	static void CleanupTestCase(Type* fixture,
								//		char[sizeof(Type::CleanupTestCase)])
								decltype(&Type::CleanupTestCase))
	{
		fixture->CleanupTestCase();
	}

	template<typename Type>
	static void CleanupTestCase(Type* /*fixture*/, ...)
	{
	}

  private:
	TestCaseMethodList fTestCaseMethods;
};

} // namespace ham::test

#endif // HAM_TEST_RUNNABLE_TEST_HPP
