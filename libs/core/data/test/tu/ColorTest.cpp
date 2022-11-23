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

#include "ColorTest.hpp"

#include <data/Color.hpp>

#include <exception>
#include <iostream>
#include <map>
#include <ostream>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::ColorTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void ColorTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ColorTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ColorTest::methode1()
{
    //-----------test values
    const float R = 0.2F;
    const float G = 0.8F;
    const float B = 0.5F;
    const float A = 0.6F;

    auto color = data::Color::New(R, G, B, A);

    CPPUNIT_ASSERT_EQUAL(color->red(), R);
    CPPUNIT_ASSERT_EQUAL(color->green(), G);
    CPPUNIT_ASSERT_EQUAL(color->blue(), B);
    CPPUNIT_ASSERT_EQUAL(color->alpha(), A);

    auto color2 = data::Color::New(R, G, B, A);
    CPPUNIT_ASSERT(*color == *color2);
}

//------------------------------------------------------------------------------

void ColorTest::methode2()
{
    //-----------test values
    const float R = 0.2F;
    const float G = 0.8F;
    const float B = 0.5F;
    const float A = 0.6F;

    auto color = data::Color::New();

    data::Color::ColorArray array;
    array[0] = R;
    array[1] = G;
    array[2] = B;
    array[3] = A;

    color->setRGBA(array);

    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[0], R);
    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[1], G);
    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[2], B);
    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[3], A);

    auto color2 = data::Color::New();
    CPPUNIT_ASSERT(*color != *color2);

    color2->setRGBA(array);
    CPPUNIT_ASSERT(*color == *color2);
}

//------------------------------------------------------------------------------

void ColorTest::methode3()
{
    // fuchsia string value
    const std::string fuchsia = "#FF006E";
    // fuchsia float values
    const float R = 1.F;
    const float G = 0.F;
    const float B = 110.F / 255.F;
    const float A = 1.F;

    data::Color::sptr color = data::Color::New();

    color->setRGBA(fuchsia);

    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[0], R);
    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[1], G);
    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[2], B);
    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[3], A);

    auto color2 = data::Color::New();
    CPPUNIT_ASSERT(*color != *color2);

    color2->setRGBA(fuchsia);
    CPPUNIT_ASSERT(*color == *color2);
}

//------------------------------------------------------------------------------

void ColorTest::equalityTest()
{
    auto color1 = data::Color::New();
    auto color2 = data::Color::New();

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
