/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <fwData/Color.hpp>

#include <exception>
#include <iostream>
#include <map>
#include <ostream>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::ColorTest );

namespace fwData
{
namespace ut
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
    const float R = 0.2f;
    const float G = 0.8f;
    const float B = 0.5f;
    const float A = 0.6f;

    ::fwData::Color::sptr color = ::fwData::Color::New(R, G, B, A);

    CPPUNIT_ASSERT_EQUAL(color->red(), R);
    CPPUNIT_ASSERT_EQUAL(color->green(), G);
    CPPUNIT_ASSERT_EQUAL(color->blue(), B);
    CPPUNIT_ASSERT_EQUAL(color->alpha(), A);
}

//------------------------------------------------------------------------------

void ColorTest::methode2()
{
    //-----------test values
    const float R = 0.2f;
    const float G = 0.8f;
    const float B = 0.5f;
    const float A = 0.6f;

    ::fwData::Color::sptr color = ::fwData::Color::New();

    ::fwData::Color::ColorArray array;
    array[0] = R;
    array[1] = G;
    array[2] = B;
    array[3] = A;

    color->setRGBA(array);

    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[0], R);
    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[1], G);
    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[2], B);
    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[3], A);
}

//------------------------------------------------------------------------------

void ColorTest::methode3()
{
    // fuchsia string value
    const std::string fuchsia = "#FF006E";
    // fuchsia float values
    const float R = 1.f;
    const float G = 0.f;
    const float B = 110.f/255.f;
    const float A = 1.f;

    ::fwData::Color::sptr color = ::fwData::Color::New();

    color->setRGBA(fuchsia);

    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[0], R);
    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[1], G);
    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[2], B);
    CPPUNIT_ASSERT_EQUAL(color->getRGBA()[3], A);
}

} //namespace ut
} //namespace fwData
