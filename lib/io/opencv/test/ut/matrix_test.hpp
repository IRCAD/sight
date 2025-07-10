/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

namespace sight::io::opencv::ut
{

/**
 * @brief Tests the io::opencv::matrix functions
 */
class matrix_test : public CPPUNIT_NS::TestFixture
{
public:

    CPPUNIT_TEST_SUITE(matrix_test);
    CPPUNIT_TEST(copy_from_cv_float);
    CPPUNIT_TEST(copy_to_cv_float);
    CPPUNIT_TEST(copy_from_cv_double);
    CPPUNIT_TEST(copy_to_cv_double);
    CPPUNIT_TEST(copy_from_cv_mat);
    CPPUNIT_TEST(copy_to_cv_mat);
    CPPUNIT_TEST(copy_from_rvec_tvec);
    CPPUNIT_TEST(copy_to_rvec_tvec);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    ///Tests the copy from/to cv::Matx44f (float)
    static void copy_from_cv_float();
    static void copy_to_cv_float();
    ///Tests the copy from/to cv::Matx44d (double)
    static void copy_from_cv_double();
    static void copy_to_cv_double();
    ///Tests the copy from/to cv::Mat
    static void copy_from_cv_mat();
    static void copy_to_cv_mat();
    ///Tests the copy from/to cv::Mat rvec tvec
    static void copy_from_rvec_tvec();
    static void copy_to_rvec_tvec();
};

} // namespace sight::io::opencv::ut
