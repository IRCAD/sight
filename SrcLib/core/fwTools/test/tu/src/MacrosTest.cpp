/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "MacrosTest.hpp"

#include <fwTools/Object.hpp>

#include <fwCore/base.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTools::ut::MacrosTest );

namespace fwTools
{
namespace ut
{

class ClassTest
{

public:
    fwCoreClassFactoryMacro((ClassTest), (())( ((int)) ), new ClassTest);

    //------------------------------------------------------------------------------

    int getNum()
    {
        return m_num;
    }

    ClassTest() :
        m_num(1)
    {
    }

    ClassTest(int num) :
        m_num(num)
    {
    }

    virtual ~ClassTest()
    {
    }

private:
    int m_num;
};

//------------------------------------------------------------------------------

void MacrosTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void MacrosTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void MacrosTest::factoryMacroTest()
{
    const int NUM = 15;

    ClassTest::sptr classTest1;
    ClassTest::sptr classTest2 = ClassTest::New();
    ClassTest::sptr classTest3 = ClassTest::New(NUM);
    ClassTest::sptr classTest4 = ClassTest::New();

    CPPUNIT_ASSERT(classTest1 == NULL);
    CPPUNIT_ASSERT_EQUAL(classTest2->getNum(), 1);
    CPPUNIT_ASSERT_EQUAL(classTest3->getNum(), NUM);
    CPPUNIT_ASSERT_EQUAL(classTest4->getNum(), 1);
}

} // namespace ut
} // namespace fwTools
