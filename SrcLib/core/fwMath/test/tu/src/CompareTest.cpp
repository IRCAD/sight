/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "CompareTest.hpp"

#include <fwMath/Compare.hpp>

#include <fwTools/random/Generator.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMath::ut::CompareTest );

namespace fwMath
{
namespace ut
{

//------------------------------------------------------------------------------

void CompareTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void CompareTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void CompareTest::checkEquality()
{
    const double x1 = 0.000001;
    const double x2 = 0.000002;

    bool isEqual = ::fwMath::isEqual(x1, x2);
    CPPUNIT_ASSERT_EQUAL(true, isEqual);

    isEqual = ::fwMath::isEqual(x1, x2, 0.0000001f);
    CPPUNIT_ASSERT_EQUAL(false, isEqual);
}

//------------------------------------------------------------------------------

void CompareTest::checkContainerEquality()
{
    std::vector< double > vect1(20);
    std::vector< double > vect2(20);

    ::fwTools::random::fillContainer(0.0001, 10., vect1);
    vect2 = vect1;

    bool isEqual = ::fwMath::isContainerEqual(vect1, vect2);
    CPPUNIT_ASSERT_EQUAL(true, isEqual);

    std::uint32_t seedVal = std::time(NULL);
    ::fwTools::random::fillContainer(0.000001, 0.000009, vect1, seedVal);
    ::fwTools::random::fillContainer(0.000001, 0.000009, vect2, ++seedVal);

    isEqual = ::fwMath::isContainerEqual(vect1, vect2);
    CPPUNIT_ASSERT_EQUAL(true, isEqual);

    isEqual = ::fwMath::isContainerEqual(vect1, vect2, 0.0000001f);
    CPPUNIT_ASSERT_EQUAL(false, isEqual);
}

} //namespace ut
} //namespace fwMath
