/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#pragma once

#include <core/base.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::data::tools
{

namespace ut
{

class MedicalImageHelpersTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(MedicalImageHelpersTest);
CPPUNIT_TEST(getMinMaxTest);
CPPUNIT_TEST(getPixelTest);
CPPUNIT_TEST(setPixelTest);
CPPUNIT_TEST(isBufNull);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    void getMinMaxTest();

    // Test the getPixel method for several image pixel types
    void getPixelTest();

    //------------------------------------------------------------------------------

    // Test the setPixel method for several image pixel types
    void setPixelTest();

    void isBufNull();
};

} // namespace ut

} // namespace sight::data::tools
