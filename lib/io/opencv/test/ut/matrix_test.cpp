/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
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

#include <data/matrix4.hpp>

#include <io/opencv/matrix.hpp>

#include <doctest/doctest.h>

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>

TEST_SUITE("sight::io::opencv::matrix")
{
//-----------------------------------------------------------------------------

    TEST_CASE("copy_from_cv_float")
    {
        cv::Matx44f cv_mat                = cv::Matx44f::eye();
        sight::data::matrix4::sptr fw_mat = std::make_shared<sight::data::matrix4>();

        //identity test
        sight::io::opencv::matrix::copy_from_cv(cv_mat, fw_mat);

        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_MESSAGE(
                    static_cast<double>(cv_mat(i, j)) == (*fw_mat)(i, j),
                    "values are not equals at [",
                    std::to_string(i),
                    " ; ",
                    std::to_string(j),
                    "]"
                );
            }
        }

        // 1 to 16 values
        cv_mat = cv::Matx44f(1.F, 2.F, 3.F, 4.F, 5.F, 6.F, 7.F, 8.F, 9.F, 10.F, 11.F, 12.F, 13.F, 14.F, 15.F, 16.F);

        //values test
        sight::io::opencv::matrix::copy_from_cv(cv_mat, fw_mat);

        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_MESSAGE(
                    static_cast<double>(cv_mat(i, j)) == (*fw_mat)(i, j),
                    "values are not equals at [",
                    std::to_string(i),
                    " ; ",
                    std::to_string(j),
                    "]"
                );
            }
        }
    }

//-----------------------------------------------------------------------------

    TEST_CASE("copy_to_cv_float")
    {
        sight::data::matrix4::sptr fw_mat = std::make_shared<sight::data::matrix4>();
        cv::Matx44f cv_mat                = cv::Matx44f::eye();

        //identity test
        sight::io::opencv::matrix::copy_to_cv(fw_mat, cv_mat);

        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_EQ((*fw_mat)(i, j), static_cast<double>(cv_mat(i, j)));
            }
        }

        sight::data::matrix4::container_t array = {{0.16, 0.15, 0.14, 0.1378942,
            12.0, 11.0, 10.0, 9.0,
            0.08, 0.07, 0.0645687, 0.05,
            40.0, 30.0, 20.0, 10.0
        }
        };
        (*fw_mat) = array;

        sight::io::opencv::matrix::copy_to_cv(fw_mat, cv_mat);

        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_MESSAGE(
                    static_cast<double>(cv_mat(i, j)) == doctest::Approx((*fw_mat)(i, j)),
                    "values are not equals at [",
                    std::to_string(i),
                    " ; ",
                    std::to_string(j),
                    "]"
                );
            }
        }
    }

//-----------------------------------------------------------------------------

    TEST_CASE("copy_from_cv_double")
    {
        cv::Matx44d cv_mat                = cv::Matx44d::eye();
        sight::data::matrix4::sptr fw_mat = std::make_shared<sight::data::matrix4>();

        //identity test
        sight::io::opencv::matrix::copy_from_cv(cv_mat, fw_mat);

        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_MESSAGE(
                    static_cast<double>(cv_mat(i, j)) == (*fw_mat)(i, j),
                    "values are not equals at [",
                    std::to_string(i),
                    " ; ",
                    std::to_string(j),
                    "]"
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
        sight::io::opencv::matrix::copy_from_cv(cv_mat, fw_mat);

        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_MESSAGE(
                    static_cast<double>(cv_mat(i, j)) == (*fw_mat)(i, j),
                    "values are not equals at [",
                    std::to_string(i),
                    " ; ",
                    std::to_string(j),
                    "]"
                );
            }
        }
    }

