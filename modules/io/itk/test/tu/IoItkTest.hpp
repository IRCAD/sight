/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

namespace sight::module::io::itk::ut
{

/**
 * @brief Test many methods to create mesh.
 */
class io_itk_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(io_itk_test);
CPPUNIT_TEST(test_image_series_writer_jpg);
CPPUNIT_TEST(test_save_load_inr);
CPPUNIT_TEST(test_save_load_nifti);
CPPUNIT_TEST(image_series_inr_test);
CPPUNIT_TEST(image_series_nifti_test);
CPPUNIT_TEST(series_set_inr_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void test_image_series_writer_jpg();
    static void test_save_load_inr();
    static void test_save_load_nifti();
    static void image_series_inr_test();
    static void image_series_nifti_test();
    static void series_set_inr_test();
};

} // namespace sight::module::io::itk::ut
