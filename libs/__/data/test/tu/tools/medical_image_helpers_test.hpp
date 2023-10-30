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
CPPUNIT_TEST(get_min_max_test);
CPPUNIT_TEST(get_pixel_test);
CPPUNIT_TEST(set_pixel_test);
CPPUNIT_TEST(is_buf_null);
CPPUNIT_TEST(test_landmarks);
CPPUNIT_TEST(test_image_validity);
CPPUNIT_TEST(test_slice_index);
CPPUNIT_TEST(test_distances);
CPPUNIT_TEST(test_distance_visibility);
CPPUNIT_TEST(test_landmarks_visibility);
CPPUNIT_TEST(compute_histogram);
CPPUNIT_TEST_SUITE_END();

public:

    /// Does nothing.
    void setUp() override;
    /// Does nothing.
    void tearDown() override;

    /// Tests the get_min_max method.
    static void get_min_max_test();

    /// Tests the getPixel method for several image pixel types
    static void get_pixel_test();

    /// Tests the setPixel method for several image pixel types
    static void set_pixel_test();

    /// Tests isBuffNull method.
    static void is_buf_null();

    /// Tests helper method around landmarks (get/set).
    static void test_landmarks();

    /// Tests check_image_validity method.
    static void test_image_validity();

    /// Tests helper methods around slice count.
    void test_slice_count();

    /// Tests helper methods around slice indexes.
    static void test_slice_index();

    /// Tests helper methods around distances.
    static void test_distances();

    /// Tests set/get methods on distance visibility.
    static void test_distance_visibility();

    /// Tests set/get methods on landmarks visibility.
    static void test_landmarks_visibility();

    /// Test the computation of the image histogram
    static void compute_histogram();
};

} // namespace sight::data::tools::ut