//-----------------------------------------------------------------------------

    TEST_CASE("copy_to_cv_double")
    {
        sight::data::matrix4::sptr fw_mat = std::make_shared<sight::data::matrix4>();
        cv::Matx44d cv_mat                = cv::Matx44d::eye();

        //identity test
        sight::io::opencv::matrix::copy_to_cv(fw_mat, cv_mat);

        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_MESSAGE(
                    static_cast<double>(cv_mat(i, j)) == (*fw_mat)(i, j),
                    "values are not equals at [",
                    std::to_string(i),
                    " ; ",
                    std::to_string(j),
                    "]"
                );
            }
        }

        sight::data::matrix4::container_t array = {{0.16, 0.15, 0.14, 0.1378942,
            12.0, 11.0, 10.0, 9.0,
            0.08, 0.07, 0.0645687, 0.05,
            40.0, 30.0, 20.0, 10.0
        }
        };
        (*fw_mat) = array;

        //values test
        sight::io::opencv::matrix::copy_to_cv(fw_mat, cv_mat);

        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_MESSAGE(
                    static_cast<double>(cv_mat(i, j)) == (*fw_mat)(i, j),
                    "values are not equals at [",
                    std::to_string(i),
                    " ; ",
                    std::to_string(j),
                    "]"
                );
            }
        }
    }

//-----------------------------------------------------------------------------

    TEST_CASE("copy_from_cv_mat")
    {
        // identity test
        sight::data::matrix4::sptr fw_mat = std::make_shared<sight::data::matrix4>();
        cv::Mat cv_mat                    = cv::Mat::eye(4, 4, CV_64F);

        CHECK_NOTHROW(sight::io::opencv::matrix::copy_from_cv(cv_mat, fw_mat));

        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_MESSAGE(
                    static_cast<double>(cv_mat.at<double>(i, j)) == (*fw_mat)(i, j),
                    "values are not equals at [",
                    std::to_string(i),
                    " ; ",
                    std::to_string(j),
                    "]"
                );
            }
        }

        // values test

        cv_mat = (cv::Mat_<double>(4, 4)
                  << 1., 2., 3., 4.,
                  5., 6., 7., 8.,
                  9., 10., 11.9874563, 12.,
                  13.123456, 14., 15., 16.);

        CHECK_NOTHROW(sight::io::opencv::matrix::copy_from_cv(cv_mat, fw_mat));

        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_MESSAGE(
                    static_cast<double>(cv_mat.at<double>(i, j)) == (*fw_mat)(i, j),
                    "values are not equals at [",
                    std::to_string(i),
                    " ; ",
                    std::to_string(j),
                    "]"
                );
            }
        }
    }

//-----------------------------------------------------------------------------

    TEST_CASE("copy_to_cv_mat")
    {
        sight::data::matrix4::sptr fw_mat = std::make_shared<sight::data::matrix4>();
        cv::Mat cv_mat                    = cv::Mat::eye(4, 4, CV_64F);

        //identity test
        CHECK_NOTHROW(sight::io::opencv::matrix::copy_to_cv(fw_mat, cv_mat));

        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_MESSAGE(
                    static_cast<double>(cv_mat.at<double>(i, j)) == (*fw_mat)(i, j),
                    "values are not equals at [",
                    std::to_string(i),
                    " ; ",
                    std::to_string(j),
                    "]"
                );
            }
        }

        sight::data::matrix4::container_t array = {
            0.16, 0.15, 0.14, 0.1378942,
            12.0, 11.0, 10.0, 9.0,
            0.08, 0.07, 0.0645687, 0.05,
            40.0, 30.0, 20.0, 10.0
        };
        (*fw_mat) = array;

        //values test
        sight::io::opencv::matrix::copy_to_cv(fw_mat, cv_mat);

        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_MESSAGE(
                    static_cast<double>(cv_mat.at<double>(i, j)) == (*fw_mat)(i, j),
                    "values are not equals at [",
                    std::to_string(i),
                    " ; ",
                    std::to_string(j),
                    "]"
                );
            }
        }
    }

