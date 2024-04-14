/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

namespace sight::module::io::vtk::ut
{

/**
 * @brief Test the reader and the writer of image using VTK.
 */
class image_reader_writer_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(image_reader_writer_test);
CPPUNIT_TEST(test_vtk_image_reader);
CPPUNIT_TEST(test_vti_image_reader);
CPPUNIT_TEST(test_mhd_image_reader);
CPPUNIT_TEST(test_image_reader_extension);
CPPUNIT_TEST(test_vtk_image_writer);
CPPUNIT_TEST(test_vtk_image_series_writer);
CPPUNIT_TEST(test_vti_image_writer);
CPPUNIT_TEST(test_mhd_image_writer);
CPPUNIT_TEST(test_image_writer_extension);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void test_vtk_image_reader();
    static void test_vti_image_reader();
    static void test_mhd_image_reader();
    static void test_image_reader_extension();
    static void test_vtk_image_writer();
    static void test_vtk_image_series_writer();
    static void test_vti_image_writer();
    static void test_mhd_image_writer();
    static void test_image_writer_extension();
};

} // namespace sight::module::io::vtk::ut
