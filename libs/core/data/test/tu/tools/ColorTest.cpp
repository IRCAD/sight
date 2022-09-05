/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2015 IHU Strasbourg
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

#include <core/Exception.hpp>

#include <data/tools/Color.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::tools::ut::ColorTest);

namespace sight::data::tools::ut
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

void ColorTest::hexaStringToRGBA()
{
    const std::string strColor1 = "#ff0527";
    std::array<std::uint8_t, 4> color1 {};
    CPPUNIT_ASSERT_NO_THROW(data::tools::Color::hexaStringToRGBA(strColor1, color1));

    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(255), color1[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(5), color1[1]);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(39), color1[2]);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(255), color1[3]);

    const std::string strColor2 = "#45a5bc28";
    std::array<std::uint8_t, 4> color2 {};
    CPPUNIT_ASSERT_NO_THROW(data::tools::Color::hexaStringToRGBA(strColor2, color2));

    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(69), color2[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(165), color2[1]);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(188), color2[2]);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(40), color2[3]);

    const std::string strColor3 = "45a5bc28";
    std::array<std::uint8_t, 4> color3 {};
    CPPUNIT_ASSERT_THROW(data::tools::Color::hexaStringToRGBA(strColor3, color3), core::Exception);

    const std::string strColor4 = "45a58";
    CPPUNIT_ASSERT_THROW(data::tools::Color::hexaStringToRGBA(strColor4, color3), core::Exception);

    const std::string strColor5 = "#45a564928";
    CPPUNIT_ASSERT_THROW(data::tools::Color::hexaStringToRGBA(strColor5, color3), core::Exception);
}

} // namespace sight::data::tools::ut
