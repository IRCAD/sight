/************************************************************************
 *
 * Copyright (C) 2014-2015 IRCAD France
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

#include <fwCore/Exception.hpp>
#include <fwDataTools/Color.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataTools::ut::ColorTest );

namespace fwDataTools
{
namespace ut
{

void ColorTest::setUp()
{
    // Set up context before running a test.

}
void ColorTest::tearDown()
{
    // Clean up after the test run.
}

void ColorTest::hexaStringToRGBA()
{
    const std::string strColor1 = "#ff0527";
    std::uint8_t color1[4];
    CPPUNIT_ASSERT_NO_THROW(::fwDataTools::Color::hexaStringToRGBA(strColor1, color1));

    CPPUNIT_ASSERT_EQUAL(static_cast< std::uint8_t >(255), color1[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast< std::uint8_t >(5),   color1[1]);
    CPPUNIT_ASSERT_EQUAL(static_cast< std::uint8_t >(39),  color1[2]);
    CPPUNIT_ASSERT_EQUAL(static_cast< std::uint8_t >(255), color1[3]);

    const std::string strColor2 = "#45a5bc28";
    std::uint8_t color2[4];
    CPPUNIT_ASSERT_NO_THROW(::fwDataTools::Color::hexaStringToRGBA(strColor2, color2));

    CPPUNIT_ASSERT_EQUAL(static_cast< std::uint8_t >(69),  color2[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast< std::uint8_t >(165), color2[1]);
    CPPUNIT_ASSERT_EQUAL(static_cast< std::uint8_t >(188), color2[2]);
    CPPUNIT_ASSERT_EQUAL(static_cast< std::uint8_t >(40),  color2[3]);

    const std::string strColor3 = "45a5bc28";
    std::uint8_t color3[4];
    CPPUNIT_ASSERT_THROW(::fwDataTools::Color::hexaStringToRGBA(strColor3, color3), ::fwCore::Exception);

    const std::string strColor4 = "45a58";
    CPPUNIT_ASSERT_THROW(::fwDataTools::Color::hexaStringToRGBA(strColor4, color3), ::fwCore::Exception);

    const std::string strColor5 = "#45a564928";
    CPPUNIT_ASSERT_THROW(::fwDataTools::Color::hexaStringToRGBA(strColor5, color3), ::fwCore::Exception);
}

} //namespace ut
} //namespace fwDataTools
