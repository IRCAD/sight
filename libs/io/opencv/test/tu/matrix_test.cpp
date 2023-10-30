/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "matrix_test.hpp"

#include <data/matrix4.hpp>

#include <io/opencv/matrix.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::opencv::ut::matrix_test);

namespace sight::io::opencv::ut
{

//-----------------------------------------------------------------------------

void matrix_test::setUp()
{
}

//-----------------------------------------------------------------------------

void matrix_test::tearDown()
{
}

//-----------------------------------------------------------------------------

void matrix_test::copy_from_cv_float()
{
    cv::Matx44f cv_mat         = cv::Matx44f::eye();
    data::matrix4::sptr fw_mat = std::make_shared<data::matrix4>();

    //identity test
    io::opencv::matrix::copy_from_cv(cv_mat, fw_mat);

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "values are not equals at [" + std::to_string(i) + " ; " + std::to_string(j) + "]",
                static_cast<double>(cv_mat(i, j)),
                (*fw_mat)(i, j)
            );
        }
    }

    // 1 to 16 values
    cv_mat = cv::Matx44f(
        1.F,
        2.F,
        3.F,
        4.F,
        5.F,
        6.F,
        7.F,
        8.F,
        9.F,
        10.F,
        11.F,
        12.F,
        13.F,
        14.F,
        15.F,
        16.F
    );

    //values test
    io::opencv::matrix::copy_from_cv(cv_mat, fw_mat);

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "values are not equals at [" + std::to_string(i) + " ; " + std::to_string(j) + "]",
                static_cast<double>(cv_mat(i, j)),
                (*fw_mat)(i, j)
            );
        }
    }
}

//-----------------------------------------------------------------------------

void matrix_test::copy_to_cv_float()
{
    data::matrix4::sptr fw_mat = std::make_shared<data::matrix4>();
    cv::Matx44f cv_mat         = cv::Matx44f::eye();

    //identity test
    io::opencv::matrix::copy_to_cv(fw_mat, cv_mat);

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
                "values are not equals at [" + std::to_string(i) + " ; " + std::to_string(j) + "]",
                (*fw_mat)(i, j),
                static_cast<double>(cv_mat(i, j)),
                1e-8
            );
        }
    }

    data::matrix4::container_t array = {{0.16, 0.15, 0.14, 0.1378942,
        12.0, 11.0, 10.0, 9.0,
        0.08, 0.07, 0.0645687, 0.05,
        40.0, 30.0, 20.0, 10.0
    }
    };
    (*fw_mat) = array;

    io::opencv::matrix::copy_to_cv(fw_mat, cv_mat);

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
                "values are not equals at [" + std::to_string(i) + " ; " + std::to_string(j) + "]",
                static_cast<double>(cv_mat(i, j)),
                (*fw_mat)(i, j),
                1e-8
            );
        }
    }
}

//-----------------------------------------------------------------------------

void matrix_test::copy_from_cv_double()
{
    cv::Matx44d cv_mat         = cv::Matx44d::eye();
    data::matrix4::sptr fw_mat = std::make_shared<data::matrix4>();

    //identity test
    io::opencv::matrix::copy_from_cv(cv_mat, fw_mat);

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "values are not equals at [" + std::to_string(i) + " ; " + std::to_string(j) + "]",
                static_cast<double>(cv_mat(i, j)),
                (*fw_mat)(i, j)
            );
        }
    }

    // 1 to 16 values
    cv_mat = cv::Matx44d(
        1.,
        2.,
        3.,
        4.,
        5.,
        6.,
        7.,
        8.,
        9.,
        10.,
        11.9874563,
        12.,
        13.123456,
        14.,
        15.,
        16.
    );

    //values test
    io::opencv::matrix::copy_from_cv(cv_mat, fw_mat);

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "values are not equals at [" + std::to_string(i) + " ; " + std::to_string(j) + "]",
                static_cast<double>(cv_mat(i, j)),
                (*fw_mat)(i, j)
            );
        }
    }
}

//-----------------------------------------------------------------------------

