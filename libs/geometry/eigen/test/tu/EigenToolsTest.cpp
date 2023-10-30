/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include "EigenToolsTest.hpp"

#include <data/matrix4.hpp>

#include <geometry/eigen/helper.hpp>

#include <Eigen/Geometry>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::geometry::eigen::ut::eigen_tools_test);

namespace sight::geometry::eigen::ut
{

//------------------------------------------------------------------------------

void eigen_tools_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void eigen_tools_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void eigen_tools_test::from_eigen()
{
    Eigen::Matrix<double, 4, 4> mat;

    for(unsigned int r = 0 ; r < 3 ; ++r)
    {
        for(unsigned int c = 0 ; c < 3 ; ++c)
        {
            mat(r, c) = eigen_tools_test::random<double>(-1.F, 1.F);
        }
    }

    mat(0, 3) = 1;
    mat(1, 3) = 2;
    mat(2, 3) = 3;

    mat(3, 0) = 0;
    mat(3, 1) = 0;
    mat(3, 2) = 0;
    mat(3, 3) = 1;

    data::matrix4::sptr f4s_res = geometry::eigen::helper::from_eigen(mat);

    for(unsigned int r = 0 ; r < 4 ; ++r)
    {
        for(unsigned int c = 0 ; c < 4 ; ++c)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "Elements differ at (" + std::to_string(r) + "," + std::to_string(
                    c
                ) + ")",
                mat(r, c),
                (*f4s_res)(r, c)
            );
        }
    }
}

//------------------------------------------------------------------------------

void eigen_tools_test::f4s_to_eigen()
{
    Eigen::Matrix<float, 4, 4> eigen_res;

    data::matrix4::sptr mat = std::make_shared<data::matrix4>();

    for(unsigned int r = 0 ; r < 3 ; ++r)
    {
        for(unsigned int c = 0 ; c < 3 ; ++c)
        {
            (*mat)(r, c) = random<float>(-1.F, 1.F);
        }
    }

    (*mat)(0, 3) = 1;
    (*mat)(1, 3) = 2;
    (*mat)(2, 3) = 3;

    (*mat)(3, 0) = 0;
    (*mat)(3, 1) = 0;
    (*mat)(3, 2) = 0;
    (*mat)(3, 3) = 1;

    eigen_res = geometry::eigen::helper::to_eigen<float>(mat);

    data::matrix4::sptr mat2 = geometry::eigen::helper::from_eigen(eigen_res);

    for(unsigned int r = 0 ; r < 4 ; ++r)
    {
        for(unsigned int c = 0 ; c < 4 ; ++c)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
                "Elements differ at (" + std::to_string(r) + "," + std::to_string(
                    c
                ) + ")",
                (*mat)(r, c),
                eigen_res(r, c)
                ,
                0.0000000001
            );
        }
    }
}

//------------------------------------------------------------------------------

void eigen_tools_test::eigen_mat_to_rvec_tvec()
{
    Eigen::Matrix4d m         = Eigen::Matrix4d::Identity();
    Eigen::AngleAxisd rot_vec = Eigen::AngleAxisd(0.2, Eigen::Vector3d(0.F, 0.F, 1.0).normalized());

    Eigen::Vector3d expected_rvec = rot_vec.angle() * rot_vec.axis();
    Eigen::Vector3d expected_tvec(0.3, 0.4, 0.5);

    m.block<3, 3>(0, 0) = rot_vec.toRotationMatrix();
    m.block<3, 1>(0, 3) = expected_tvec;

    geometry::eigen::helper::rvec_tvec_t actual_rvec_tvec =
        geometry::eigen::helper::eigen_mat_to_rvec_tvec(m);

    for(unsigned int i = 0 ; i < 3 ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "RVecs differ at " + std::to_string(i)
            ,
            expected_rvec(i),
            actual_rvec_tvec.first(i),
            0.0000000001
        );
    }

    for(unsigned int i = 0 ; i < 3 ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "TVecs differ at " + std::to_string(i)
            ,
            expected_tvec(i),
            actual_rvec_tvec.second(i),
            0.0000000001
        );
    }
}

//------------------------------------------------------------------------------

void eigen_tools_test::f4s_mat_to_rvec_tvec()
{
    Eigen::Matrix4d m         = Eigen::Matrix4d::Identity();
    Eigen::AngleAxisd rot_vec = Eigen::AngleAxisd(0.2, Eigen::Vector3d(0.F, 0.F, 1.0).normalized());

    Eigen::Vector3d expected_rvec = rot_vec.angle() * rot_vec.axis();
    Eigen::Vector3d expected_tvec(0.3, 0.4, 0.5);

    m.block<3, 3>(0, 0) = rot_vec.toRotationMatrix();
    m.block<3, 1>(0, 3) = expected_tvec;

    data::matrix4::sptr trf = geometry::eigen::helper::from_eigen(m);

    geometry::eigen::helper::rvec_tvec_t actual_rvec_tvec =
        geometry::eigen::helper::f4s_mat_to_rvec_tvec(const_pointer_cast<data::matrix4>(trf));

    for(unsigned int i = 0 ; i < 3 ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "RVecs differ at " + std::to_string(i)
            ,
            expected_rvec(i),
            actual_rvec_tvec.first(i),
            0.0000000001
        );
    }

    for(unsigned int i = 0 ; i < 3 ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "TVecs differ at " + std::to_string(i)
            ,
            expected_tvec(i),
            actual_rvec_tvec.second(i),
            0.0000000001
        );
    }
}

//------------------------------------------------------------------------------

void eigen_tools_test::float16_to_eigen()
{
    std::array<float, 16> mat16 = {{0.1F, 0.2F, 0.3F, 1.F,
        0.4F, 0.5F, 0.6F, 2.F,
        0.7F, 0.8F, 0.9F, 3.F,
        0.F, 0.F, 0.F, 1.F
    }
    };

    Eigen::Matrix<double, 4, 4, Eigen::RowMajor> mat;

    mat = geometry::eigen::helper::to_eigen(mat16);

    for(unsigned int r = 0 ; r < 4 ; ++r)
    {
        for(unsigned int c = 0 ; c < 4 ; ++c)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
                "Matrices differ at (" + std::to_string(r) + " , " + std::to_string(c) + ")",
                mat16[4 * r + c],
                mat(r, c),
                0.00000000001
            );
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::geometry::eigen::ut
