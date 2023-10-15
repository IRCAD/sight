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
    CPPUNIT_TEST(copyFromCvFloat);
    CPPUNIT_TEST(copyToCvFloat);
    CPPUNIT_TEST(copyFromCvDouble);
    CPPUNIT_TEST(copyToCvDouble);
    CPPUNIT_TEST(copyFromCvMat);
    CPPUNIT_TEST(copyToCvMat);
    CPPUNIT_TEST(copyFromRvecTvec);
    CPPUNIT_TEST(copyToRvecTvec);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    ///Tests the copy from/to cv::Matx44f (float)
    static void copyFromCvFloat();
    static void copyToCvFloat();
    ///Tests the copy from/to cv::Matx44d (double)
    static void copyFromCvDouble();
    static void copyToCvDouble();
    ///Tests the copy from/to cv::Mat
    static void copyFromCvMat();
    static void copyToCvMat();
    ///Tests the copy from/to cv::Mat rvec tvec
    static void copyFromRvecTvec();
    static void copyToRvecTvec();
};

} // namespace sight::io::opencv::ut
