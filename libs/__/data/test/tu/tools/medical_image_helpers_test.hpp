/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

namespace sight::data::tools::ut
{

class medical_image_helpers_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(medical_image_helpers_test);
CPPUNIT_TEST(getMinMaxTest);
CPPUNIT_TEST(getPixelTest);
CPPUNIT_TEST(setPixelTest);
CPPUNIT_TEST(isBufNull);
CPPUNIT_TEST(testLandmarks);
CPPUNIT_TEST(testImageValidity);
CPPUNIT_TEST(testSliceIndex);
CPPUNIT_TEST(testDistances);
CPPUNIT_TEST(testDistanceVisibility);
CPPUNIT_TEST(testLandmarksVisibility);
CPPUNIT_TEST(computeHistogram);
CPPUNIT_TEST_SUITE_END();

public:

    /// Does nothing.
    void setUp() override;
    /// Does nothing.
    void tearDown() override;

    /// Tests the getMinMax method.
    static void getMinMaxTest();

    /// Tests the getPixel method for several image pixel types
    static void getPixelTest();

    /// Tests the setPixel method for several image pixel types
    static void setPixelTest();

    /// Tests isBuffNull method.
    static void isBufNull();

    /// Tests helper method around landmarks (get/set).
    static void testLandmarks();

    /// Tests checkImageValidity method.
    static void testImageValidity();

    /// Tests helper methods around slice count.
    void testSliceCount();

    /// Tests helper methods around slice indexes.
    static void testSliceIndex();

    /// Tests helper methods around distances.
    static void testDistances();

    /// Tests set/get methods on distance visibility.
    static void testDistanceVisibility();

    /// Tests set/get methods on landmarks visibility.
    static void testLandmarksVisibility();

    /// Test the computation of the image histogram
    static void computeHistogram();
};

} // namespace sight::data::tools::ut
