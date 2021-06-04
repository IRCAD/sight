/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include <data/Image.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::geometry::data
{

namespace ut
{

/**
 * @brief Test many methods to create image.
 */
class MeshTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(MeshTest);
CPPUNIT_TEST(colorizePointsTest);
CPPUNIT_TEST(colorizeCellsTest);
CPPUNIT_TEST(transformTest);
CPPUNIT_TEST(isClosedTest);
CPPUNIT_TEST(cellNormalTest);
CPPUNIT_TEST(pointNormalTest);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void colorizePointsTest();
    void colorizeCellsTest();
    void transformTest();
    void isClosedTest();
    void cellNormalTest();
    void pointNormalTest();
};

} //namespace ut

} //namespace sight::geometry::data
