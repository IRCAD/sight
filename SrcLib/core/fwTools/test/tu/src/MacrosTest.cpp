/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>

#include "MacrosTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( MacrosTest );

class ClassTest
{

public:
	fwCoreClassDefinitionsWithFactoryMacro((ClassTest), (()) ( ((int)) ), new ClassTest);

	int getNum() { return m_num; }

protected:
	ClassTest()
	: m_num(1)
	{}

	ClassTest(int num)
	: m_num(num)
	{}

private:
	int m_num;
};

void MacrosTest::setUp()
{
	// Set up context before running a test.
}

void MacrosTest::tearDown()
{
	// Clean up after the test run.
}

void MacrosTest::methode1()
{
	const int NUM = 15;

	ClassTest::sptr classTest1;
	ClassTest::sptr classTest2 = ClassTest::New();
	ClassTest::sptr classTest3 = ClassTest::New(NUM);
	ClassTest::NewSptr classTest4;

	CPPUNIT_ASSERT(classTest1 == NULL);
	CPPUNIT_ASSERT_EQUAL(classTest2->getNum(), 1);
	CPPUNIT_ASSERT_EQUAL(classTest3->getNum(), NUM);
	CPPUNIT_ASSERT_EQUAL(classTest4->getNum(), 1);
}
