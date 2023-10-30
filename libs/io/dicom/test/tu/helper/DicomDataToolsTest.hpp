/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

namespace sight::io::dicom::helper::ut
{

class dicom_data_tools_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(dicom_data_tools_test);
CPPUNIT_TEST(get_pixel_type_test);
CPPUNIT_TEST(get_photometric_interpretation_test);
CPPUNIT_TEST(convert_to_presentation_type_test);
CPPUNIT_TEST(convert_to_representation_mode_test);
CPPUNIT_TEST(convert_point_to_frame_number_nominal_test);
CPPUNIT_TEST(convert_point_to_frame_number_failure_test);
CPPUNIT_TEST(convert_frame_number_to_z_coordinate_nominal_test);
CPPUNIT_TEST(convert_frame_number_to_z_coordinate_failure_test);
CPPUNIT_TEST_SUITE_END();

public:

    static void get_pixel_type_test();
    static void get_photometric_interpretation_test();
    static void convert_to_presentation_type_test();
    static void convert_to_representation_mode_test();
    static void convert_point_to_frame_number_nominal_test();
    static void convert_point_to_frame_number_failure_test();
    static void convert_frame_number_to_z_coordinate_nominal_test();
    static void convert_frame_number_to_z_coordinate_failure_test();
};

} // namespace sight::io::dicom::helper::ut