void matrix_test::copy_to_cv_double()
{
    data::matrix4::sptr fw_mat = std::make_shared<data::matrix4>();
    cv::Matx44d cv_mat         = cv::Matx44d::eye();

    //identity test
    io::opencv::matrix::copy_to_cv(fw_mat, cv_mat);

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "values are not equals at [" + std::to_string(i) + " ; " + std::to_string(j) + "]",
                (*fw_mat)(i, j),
                static_cast<double>(cv_mat(i, j))
            );
        }
    }

    data::matrix4::container_t array = {{0.16, 0.15, 0.14, 0.1378942,
        12.0, 11.0, 10.0, 9.0,
        0.08, 0.07, 0.0645687, 0.05,
        40.0, 30.0, 20.0, 10.0
    }
    };
    (*fw_mat) = array;

    //values test
    io::opencv::matrix::copy_to_cv(fw_mat, cv_mat);

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "values are not equals at [" + std::to_string(i) + " ; " + std::to_string(j) + "]",
                static_cast<double>(cv_mat(i, j)),
                (*fw_mat)(i, j)
            );
        }
    }
}

//-----------------------------------------------------------------------------

void matrix_test::copy_from_cv_mat()
{
    // identity test
    data::matrix4::sptr fw_mat = std::make_shared<data::matrix4>();
    cv::Mat cv_mat             = cv::Mat::eye(4, 4, CV_64F);

    CPPUNIT_ASSERT_NO_THROW(io::opencv::matrix::copy_from_cv(cv_mat, fw_mat));

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "values are not equals at [" + std::to_string(i) + " ; " + std::to_string(j) + "]",
                (*fw_mat)(i, j),
                cv_mat.at<double>(i, j)
            );
        }
    }

    // values test

    cv_mat = (cv::Mat_<double>(4, 4)
              << 1., 2., 3., 4.,
              5., 6., 7., 8.,
              9., 10., 11.9874563, 12.,
              13.123456, 14., 15., 16.);

    CPPUNIT_ASSERT_NO_THROW(io::opencv::matrix::copy_from_cv(cv_mat, fw_mat));

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "values are not equals at [" + std::to_string(i) + " ; " + std::to_string(j) + "]",
                (*fw_mat)(i, j),
                cv_mat.at<double>(i, j)
            );
        }
    }
}

//-----------------------------------------------------------------------------

void matrix_test::copy_to_cv_mat()
{
    data::matrix4::sptr fw_mat = std::make_shared<data::matrix4>();
    cv::Mat cv_mat             = cv::Mat::eye(4, 4, CV_64F);

    //identity test
    CPPUNIT_ASSERT_NO_THROW(io::opencv::matrix::copy_to_cv(fw_mat, cv_mat));

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "values are not equals at [" + std::to_string(i) + " ; " + std::to_string(j) + "]",
                (*fw_mat)(i, j),
                cv_mat.at<double>(i, j)
            );
        }
    }

    data::matrix4::container_t array = {
        0.16, 0.15, 0.14, 0.1378942,
        12.0, 11.0, 10.0, 9.0,
        0.08, 0.07, 0.0645687, 0.05,
        40.0, 30.0, 20.0, 10.0
    };
    (*fw_mat) = array;

    //values test
    io::opencv::matrix::copy_to_cv(fw_mat, cv_mat);

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "values are not equals at [" + std::to_string(i) + " ; " + std::to_string(j) + "]",
                cv_mat.at<double>(i, j),
                (*fw_mat)(i, j)
            );
        }
    }
}

//-----------------------------------------------------------------------------

