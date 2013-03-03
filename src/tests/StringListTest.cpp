/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "tests/StringListTest.h"

#include <numeric>
#include <string>
#include <vector>

#include "data/StringList.h"


namespace ham {
namespace tests {


using data::String;
using data::StringList;


static int
sign(int number)
{
	return number == 0 ? 0 : (number < 0 ? - 1 : 1);
}


typedef std::vector<std::string> TestList;
typedef std::vector<TestList> TestListList;


static bool
operator==(const StringList& list, const TestList& testList)
{
	size_t size = list.Size();
	if (size != testList.size())
		return false;

	for (size_t i = 0; i < size; i++) {
		String string = list.ElementAt(i);
		std::string testString = testList[i];
		if (testString != string.ToCString())
			return false;
	}

	return true;
}


static bool
operator!=(const StringList& list, const TestList& testList)
{
	return !(list == testList);
}


static TestList
operator+(const TestList& testList, const char* string)
{
	TestList result(testList);
	result.push_back(string);
	return result;
}


static TestListList
operator+(const TestListList& testListList, const TestList& testList)
{
	TestListList result(testListList);
	result.push_back(testList);
	return result;
}


#define LIST_EQUAL(actual, expected)							\
	HAM_TEST_EQUAL(actual.Size(), (expected).size())			\
	HAM_TEST_EQUAL(actual, (expected))							\
	HAM_TEST_EQUAL(actual.IsEmpty(), (expected).empty())

#define LIST_COMPARE_WORK(list1, list2, expected,				\
		expectedIgnoreTrailingEmpty)							\
	HAM_TEST_EQUAL(sign(list1.CompareWith(list2)), expected)	\
	HAM_TEST_EQUAL(sign(list1.CompareWith(list2, true)),		\
		expectedIgnoreTrailingEmpty)							\
	HAM_TEST_EQUAL(list1 == list2, expected == 0)				\
	HAM_TEST_EQUAL(list1 != list2, expected != 0)				\
	HAM_TEST_EQUAL(list1 < list2, expected < 0)					\
	HAM_TEST_EQUAL(list1 > list2, expected > 0)					\
	HAM_TEST_EQUAL(list1 <= list2, expected <= 0)				\
	HAM_TEST_EQUAL(list1 >= list2, expected >= 0)

#define LIST_COMPARE(list1, list2, expected, expectedIgnoreTrailingEmpty)	\
	LIST_COMPARE_WORK(list1, list2, expected, expectedIgnoreTrailingEmpty)	\
	LIST_COMPARE_WORK(list2, list1, -expected, -expectedIgnoreTrailingEmpty)


void
ham::tests::StringListTest::Constructor()
{
	// default constructor
	{
		StringList list;
		LIST_EQUAL(list, TestList())
	}

	// (size_t) constructor
	{
		for (size_t size = 0; size < 100; size++) {
			StringList list(size);
			LIST_EQUAL(list, TestList(size, std::string()))
		}
	}

	// (const String&) constructor
	{
		StringList list(String(""));
		LIST_EQUAL(list, TestList() + "")
	}

	{
		StringList list(String("foo"));
		LIST_EQUAL(list, TestList() + "foo")
	}

	// (const StringList*) constructor
	{
		StringList list2((const StringList*)NULL);
		LIST_EQUAL(list2, TestList())
	}

	{
		StringList list1;
		LIST_EQUAL(list1, TestList())
		StringList list2(&list1);
		LIST_EQUAL(list2, TestList())
	}

	{
		StringList list1("foo");
		LIST_EQUAL(list1, TestList() + "foo")
		StringList list2(&list1);
		LIST_EQUAL(list2, TestList() + "foo")
	}

	{
		StringList list1 = MakeStringList("foo", "bar", "foobar");
		LIST_EQUAL(list1, TestList() + "foo" + "bar" + "foobar")
		StringList list2(&list1);
		LIST_EQUAL(list2, TestList() + "foo" + "bar" + "foobar")
	}

	// copy constructor
	{
		StringList list1;
		LIST_EQUAL(list1, TestList())
		StringList list2(list1);
		LIST_EQUAL(list2, TestList())
	}

	{
		StringList list1("foo");
		LIST_EQUAL(list1, TestList() + "foo")
		StringList list2(list1);
		LIST_EQUAL(list2, TestList() + "foo")
	}

	{
		StringList list1 = MakeStringList("foo", "bar", "foobar");
		LIST_EQUAL(list1, TestList() + "foo" + "bar" + "foobar")
		StringList list2(list1);
		LIST_EQUAL(list2, TestList() + "foo" + "bar" + "foobar")
	}

	// (const StringList&, size_t, size_t) constructor tested in SubList()
}


void
StringListTest::ElementAccess()
{
	for (size_t size = 0; size < 100; size++) {
		StringList list;
		TestList testList;
		for (size_t i = 0; i < size; i++) {
			char c = 'a' + (i % 26);
			size_t length = (5 + i) % 7;
			std::string element(length, c);
			testList.push_back(element);
			list.Append(String(element.c_str()));
		}

		// test ElementAt(), also for a range beyond the end of the list
		for (size_t i = 0; i < 2 * size + 10; i++) {
			HAM_TEST_EQUAL(list.ElementAt(i).ToCString(),
				i < size ? testList[i] : std::string())
		}

		// test Head()
		HAM_TEST_EQUAL(list.Head().ToCString(),
			size > 0 ? testList[0] : std::string())

		// test SetElementAt(), also for a range beyond the end of the list
		for (size_t i = 0; i < 2 * size + 10; i++) {
			std::string testString = ValueToString(i);
			if (i < size)
				testList[i] = testString;
			list.SetElementAt(i, String(testString.c_str()));

			LIST_EQUAL(list, testList);
		}
	}
}


void
StringListTest::IsTrue()
{
	HAM_TEST_VERIFY(!StringList().IsTrue());
	HAM_TEST_VERIFY(!MakeStringList("").IsTrue());
	HAM_TEST_VERIFY(!MakeStringList("", "").IsTrue());
	HAM_TEST_VERIFY(!MakeStringList("", "", "").IsTrue());
	HAM_TEST_VERIFY(MakeStringList("foo").IsTrue());
	HAM_TEST_VERIFY(MakeStringList("", "foo").IsTrue());
	HAM_TEST_VERIFY(MakeStringList("", "", "foo").IsTrue());
	HAM_TEST_VERIFY(MakeStringList("foo", "").IsTrue());
	HAM_TEST_VERIFY(MakeStringList("foo", "", "").IsTrue());
	HAM_TEST_VERIFY(MakeStringList("foo", "bar").IsTrue());
	HAM_TEST_VERIFY(MakeStringList("", "foo", "bar").IsTrue());
	HAM_TEST_VERIFY(MakeStringList("", "", "foo", "bar").IsTrue());
}


void
StringListTest::SubList()
{
	for (size_t size = 0; size < 100; size++) {
		StringList list;
		TestList testList;
		for (size_t i = 0; i < size; i++) {
			char c = 'a' + (i % 26);
			size_t length = (5 + i) % 7;
			std::string element(length, c);
			testList.push_back(element);
			list.Append(String(element.c_str()));
		}

		for (size_t start = 0; start < size + 10; start++) {
			for (size_t end = 0; end < size + 10; end++) {
				TestList testSubList;
				if (start < size && start < end) {
					size_t actualEnd = std::min(end, size);
					testSubList.resize(actualEnd - start);
					std::copy(testList.begin() + start,
						testList.begin() + actualEnd, testSubList.begin());
				}

				StringList subList1 = list.SubList(start, end);
				LIST_EQUAL(subList1, testSubList);

				StringList subList2(list, start, end);
				LIST_EQUAL(subList2, testSubList);
			}
		}
	}
}


void
StringListTest::Contains()
{
	{
		StringList list;
		HAM_TEST_VERIFY(!list.Contains(String("")));
		HAM_TEST_VERIFY(!list.Contains(String("foo")));
	}

	{
		StringList list = MakeStringList("");
		HAM_TEST_VERIFY(list.Contains(String("")));
		HAM_TEST_VERIFY(!list.Contains(String("foo")));
	}

	{
		StringList list = MakeStringList("foo");
		HAM_TEST_VERIFY(!list.Contains(String("")));
		HAM_TEST_VERIFY(list.Contains(String("foo")));
		HAM_TEST_VERIFY(!list.Contains(String("bar")));
		HAM_TEST_VERIFY(!list.Contains(String("foobar")));
	}

	{
		StringList list = MakeStringList("foo", "bar");
		HAM_TEST_VERIFY(!list.Contains(String("")));
		HAM_TEST_VERIFY(list.Contains(String("foo")));
		HAM_TEST_VERIFY(list.Contains(String("bar")));
		HAM_TEST_VERIFY(!list.Contains(String("foobar")));
	}

	{
		StringList list = MakeStringList("foo", "bar", "foobar");
		HAM_TEST_VERIFY(!list.Contains(String("")));
		HAM_TEST_VERIFY(list.Contains(String("foo")));
		HAM_TEST_VERIFY(list.Contains(String("bar")));
		HAM_TEST_VERIFY(list.Contains(String("foobar")));
		HAM_TEST_VERIFY(!list.Contains(String("barfoo")));
	}

	{
		StringList list = MakeStringList("foo", "", "foo", "bar", "foobar");
		HAM_TEST_VERIFY(list.Contains(String("")));
		HAM_TEST_VERIFY(list.Contains(String("foo")));
		HAM_TEST_VERIFY(list.Contains(String("bar")));
		HAM_TEST_VERIFY(list.Contains(String("foobar")));
		HAM_TEST_VERIFY(!list.Contains(String("barfoo")));
	}
}


void
StringListTest::Constants()
{
	TestList testFalseList;
	TestList testTrueList = TestList() + "1";
	StringList falseList = StringList::False();
	StringList trueList = StringList::True();
	LIST_EQUAL(falseList, testFalseList)
	LIST_EQUAL(trueList, testTrueList)

	falseList.Append("foo");
	trueList.Append("bar");
	LIST_EQUAL(falseList, testFalseList + "foo")
	LIST_EQUAL(trueList, testTrueList + "bar")

	LIST_EQUAL(StringList::False(), testFalseList)
	LIST_EQUAL(StringList::True(), testTrueList)
}


void
StringListTest::Comparison()
{
	struct TestData {
		StringList	list1;
		StringList	list2;
		int			compare;
		int			compareIgnoreTrailingEmpty;
	};

	const TestData testData[] = {
		{ StringList(),					StringList(),					0, 0 },
		{ MakeStringList(""),			StringList(),					1, 0 },
		{ MakeStringList(""),			MakeStringList(""),				0, 0 },
		{ MakeStringList("foo"),		StringList(),					1, 1 },
		{ MakeStringList("foo"),		MakeStringList(""),				1, 1 },
		{ MakeStringList("foo"),		MakeStringList("foo"),			0, 0 },
		{ MakeStringList("foo", ""),	MakeStringList("foo"),			1, 0 },
		{ MakeStringList("foo", "", ""), MakeStringList("foo"),			1, 0 },
		{ MakeStringList("foobar"),		MakeStringList("foo"),			1, 1 },
		{ MakeStringList("foobar"),		MakeStringList("foo", "bar"),	1, 1 },
		{ MakeStringList("foo"),		MakeStringList("bar"),			1, 1 },
		{ MakeStringList("foo", "bar"),	MakeStringList("foo", "bar"),	0, 0 },
		{ MakeStringList("foo", "bar"),	MakeStringList("foo"),			1, 1 },
		{ MakeStringList("foo", "x"),	MakeStringList("foo", "bar"),	1, 1 }
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		HAM_TEST_ADD_INFO(
			LIST_COMPARE(testData[i].list1, testData[i].list2,
				testData[i].compare, testData[i].compareIgnoreTrailingEmpty),
			"list1: %s\nlist2: %s",
			ValueToString(testData[i].list1).c_str(),
			ValueToString(testData[i].list2).c_str()
		)
	}
}


void
StringListTest::Assignment()
{
	struct TestData {
		TestList	list1;
		TestList	list2;
	};

	const TestData testData[] = {
		{ TestList(),			TestList()		},
		{ TestList(),			TestList() + "" },
		{ TestList() + "",		TestList() + "" },
		{ TestList() + "foo",	TestList() },
		{ TestList() + "foo",	TestList() + "" },
		{ TestList() + "foo",	TestList() + "bar" },
		{ TestList() + "foo",	TestList() + "bar" + "foobar" }
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		const TestList* testList1 = &testData[i].list1;
		const TestList* testList2 = &testData[i].list2;
		for (int k = 0; k < 2; k++) {
			StringList list1 = MakeStringList(*testList1);
			StringList list2 = MakeStringList(*testList2);
			LIST_EQUAL(list1, *testList1)
			LIST_EQUAL(list2, *testList2)

			list1 = list2;
			LIST_EQUAL(list1, *testList2)
			LIST_EQUAL(list2, *testList2)

			std::swap(testList1, testList2);
		}
	}
}


void
StringListTest::Concatenation()
{
	// We use the String version of Append() all the time with all kinds of
	// parameter, so there's no real need to test it here. We only test the
	// StringList version.

	struct TestData {
		TestList	list1;
		TestList	list2;
	};

	const TestData testData[] = {
		{ TestList(),			TestList()		},
		{ TestList(),			TestList() + "" },
		{ TestList() + "",		TestList() + "" },
		{ TestList() + "foo",	TestList() },
		{ TestList() + "foo",	TestList() + "" },
		{ TestList() + "foo",	TestList() + "bar" },
		{ TestList() + "foo",	TestList() + "bar" + "foobar" }
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		const TestList* testList1 = &testData[i].list1;
		const TestList* testList2 = &testData[i].list2;
		for (int k = 0; k < 2; k++) {
			StringList list1 = MakeStringList(*testList1);
			StringList list2 = MakeStringList(*testList2);
			LIST_EQUAL(list1, *testList1)
			LIST_EQUAL(list2, *testList2)

			list1.Append(list2);
			TestList resultTestList(*testList1);
			resultTestList.resize(testList1->size() + testList2->size());
			std::copy(testList2->begin(), testList2->end(),
				resultTestList.begin() + testList1->size());
			LIST_EQUAL(list1, resultTestList)
			LIST_EQUAL(list2, *testList2)

			std::swap(testList1, testList2);
		}
	}

	// special case: append list to itself
	{
		StringList list;
		list.Append(list);
		LIST_EQUAL(list, TestList())
	}

	{
		StringList list = MakeStringList("a");
		list.Append(list);
		LIST_EQUAL(list, TestList() + "a" + "a")
	}

	{
		StringList list = MakeStringList("a", "b");
		list.Append(list);
		LIST_EQUAL(list, TestList() + "a" + "b" + "a" + "b")
	}

	{
		StringList list = MakeStringList("a", "b", "c");
		list.Append(list);
		LIST_EQUAL(list, TestList() + "a" + "b" + "c" + "a" + "b" + "c")
	}

	{
		StringList list = MakeStringList("a", "b", "c", "d", "e");
		list.Append(list);
		LIST_EQUAL(list,
			TestList() + "a" + "b" + "c" + "d" + "e" + "a" + "b" + "c" + "d"
				+ "e")
	}

	// append sublists with the same buffer
	{
		StringList list = MakeStringList("a", "b", "c", "d", "e");
		StringList subList1 = list.SubList(0, 2);
		StringList subList2 = list.SubList(1, 3);
		subList1.Append(subList2);
		LIST_EQUAL(subList1, TestList() + "a" + "b" + "b" + "c")
	}
}


void
StringListTest::Clear()
{
	{
		StringList list;
		LIST_EQUAL(list, TestList())
		list.Clear();
		LIST_EQUAL(list, TestList())
	}

	{
		StringList list = MakeStringList("foo");
		LIST_EQUAL(list, TestList() + "foo")
		list.Clear();
		LIST_EQUAL(list, TestList())
	}

	{
		StringList list = MakeStringList("foo", "bar");
		LIST_EQUAL(list, TestList() + "foo" + "bar")
		list.Clear();
		LIST_EQUAL(list, TestList())
	}
}


void
StringListTest::Join()
{
	const TestList testData[] = {
		TestList(),
		TestList() + "",
		TestList() + "foo",
		TestList() + "foo" + "",
		TestList() + "" + "foo",
		TestList() + "foo" + "bar",
		TestList() + "foo" + "" + "bar",
		TestList() + "foo" + "bar" + "",
		TestList() + "" + "foo" + "" + "" + "bar" + "",
		TestList() + "foo" + "bar" + "foobar"
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		const TestList& testList = testData[i];
		StringList list = MakeStringList(testList);
		LIST_EQUAL(list, testList);

		std::string testString
			= std::accumulate(testList.begin(), testList.end(), std::string());
		String string = list.Join();
		HAM_TEST_EQUAL(string.ToCString(), testString)
	}
}


void
StringListTest::JoinWithSeparator()
{
	struct TestData {
		TestList	listList;
		const char*	separator;
		std::string	result;
	};

	const TestData testData[] = {
		{ TestList(),
			"", "" },
		{ TestList() + "",
			"", "" },
		{ TestList() + "foo",
			"", "foo" },
		{ TestList() + "foo" + "",
			"", "foo" },
		{ TestList() + "" + "foo",
			"", "foo" },
		{ TestList() + "foo" + "bar",
			"", "foobar" },
		{ TestList() + "foo" + "" + "bar",
			"", "foobar" },
		{ TestList() + "foo" + "bar" + "",
			"", "foobar" },
		{ TestList() + "" + "foo" + "" + "" + "bar" + "",
			"", "foobar" },
		{ TestList() + "foo" + "bar" + "foobar",
			"", "foobarfoobar" },
		{ TestList(),
			"X", "" },
		{ TestList() + "",
			"X", "" },
		{ TestList() + "foo",
			"X", "foo" },
		{ TestList() + "foo" + "",
			"X", "fooX" },
		{ TestList() + "" + "foo",
			"X", "Xfoo" },
		{ TestList() + "foo" + "bar",
			"X", "fooXbar" },
		{ TestList() + "foo" + "" + "bar",
			"X", "fooXXbar" },
		{ TestList() + "foo" + "bar" + "",
			"X", "fooXbarX" },
		{ TestList() + "" + "foo" + "" + "" + "bar" + "",
			"X", "XfooXXXbarX" },
		{ TestList() + "foo" + "bar" + "foobar",
			"X", "fooXbarXfoobar" },
		{ TestList(),
			"XYZ", "" },
		{ TestList() + "",
			"XYZ", "" },
		{ TestList() + "foo",
			"XYZ", "foo" },
		{ TestList() + "foo" + "",
			"XYZ", "fooXYZ" },
		{ TestList() + "" + "foo",
			"XYZ", "XYZfoo" },
		{ TestList() + "foo" + "bar",
			"XYZ", "fooXYZbar" },
		{ TestList() + "foo" + "" + "bar",
			"XYZ", "fooXYZXYZbar" },
		{ TestList() + "foo" + "bar" + "",
			"XYZ", "fooXYZbarXYZ" },
		{ TestList() + "" + "foo" + "" + "" + "bar" + "",
			"XYZ", "XYZfooXYZXYZXYZbarXYZ" },
		{ TestList() + "foo" + "bar" + "foobar",
			"XYZ", "fooXYZbarXYZfoobar" },
	};

	const char* const testDataContext[] = {
		"",
		"a",
		"abc",
		"xyz"
	};
	const size_t testDataContextCount
		= sizeof(testDataContext) / sizeof(testDataContext[0]);

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		const TestList& testList = testData[i].listList;
		StringList list = MakeStringList(testList);
		LIST_EQUAL(list, testList);

		for (size_t prefix = 0; prefix < testDataContextCount; prefix++) {
			for (size_t suffix = 0; suffix < testDataContextCount; suffix++) {
				std::string separatorString = std::string()
					+ testDataContext[prefix] + testData[i].separator
					+ testDataContext[suffix];
				StringPart separator(
					separatorString.c_str() + strlen(testDataContext[prefix]),
					strlen(testData[i].separator));

				String string = list.Join(separator);
				HAM_TEST_EQUAL(string.ToCString(), testData[i].result)
			}
		}
	}
}


