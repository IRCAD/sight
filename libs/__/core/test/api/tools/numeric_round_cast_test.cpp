/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "numeric_round_cast_test.hpp"

#include <core/tools/numeric_round_cast.hxx>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::tools::ut::numeric_round_cast_test);

namespace sight::core::tools::ut
{

//------------------------------------------------------------------------------

void numeric_round_cast_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void numeric_round_cast_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void numeric_round_cast_test::cast_float_to_int_test()
{
    float val       = 1.32F;
    int casted_val1 = core::tools::numeric_round_cast<int>(val);
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(1), casted_val1);

    val         = 1.82F;
    casted_val1 = core::tools::numeric_round_cast<int>(val);
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(2), casted_val1);

    val         = -16.62F;
    casted_val1 = core::tools::numeric_round_cast<int>(val);
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(-17), casted_val1);

    val = 4826.2F;
    const auto casted_val2 = core::tools::numeric_round_cast<std::uint16_t>(val);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint16_t>(4826), casted_val2);
}

//------------------------------------------------------------------------------

void numeric_round_cast_test::cast_double_to_int_test()
{
    double val1      = 895.33;
    auto casted_val1 = core::tools::numeric_round_cast<unsigned int>(val1);
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(895), casted_val1);

    double val2     = -56655.58;
    int casted_val2 = core::tools::numeric_round_cast<int>(val2);
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(-56656), casted_val2);

    double val3      = 88.9005;
    auto casted_val3 = core::tools::numeric_round_cast<std::uint16_t>(val3);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint16_t>(89), casted_val3);
}

//------------------------------------------------------------------------------

void numeric_round_cast_test::cast_int_to_float_test()
{
    int val1         = 55;
    auto casted_val1 = core::tools::numeric_round_cast<float>(val1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(55.0F, casted_val1, 0.00000001);

    int val2         = -4564;
    auto casted_val2 = core::tools::numeric_round_cast<float>(val2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-4564.0F, casted_val2, 0.00000001);

    std::uint16_t val3 = 5394;
    auto casted_val3   = core::tools::numeric_round_cast<double>(val3);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5394.0, casted_val3, 0.00000001);
}

//------------------------------------------------------------------------------

void numeric_round_cast_test::cast_double_to_float_test()
{
    double val      = 55.678;
    auto casted_val = core::tools::numeric_round_cast<float>(val);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(55.678F, casted_val, 0.00000001);

    val        = -4564.851;
    casted_val = core::tools::numeric_round_cast<float>(val);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-4564.851F, casted_val, 0.00000001);

    val        = -84.851 / 3.0;
    casted_val = core::tools::numeric_round_cast<float>(val);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-84.851F / 3.0F, casted_val, 0.00000001);
}

//------------------------------------------------------------------------------

void numeric_round_cast_test::cast_int_to_int_test()
{
    unsigned int val1 = 153;
    auto casted_val1  = core::tools::numeric_round_cast<unsigned char>(val1);
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(153), casted_val1);

    unsigned char val2 = 25;
    auto casted_val2   = core::tools::numeric_round_cast<unsigned char>(val2);
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(25), casted_val2);

    char val3                 = 121;
    std::uint16_t casted_val3 = core::tools::numeric_round_cast<unsigned char>(val3);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint16_t>(121), casted_val3);
}

//------------------------------------------------------------------------------

} // namespace sight::core::tools::ut