void matrix_test::copy_from_rvec_tvec()
{
    // Identity test.
    cv::Mat cv_mat = cv::Mat::eye(4, 4, CV_64F);
    // Convert to rvec tvec.
    cv::Mat tvec = cv_mat(cv::Rect(3, 0, 1, 3));
    cv::Mat rvec;
    cv::Rodrigues(cv_mat(cv::Rect(0, 0, 3, 3)), rvec);
    data::matrix4::sptr fw_mat = std::make_shared<data::matrix4>();

    CPPUNIT_ASSERT_NO_THROW(io::opencv::matrix::copy_from_cv(rvec, tvec, fw_mat));

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
                "values are not equals at [" + std::to_string(i) + " ; " + std::to_string(j) + "]",
                (*fw_mat)(i, j),
                cv_mat.at<double>(i, j),
                1e-8
            );
        }
    }

    // Values test (30 degrees around X and translations on every axes).
    cv_mat = (cv::Mat_<double>(4, 4)
              << 1., 0., 0., 4.,
              0., 0.86601905, -0.50001100, 8.,
              0., 0.50001100, 0.86601905, 12.,
              0., 0., 0., 1.);

    tvec = cv_mat(cv::Rect(3, 0, 1, 3));
    cv::Rodrigues(cv_mat(cv::Rect(0, 0, 3, 3)), rvec);

    CPPUNIT_ASSERT_NO_THROW(io::opencv::matrix::copy_from_cv(rvec, tvec, fw_mat));

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
                "values are not equals at [" + std::to_string(i) + " ; " + std::to_string(j) + "]",
                (*fw_mat)(i, j),
                cv_mat.at<double>(i, j),
                1e-8
            );
        }
    }

    // Values test (without cv::Rodrigues)
    // Should be equals to the cvMat above.
    rvec = (cv::Mat_<double>(3, 1) << 0.523611478769991, 0, 0);
    tvec = (cv::Mat_<double>(3, 1) << 4, 8, 12);

    CPPUNIT_ASSERT_NO_THROW(io::opencv::matrix::copy_from_cv(rvec, tvec, fw_mat));

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
                "values are not equals at [" + std::to_string(i) + " ; " + std::to_string(j) + "]",
                (*fw_mat)(i, j),
                cv_mat.at<double>(i, j),
                1e-8
            );
        }
    }
}

//-----------------------------------------------------------------------------

void matrix_test::copy_to_rvec_tvec()
{
    data::matrix4::sptr fw_mat = std::make_shared<data::matrix4>();
    cv::Mat expected_rvec      = (cv::Mat_<double>(3, 1) << 0., 0., 0.);
    cv::Mat expected_tvec      = (cv::Mat_<double>(3, 1) << 0., 0., 0.);
    cv::Mat rvec;
    cv::Mat tvec;

    //identity test
    CPPUNIT_ASSERT_NO_THROW(io::opencv::matrix::copy_to_cv(fw_mat, rvec, tvec));

    for(std::uint8_t i = 0 ; i < 3 ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "values are not equals at [" + std::to_string(i) + "]",
            expected_rvec.at<double>(i),
            rvec.at<double>(i),
            1e-8
        );

        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "values are not equals at [" + std::to_string(i) + "]",
            expected_tvec.at<double>(i),
            tvec.at<double>(i),
            1e-8
        );
    }

    data::matrix4::container_t array = {
        1., 0., 0., 4.,
        0., 0.86601905, -0.50001100, 8.,
        0., 0.50001100, 0.86601905, 12.,
        0., 0., 0., 1.
    };
    (*fw_mat) = array;

    //values test
    io::opencv::matrix::copy_to_cv(fw_mat, rvec, tvec);
    expected_rvec = (cv::Mat_<double>(3, 1) << 0.523611478769991, 0., 0.);
    expected_tvec = (cv::Mat_<double>(3, 1) << 4., 8., 12.);

    for(std::uint8_t i = 0 ; i < 3 ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "values are not equals at [" + std::to_string(i) + "]",
            expected_rvec.at<double>(i),
            rvec.at<double>(i),
            1e-8
        );

        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "values are not equals at [" + std::to_string(i) + "]",
            expected_tvec.at<double>(i),
            tvec.at<double>(i),
            1e-8
        );
    }

    // Reconstruct 4x4 matrix and compare with original Sight one.
    cv::Mat mat4x4 = cv::Mat::eye(4, 4, CV_64F);
    cv::Rodrigues(rvec, mat4x4(cv::Rect(0, 0, 3, 3)));
    tvec.copyTo(mat4x4(cv::Rect(3, 0, 1, 3)));

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
                "values are not equals at [" + std::to_string(i) + " ; " + std::to_string(j) + "]",
                (*fw_mat)(i, j),
                mat4x4.at<double>(i, j),
                1e-8
            );
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::io::opencv::ut
