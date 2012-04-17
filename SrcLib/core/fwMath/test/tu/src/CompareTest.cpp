/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwMath/Compare.hpp>
#include <fwTools/random/Generator.hpp>

#include "CompareTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMath::ut::CompareTest );

namespace fwMath
{
namespace ut
{

void CompareTest::setUp()
{
    // Set up context before running a test.


}
void CompareTest::tearDown()
{
    // Clean up after the test run.
}

void CompareTest::checkEquality()
{
    const double x1 = 0.000001;
    const double x2 = 0.000002;

    bool isEqual = ::fwMath::isEqual(x1, x2);
    CPPUNIT_ASSERT_EQUAL(true, isEqual);

    isEqual = ::fwMath::isEqual(x1, x2, 0.0000001f);
    CPPUNIT_ASSERT_EQUAL(false, isEqual);
}

void CompareTest::checkContainerEquality()
{
    std::vector< double > vect1(20);
    std::vector< double > vect2(20);

    ::fwTools::random::fillContainer(0.0001, 10., vect1);
    vect2 = vect1;

    bool isEqual = ::fwMath::isContainerEqual(vect1, vect2);
    CPPUNIT_ASSERT_EQUAL(true, isEqual);

    ::boost::uint32_t seedVal = std::time(NULL);
    ::fwTools::random::fillContainer(0.000001, 0.000009, vect1, seedVal);
    ::fwTools::random::fillContainer(0.000001, 0.000009, vect2, ++seedVal);

    isEqual = ::fwMath::isContainerEqual(vect1, vect2);
    CPPUNIT_ASSERT_EQUAL(true, isEqual);

    isEqual = ::fwMath::isContainerEqual(vect1, vect2, 0.0000001f);
    CPPUNIT_ASSERT_EQUAL(false, isEqual);
}


} //namespace ut
} //namespace fwMath
