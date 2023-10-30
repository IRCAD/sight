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

#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace sight::io::session::ut
{

class session_test : public CPPUNIT_NS::TestFixture
{
public:

    CPPUNIT_TEST_SUITE(session_test);

    CPPUNIT_TEST(boolean_test);
    CPPUNIT_TEST(integer_test);
    CPPUNIT_TEST(float_test);
    CPPUNIT_TEST(string_test);
    CPPUNIT_TEST(composite_test);
    CPPUNIT_TEST(mesh_test);
    CPPUNIT_TEST(series_test);
    CPPUNIT_TEST(activity_test);
    CPPUNIT_TEST(array_test);
    CPPUNIT_TEST(image_test);
    CPPUNIT_TEST(vector_test);
    CPPUNIT_TEST(point_test);
    CPPUNIT_TEST(point_list_test);
    CPPUNIT_TEST(calibration_info_test);
    CPPUNIT_TEST(camera_test);
    CPPUNIT_TEST(color_test);
    CPPUNIT_TEST(landmarks_test);
    CPPUNIT_TEST(line_test);
    CPPUNIT_TEST(material_test);
    CPPUNIT_TEST(matrix4_test);
    CPPUNIT_TEST(plane_test);
    CPPUNIT_TEST(plane_list_test);
    CPPUNIT_TEST(reconstruction_test);
    CPPUNIT_TEST(structure_traits_test);
    CPPUNIT_TEST(structure_traits_dictionary_test);
    CPPUNIT_TEST(resection_test);
    CPPUNIT_TEST(resection_db_test);
    CPPUNIT_TEST(transfer_function_test);
    CPPUNIT_TEST(dicom_series_test);
    CPPUNIT_TEST(image_series_test);
    CPPUNIT_TEST(model_series_test);
    CPPUNIT_TEST(activity_set_test);
    CPPUNIT_TEST(camera_set_test);
    CPPUNIT_TEST(series_set_test);
    CPPUNIT_TEST(set_test);

    CPPUNIT_TEST(custom_serializer_test);

    CPPUNIT_TEST_SUITE_END();

public:

    // Interface
    void setUp() override;
    void tearDown() override;

    // Tests
    static void boolean_test();
    static void integer_test();
    static void float_test();
    static void string_test();
    static void composite_test();
    static void mesh_test();
    static void series_test();
    static void activity_test();
    static void array_test();
    static void image_test();
    static void vector_test();
    static void point_test();
    static void point_list_test();
    static void calibration_info_test();
    static void camera_test();
    static void color_test();
    static void landmarks_test();
    static void line_test();
    static void material_test();
    static void matrix4_test();
    static void plane_test();
    static void plane_list_test();
    static void reconstruction_test();
    static void structure_traits_test();
    static void structure_traits_dictionary_test();
    static void resection_test();
    static void resection_db_test();
    static void transfer_function_test();
    static void dicom_series_test();
    static void image_series_test();
    static void model_series_test();
    static void activity_set_test();
    static void camera_set_test();
    static void series_set_test();
    static void set_test();

    static void custom_serializer_test();
};

} // namespace sight::io::session::ut
