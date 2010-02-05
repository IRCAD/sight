/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/Stringizer.hpp>

#include "StringizerTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( StringizerTest );

void StringizerTest::setUp()
{
    // Set up context before running a test.
}

void StringizerTest::tearDown()
{
    // Clean up after the test run.
}

void StringizerTest::methode1()
{
    const std::string STR1 = "[1,2,3]" ;
    const std::string STR2 = "6";
    const double D = 6.0;
    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    std::string res1 = ::fwTools::getString(vec.begin(), vec.end());
    std::string res2 = ::fwTools::getString<double>(D);


    // check
    CPPUNIT_ASSERT_EQUAL(res1, STR1);
    CPPUNIT_ASSERT_EQUAL(res2, STR2);
}
