/************************************************************************
 *
 * Copyright (C) 2017-2018 IRCAD France
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

#include <eigenTools/helper.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <Eigen/Geometry>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::eigenTools::ut::EigenToolsTest );

namespace eigenTools
{
namespace ut
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

    Eigen::Matrix< double, 4, 4> mat;

    ::fwData::TransformationMatrix3D::sptr f4sRes = ::fwData::TransformationMatrix3D::New();

    for(unsigned int r = 0; r < 3; ++r)
    {
        for(unsigned int c = 0; c < 3; ++c)
        {
            mat(r, c) = EigenToolsTest::random<double>(-1.f, 1.f);
        }
    }

    mat(0, 3) = 1;
    mat(1, 3) = 2;
    mat(2, 3) = 3;

    mat(3, 0) = 0;
    mat(3, 1) = 0;
    mat(3, 2) = 0;
    mat(3, 3) = 1;

    f4sRes = ::eigenTools::helper::toF4s(mat);

    for(unsigned int r = 0; r < 4; ++r)
    {
        for(unsigned int c = 0; c < 4; ++c)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Elements differ at (" + std::to_string(r) + ","  + std::to_string(
                                                     c) + ")", mat(r, c), f4sRes->getCoefficient(r, c), 0.0000000001);
        }
    }
}

//------------------------------------------------------------------------------

void EigenToolsTest::f4sToEigen()
{
    Eigen::Matrix< float, 4, 4> eigenRes;

    ::fwData::TransformationMatrix3D::sptr mat = ::fwData::TransformationMatrix3D::New();

    for(unsigned int r = 0; r < 3; ++r)
    {
        for(unsigned int c = 0; c < 3; ++c)
        {
            mat->setCoefficient(r, c, random<float>(-1.f, 1.f));
        }
    }

    mat->setCoefficient(0, 3, 1);
    mat->setCoefficient(1, 3, 2);
    mat->setCoefficient(2, 3, 3);

    mat->setCoefficient(3, 0, 0);
    mat->setCoefficient(3, 1, 0);
    mat->setCoefficient(3, 2, 0);
    mat->setCoefficient(3, 3, 1);

    eigenRes = ::eigenTools::helper::toEigen< float >(mat);

    ::fwData::TransformationMatrix3D::sptr mat2 = ::eigenTools::helper::toF4s(eigenRes);

    for(unsigned int r = 0; r < 4; ++r)
    {
        for(unsigned int c = 0; c < 4; ++c)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Elements differ at (" + std::to_string(r) + ","  + std::to_string(
                                                     c) + ")", mat->getCoefficient(r, c), eigenRes(r, c)
                                                 , 0.0000000001);
        }
    }
}

//------------------------------------------------------------------------------

void EigenToolsTest::eigenMatToRvecTvec()
{
    ::Eigen::Matrix4d m        = ::Eigen::Matrix4d::Identity();
    ::Eigen::AngleAxisd rotVec = ::Eigen::AngleAxisd(0.2, Eigen::Vector3d(0.f, 0.f, 1.0).normalized());

    ::Eigen::Vector3d expected_rvec = rotVec.angle() * rotVec.axis();
    ::Eigen::Vector3d expected_tvec( 0.3, 0.4, 0.5);

    m.block<3, 3>(0, 0) = rotVec.toRotationMatrix();
    m.block<3, 1>(0, 3) = expected_tvec;

    ::eigenTools::helper::RvecTvecType actualRvecTvec =
        ::eigenTools::helper::eigenMatToRvecTvec(m);

    for(unsigned int i = 0; i < 3; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("RVecs differ at "+ std::to_string(i)
                                             , expected_rvec(i), actualRvecTvec.first(i), 0.0000000001);
    }

    for(unsigned int i = 0; i < 3; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("TVecs differ at "+ std::to_string(i)
                                             , expected_tvec(i), actualRvecTvec.second(i), 0.0000000001);
    }
}

//------------------------------------------------------------------------------

void EigenToolsTest::f4sMatToRvecTvec()
{
    ::Eigen::Matrix4d m        = ::Eigen::Matrix4d::Identity();
    ::Eigen::AngleAxisd rotVec = ::Eigen::AngleAxisd(0.2, Eigen::Vector3d(0.f, 0.f, 1.0).normalized());

    ::Eigen::Vector3d expected_rvec = rotVec.angle() * rotVec.axis();
    ::Eigen::Vector3d expected_tvec( 0.3, 0.4, 0.5);

    m.block<3, 3>(0, 0) = rotVec.toRotationMatrix();
    m.block<3, 1>(0, 3) = expected_tvec;

    ::fwData::TransformationMatrix3D::sptr trf = ::eigenTools::helper::toF4s(m);

    ::eigenTools::helper::RvecTvecType actualRvecTvec =
        ::eigenTools::helper::f4sMatToRvecTvec(::fwData::TransformationMatrix3D::constCast(trf));

    for(unsigned int i = 0; i < 3; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("RVecs differ at "+ std::to_string(i)
                                             , expected_rvec(i), actualRvecTvec.first(i), 0.0000000001);
    }

    for(unsigned int i = 0; i < 3; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("TVecs differ at "+ std::to_string(i)
                                             , expected_tvec(i), actualRvecTvec.second(i), 0.0000000001);
    }
}

//------------------------------------------------------------------------------

void EigenToolsTest::float16ToEigen()
{
    std::array<float, 16 > mat16 = {{ 0.1f, 0.2f, 0.3f, 1.f,
                                      0.4f, 0.5f, 0.6f,  2.f,
                                      0.7f, 0.8f, 0.9f, 3.f,
                                      0.f,  0.f,  0.f,  1.f }};

    ::Eigen::Matrix< double, 4, 4, ::Eigen::RowMajor> mat;

    mat = ::eigenTools::helper::toEigen(mat16);

    for(unsigned int r = 0; r < 4; ++r)
    {
        for(unsigned int c = 0; c < 4; ++c)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Matrices differ at ("+std::to_string(r)+" , "+std::to_string(c)+")",
                                                 mat16[4*r+c], mat(r, c), 0.00000000001);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace eigenTools
