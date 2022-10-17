/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "NumericRoundCastTest.hpp"

#include <core/tools/NumericRoundCast.hxx>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::tools::ut::NumericRoundCastTest);

namespace sight::core::tools::ut
{

//------------------------------------------------------------------------------

void NumericRoundCastTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void NumericRoundCastTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void NumericRoundCastTest::castFloatToIntTest()
{
    float val      = 1.32F;
    int castedVal1 = core::tools::numericRoundCast<int>(val);
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(1), castedVal1);

    val        = 1.82F;
    castedVal1 = core::tools::numericRoundCast<int>(val);
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(2), castedVal1);

    val        = -16.62F;
    castedVal1 = core::tools::numericRoundCast<int>(val);
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(-17), castedVal1);

    val = 4826.2F;
    const auto castedVal2 = core::tools::numericRoundCast<std::uint16_t>(val);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint16_t>(4826), castedVal2);
}

//------------------------------------------------------------------------------

void NumericRoundCastTest::castDoubleToIntTest()
{
    double val1     = 895.33;
    auto castedVal1 = core::tools::numericRoundCast<unsigned int>(val1);
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(895), castedVal1);

    double val2    = -56655.58;
    int castedVal2 = core::tools::numericRoundCast<int>(val2);
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(-56656), castedVal2);

    double val3     = 88.9005;
    auto castedVal3 = core::tools::numericRoundCast<std::uint16_t>(val3);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint16_t>(89), castedVal3);
}

//------------------------------------------------------------------------------

void NumericRoundCastTest::castIntToFloatTest()
{
    int val1        = 55;
    auto castedVal1 = core::tools::numericRoundCast<float>(val1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(55.0F, castedVal1, 0.00000001);

    int val2        = -4564;
    auto castedVal2 = core::tools::numericRoundCast<float>(val2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-4564.0F, castedVal2, 0.00000001);

    std::uint16_t val3 = 5394;
    auto castedVal3    = core::tools::numericRoundCast<double>(val3);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5394.0, castedVal3, 0.00000001);
}

//------------------------------------------------------------------------------

void NumericRoundCastTest::castDoubleToFloatTest()
{
    double val     = 55.678;
    auto castedVal = core::tools::numericRoundCast<float>(val);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(55.678F, castedVal, 0.00000001);

    val       = -4564.851;
    castedVal = core::tools::numericRoundCast<float>(val);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-4564.851F, castedVal, 0.00000001);

    val       = -84.851 / 3.0;
    castedVal = core::tools::numericRoundCast<float>(val);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-84.851F / 3.0F, castedVal, 0.00000001);
}

//------------------------------------------------------------------------------

void NumericRoundCastTest::castIntToIntTest()
{
    unsigned int val1 = 153;
    auto castedVal1   = core::tools::numericRoundCast<unsigned char>(val1);
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(153), castedVal1);

    unsigned char val2 = 25;
    auto castedVal2    = core::tools::numericRoundCast<unsigned char>(val2);
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(25), castedVal2);

    char val3                = 121;
    std::uint16_t castedVal3 = core::tools::numericRoundCast<unsigned char>(val3);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint16_t>(121), castedVal3);
}

//------------------------------------------------------------------------------

} // namespace sight::core::tools::ut