//-----------------------------------------------------------------------------

    TEST_CASE("copy_from_rvec_tvec")
    {
        // Identity test.
        cv::Mat cv_mat = cv::Mat::eye(4, 4, CV_64F);
        // Convert to rvec tvec.
        cv::Mat tvec = cv_mat(cv::Rect(3, 0, 1, 3));
        cv::Mat rvec;
        cv::Rodrigues(cv_mat(cv::Rect(0, 0, 3, 3)), rvec);
        sight::data::matrix4::sptr fw_mat = std::make_shared<sight::data::matrix4>();

        CHECK_NOTHROW(sight::io::opencv::matrix::copy_from_cv(rvec, tvec, fw_mat));

        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_MESSAGE(
                    static_cast<double>(cv_mat.at<double>(i, j)) == doctest::Approx((*fw_mat)(i, j)),
                    "values are not equals at [",
                    std::to_string(i),
                    " ; ",
                    std::to_string(j),
                    "]"
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

        CHECK_NOTHROW(sight::io::opencv::matrix::copy_from_cv(rvec, tvec, fw_mat));

        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_MESSAGE(
                    static_cast<double>(cv_mat.at<double>(i, j)) == doctest::Approx((*fw_mat)(i, j)),
                    "values are not equals at [",
                    std::to_string(i),
                    " ; ",
                    std::to_string(j),
                    "]"
                );
            }
        }

        // Values test (without cv::Rodrigues)
        // Should be equals to the cvMat above.
        rvec = (cv::Mat_<double>(3, 1) << 0.523611478769991, 0, 0);
        tvec = (cv::Mat_<double>(3, 1) << 4, 8, 12);

        CHECK_NOTHROW(sight::io::opencv::matrix::copy_from_cv(rvec, tvec, fw_mat));

        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_MESSAGE(
                    static_cast<double>(cv_mat.at<double>(i, j)) == doctest::Approx((*fw_mat)(i, j)),
                    "values are not equals at [",
                    std::to_string(i),
                    " ; ",
                    std::to_string(j),
                    "]"
                );
            }
        }
    }

//-----------------------------------------------------------------------------

    TEST_CASE("copy_to_rvec_tvec")
    {
        sight::data::matrix4::sptr fw_mat = std::make_shared<sight::data::matrix4>();
        cv::Mat expected_rvec             = (cv::Mat_<double>(3, 1) << 0., 0., 0.);
        cv::Mat expected_tvec             = (cv::Mat_<double>(3, 1) << 0., 0., 0.);
        cv::Mat rvec;
        cv::Mat tvec;

        //identity test
        CHECK_NOTHROW(sight::io::opencv::matrix::copy_to_cv(fw_mat, rvec, tvec));

        for(std::uint8_t i = 0 ; i < 3 ; ++i)
        {
            CHECK_MESSAGE(
                static_cast<double>(expected_rvec.at<double>(i)) == doctest::Approx(rvec.at<double>(i)),
                "values are not equals at [",
                std::to_string(i),
                "]"
            );

            CHECK_MESSAGE(
                static_cast<double>(expected_tvec.at<double>(i)) == doctest::Approx(tvec.at<double>(i)),
                "values are not equals at [",
                std::to_string(i),
                "]"
            );
        }

        sight::data::matrix4::container_t array = {
            1., 0., 0., 4.,
            0., 0.86601905, -0.50001100, 8.,
            0., 0.50001100, 0.86601905, 12.,
            0., 0., 0., 1.
        };
        (*fw_mat) = array;

        //values test
        sight::io::opencv::matrix::copy_to_cv(fw_mat, rvec, tvec);
        expected_rvec = (cv::Mat_<double>(3, 1) << 0.523611478769991, 0., 0.);
        expected_tvec = (cv::Mat_<double>(3, 1) << 4., 8., 12.);

        for(std::uint8_t i = 0 ; i < 3 ; ++i)
        {
            CHECK_MESSAGE(
                static_cast<double>(expected_rvec.at<double>(i)) == doctest::Approx(rvec.at<double>(i)),
                "values are not equals at [",
                std::to_string(i),
                "]"
            );

            CHECK_MESSAGE(
                static_cast<double>(expected_tvec.at<double>(i)) == doctest::Approx(tvec.at<double>(i)),
                "values are not equals at [",
                std::to_string(i),
                "]"
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
                CHECK_MESSAGE(
                    static_cast<double>(mat4x4.at<double>(i, j)) == doctest::Approx((*fw_mat)(i, j)),
                    "values are not equals at [",
                    std::to_string(i),
                    " ; ",
                    std::to_string(j),
                    "]"
                );
            }
        }
    }

//-----------------------------------------------------------------------------
}
