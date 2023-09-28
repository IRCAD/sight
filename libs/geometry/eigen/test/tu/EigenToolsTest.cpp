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

#include <data/Matrix4.hpp>

#include <geometry/eigen/helper.hpp>

#include <Eigen/Geometry>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::geometry::eigen::ut::EigenToolsTest);

namespace sight::geometry::eigen::ut
{

//------------------------------------------------------------------------------

void EigenToolsTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void EigenToolsTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void EigenToolsTest::eigenToF4s()
{
    Eigen::Matrix<double, 4, 4> mat;

    for(unsigned int r = 0 ; r < 3 ; ++r)
    {
        for(unsigned int c = 0 ; c < 3 ; ++c)
        {
            mat(r, c) = EigenToolsTest::random<double>(-1.F, 1.F);
        }
    }

    mat(0, 3) = 1;
    mat(1, 3) = 2;
    mat(2, 3) = 3;

    mat(3, 0) = 0;
    mat(3, 1) = 0;
    mat(3, 2) = 0;
    mat(3, 3) = 1;

    data::Matrix4::sptr f4sRes = geometry::eigen::helper::toF4s(mat);

    for(unsigned int r = 0 ; r < 4 ; ++r)
    {
        for(unsigned int c = 0 ; c < 4 ; ++c)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "Elements differ at (" + std::to_string(r) + "," + std::to_string(
                    c
                ) + ")",
                mat(r, c),
                (*f4sRes)(r, c)
            );
        }
    }
}

//------------------------------------------------------------------------------

void EigenToolsTest::f4sToEigen()
{
    Eigen::Matrix<float, 4, 4> eigenRes;

    data::Matrix4::sptr mat = std::make_shared<data::Matrix4>();

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

    eigenRes = geometry::eigen::helper::toEigen<float>(mat);

    data::Matrix4::sptr mat2 = geometry::eigen::helper::toF4s(eigenRes);

    for(unsigned int r = 0 ; r < 4 ; ++r)
    {
        for(unsigned int c = 0 ; c < 4 ; ++c)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
                "Elements differ at (" + std::to_string(r) + "," + std::to_string(
                    c
                ) + ")",
                (*mat)(r, c),
                eigenRes(r, c)
                ,
                0.0000000001
            );
        }
    }
}

//------------------------------------------------------------------------------

void EigenToolsTest::eigenMatToRvecTvec()
{
    Eigen::Matrix4d m        = Eigen::Matrix4d::Identity();
    Eigen::AngleAxisd rotVec = Eigen::AngleAxisd(0.2, Eigen::Vector3d(0.F, 0.F, 1.0).normalized());

    Eigen::Vector3d expected_rvec = rotVec.angle() * rotVec.axis();
    Eigen::Vector3d expected_tvec(0.3, 0.4, 0.5);

    m.block<3, 3>(0, 0) = rotVec.toRotationMatrix();
    m.block<3, 1>(0, 3) = expected_tvec;

    geometry::eigen::helper::RvecTvecType actualRvecTvec =
        geometry::eigen::helper::eigenMatToRvecTvec(m);

    for(unsigned int i = 0 ; i < 3 ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "RVecs differ at " + std::to_string(i)
            ,
            expected_rvec(i),
            actualRvecTvec.first(i),
            0.0000000001
        );
    }

    for(unsigned int i = 0 ; i < 3 ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "TVecs differ at " + std::to_string(i)
            ,
            expected_tvec(i),
            actualRvecTvec.second(i),
            0.0000000001
        );
    }
}

//------------------------------------------------------------------------------

void EigenToolsTest::f4sMatToRvecTvec()
{
    Eigen::Matrix4d m        = Eigen::Matrix4d::Identity();
    Eigen::AngleAxisd rotVec = Eigen::AngleAxisd(0.2, Eigen::Vector3d(0.F, 0.F, 1.0).normalized());

    Eigen::Vector3d expected_rvec = rotVec.angle() * rotVec.axis();
    Eigen::Vector3d expected_tvec(0.3, 0.4, 0.5);

    m.block<3, 3>(0, 0) = rotVec.toRotationMatrix();
    m.block<3, 1>(0, 3) = expected_tvec;

    data::Matrix4::sptr trf = geometry::eigen::helper::toF4s(m);

    geometry::eigen::helper::RvecTvecType actualRvecTvec =
        geometry::eigen::helper::f4sMatToRvecTvec(const_pointer_cast<data::Matrix4>(trf));

    for(unsigned int i = 0 ; i < 3 ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "RVecs differ at " + std::to_string(i)
            ,
            expected_rvec(i),
            actualRvecTvec.first(i),
            0.0000000001
        );
    }

    for(unsigned int i = 0 ; i < 3 ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "TVecs differ at " + std::to_string(i)
            ,
            expected_tvec(i),
            actualRvecTvec.second(i),
            0.0000000001
        );
    }
}

//------------------------------------------------------------------------------

void EigenToolsTest::float16ToEigen()
{
    std::array<float, 16> mat16 = {{0.1F, 0.2F, 0.3F, 1.F,
        0.4F, 0.5F, 0.6F, 2.F,
        0.7F, 0.8F, 0.9F, 3.F,
        0.F, 0.F, 0.F, 1.F
    }
    };

    Eigen::Matrix<double, 4, 4, Eigen::RowMajor> mat;

    mat = geometry::eigen::helper::toEigen(mat16);

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