void
StringListTest::Multiply()
{
	struct TestData {
		TestListList	listList;
		TestList		result;
	};

	const TestData testData[] = {
		{ TestListList(),
			TestList() },
		{ TestListList() + TestList(),
			TestList() },
		{ TestListList() + TestList() + TestList(),
			TestList() },
		{ TestListList() + (TestList() + ""),
			TestList() + "" },
		{ TestListList() + (TestList() + "") + TestList(),
			TestList() },
		{ TestListList() + TestList() + (TestList() + ""),
			TestList() },
		{ TestListList() + (TestList() + "") + (TestList() + ""),
			TestList() + "" },
		{ TestListList() + (TestList() + "foo"),
			TestList() + "foo" },
		{ TestListList() + (TestList() + "foo") + (TestList() + "bar"),
			TestList() + "foobar" },
		{ TestListList() + (TestList() + "a" + "b") + (TestList() + "bar"),
			TestList() + "abar" + "bbar" },
		{ TestListList() + (TestList() + "a" + "b") + (TestList() + "x" + "y"),
			TestList() + "ax" + "ay" + "bx" + "by" },
		{ TestListList() + (TestList() + "a" + "b" + "c")
				+ (TestList() + "x" + "y"),
			TestList() + "ax" + "ay" + "bx" + "by" + "cx" + "cy" },
		{ TestListList() + (TestList() + "a" + "b" + "c")
				+ (TestList() + "x" + "y") + (TestList() + ""),
			TestList() + "ax" + "ay" + "bx" + "by" + "cx" + "cy" },
		{ TestListList() + (TestList() + "a" + "b" + "c")
				+ (TestList() + "x" + "y") + TestList(),
			TestList() },
		{ TestListList() + (TestList() + "a" + "b" + "c")
				+ (TestList() + "x" + "y") + (TestList() + "1" + "2"),
			TestList() + "ax1" + "ax2" + "ay1" + "ay2" + "bx1" + "bx2" + "by1"
				+ "by2" + "cx1" + "cx2" + "cy1" + "cy2" },
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		const TestListList& testListList = testData[i].listList;
		const TestList& testList = testData[i].result;
		StringListList listList = MakeStringListList(testListList);
		StringList list = StringList::Multiply(listList);
		HAM_TEST_ADD_INFO(
			LIST_EQUAL(list, testList),
			"listList: %s\nlist: %s",
			ValueToString(listList).c_str(),
			ValueToString(list).c_str()
		)
	}
}


void
StringListTest::Iteration()
{
	const TestList testData[] = {
		TestList(),
		TestList() + "",
		TestList() + "foo",
		TestList() + "foo" + "bar",
		TestList() + "foo" + "bar" + "foobar"
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		const TestList& testList = testData[i];
		StringList list = MakeStringList(testList);
		LIST_EQUAL(list, testList);

		size_t size = testList.size();
		StringList::Iterator it = list.GetIterator();
		for (size_t k = 0; k < size; k++) {
			HAM_TEST_VERIFY(it.HasNext())
			String element = it.Next();
			HAM_TEST_EQUAL(element.ToCString(), testList[k])
		}

		HAM_TEST_VERIFY(!it.HasNext())
		String element = it.Next();
		HAM_TEST_EQUAL(element.ToCString(), std::string())
	}
}


} // namespace tests
} // namespace ham
