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

#include "color_test.hpp"

#include <data/color.hpp>

#include <exception>
#include <iostream>
#include <map>
#include <ostream>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::color_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void color_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void color_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void color_test::methode1()
{
    //-----------test values
    const float r = 0.2F;
    const float g = 0.8F;
    const float b = 0.5F;
    const float a = 0.6F;

    auto color = std::make_shared<data::color>(r, g, b, a);

    CPPUNIT_ASSERT_EQUAL(color->red(), r);
    CPPUNIT_ASSERT_EQUAL(color->green(), g);
    CPPUNIT_ASSERT_EQUAL(color->blue(), b);
    CPPUNIT_ASSERT_EQUAL(color->alpha(), a);

    auto color2 = std::make_shared<data::color>(r, g, b, a);
    CPPUNIT_ASSERT(*color == *color2);
}

//------------------------------------------------------------------------------

void color_test::methode2()
{
    //-----------test values
    const float r = 0.2F;
    const float g = 0.8F;
    const float b = 0.5F;
    const float a = 0.6F;

    auto color = std::make_shared<data::color>();

    data::color::ColorArray array;
    array[0] = r;
    array[1] = g;
    array[2] = b;
    array[3] = a;

    color->setRGBA(array);

    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[0], r);
    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[1], g);
    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[2], b);
    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[3], a);

    auto color2 = std::make_shared<data::color>();
    CPPUNIT_ASSERT(*color != *color2);

    color2->setRGBA(array);
    CPPUNIT_ASSERT(*color == *color2);
}

//------------------------------------------------------------------------------

void color_test::methode3()
{
    // fuchsia string value
    const std::string fuchsia = "#FF006E";
    // fuchsia float values
    const float r = 1.F;
    const float g = 0.F;
    const float b = 110.F / 255.F;
    const float a = 1.F;

    data::color::sptr color = std::make_shared<data::color>();

    color->setRGBA(fuchsia);

    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[0], r);
    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[1], g);
    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[2], b);
    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[3], a);

    auto color2 = std::make_shared<data::color>();
    CPPUNIT_ASSERT(*color != *color2);

    color2->setRGBA(fuchsia);
    CPPUNIT_ASSERT(*color == *color2);
}

//------------------------------------------------------------------------------

void color_test::equalityTest()
{
    auto color1 = std::make_shared<data::color>();
    auto color2 = std::make_shared<data::color>();

    CPPUNIT_ASSERT(*color1 == *color2 && !(*color1 != *color2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(...) \
    color1->setRGBA(__VA_ARGS__); \
    CPPUNIT_ASSERT_MESSAGE( \
        "The colors should be different when the first is set with " #__VA_ARGS__, \
        *color1 != *color2 && !(*color1 == *color2) \
    ); \
    color2->setRGBA(color1->getRGBA()); \
    CPPUNIT_ASSERT_MESSAGE( \
        "The colors should be equal when both are set with " #__VA_ARGS__, \
        *color1 == *color2 && !(*color1 != *color2) \
    );

    TEST(1, 0, 0, 0);
    TEST(0, 1, 0, 0);
    TEST(0, 0, 1, 0);
    TEST(0, 0, 0, 1);

    #undef TEST
}

} // namespace sight::data::ut
