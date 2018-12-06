/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <fwTools/NumericRoundCast.hxx>

#include "NumericRoundCastTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTools::ut::NumericRoundCastTest );

namespace fwTools
{
namespace ut
{

void NumericRoundCastTest::setUp()
{
    // Set up context before running a test.
}

void NumericRoundCastTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void NumericRoundCastTest::castFloatToIntTest()
{
    float val      = 1.32f;
    int castedVal1 = ::fwTools::numericRoundCast<int>(val);
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(1), castedVal1);

    val        = 1.82f;
    castedVal1 = ::fwTools::numericRoundCast<int>(val);
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(2), castedVal1);

    val        = -16.62f;
    castedVal1 = ::fwTools::numericRoundCast<int>(val);
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(-17), castedVal1);

    val = 4826.2f;
    unsigned int castedVal2 = ::fwTools::numericRoundCast<int>(val);
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(4826), castedVal2);
}

//------------------------------------------------------------------------------

void NumericRoundCastTest::castDoubleToIntTest()
{
    double val1             = 895.33;
    unsigned int castedVal1 = ::fwTools::numericRoundCast<unsigned int>(val1);
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(895), castedVal1);

    double val2    = -56655.58;
    int castedVal2 = ::fwTools::numericRoundCast<int>(val2);
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(-56656), castedVal2);

    double val3               = 88.9005;
    unsigned short castedVal3 = ::fwTools::numericRoundCast<unsigned short>(val3);
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned short>(89), castedVal3);

}

//------------------------------------------------------------------------------

void NumericRoundCastTest::castIntToFloatTest()
{
    int val1         = 55;
    float castedVal1 = ::fwTools::numericRoundCast<float>(val1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(55.0f, castedVal1, 0.00000001);

    int val2         = -4564;
    float castedVal2 = ::fwTools::numericRoundCast<float>(val2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-4564.0f, castedVal2, 0.00000001);

    unsigned short val3 = 5394;
    double castedVal3   = ::fwTools::numericRoundCast<double>(val3);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5394.0, castedVal3, 0.00000001);
}

//------------------------------------------------------------------------------

void NumericRoundCastTest::castDoubleToFloatTest()
{
    double val      = 55.678;
    float castedVal = ::fwTools::numericRoundCast<float>(val);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(55.678f, castedVal, 0.00000001);

    val       = -4564.851;
    castedVal = ::fwTools::numericRoundCast<float>(val);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-4564.851f, castedVal, 0.00000001);

    val       = -84.851/3.0;
    castedVal = ::fwTools::numericRoundCast<float>(val);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-84.851f/3.0f, castedVal, 0.00000001);
}

//------------------------------------------------------------------------------

void NumericRoundCastTest::castIntToIntTest()
{
    unsigned int val1        = 153;
    unsigned char castedVal1 = ::fwTools::numericRoundCast<unsigned char>(val1);
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(153), castedVal1);

    unsigned char val2       = 25;
    unsigned char castedVal2 = ::fwTools::numericRoundCast<unsigned char>(val2);
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(25), castedVal2);

    char val3                 = 121;
    unsigned short castedVal3 = ::fwTools::numericRoundCast<unsigned char>(val3);
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned short>(121), castedVal3);
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwTools

