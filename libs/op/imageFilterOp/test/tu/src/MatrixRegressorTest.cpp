/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "MatrixRegressorTest.hpp"

#include <imageFilterOp/MatrixRegressor.hpp>

#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::imageFilterOp::ut::MatrixRegressorTest );

namespace imageFilterOp
{
namespace ut
{

//------------------------------------------------------------------------------

void MatrixRegressorTest::setUp()
{
}

//------------------------------------------------------------------------------

void MatrixRegressorTest::tearDown()
{
}

//------------------------------------------------------------------------------

void MatrixRegressorTest::identityTest()
{
    ::fwData::TransformationMatrix3D::sptr id = ::fwData::TransformationMatrix3D::New();

    ::fwData::Vector::sptr matList = ::fwData::Vector::New();

    for(int i = 0; i < 5; ++i)
    {
        matList->getContainer().push_back(id);
    }

    const std::vector< MatrixRegressor::PointType > ptList = {{
        { 0., 0., 0., 1. },
        { 0., 0., 1., 1. },
        { 0., 1., 0., 1. },
        { 0., 1., 1., 1. },
        { 1., 0., 0., 1. },
        { 1., 0., 1., 1. },
        { 1., 1., 0., 1. },
        { 1., 1., 1., 1. }
    }};

    MatrixRegressor regressor(matList, ptList);

    ::fwData::TransformationMatrix3D::sptr res = regressor.minimize(id);

    for(int i = 0; i < 16; ++i)
    {
        const double expected = id->getCoefficients()[i];
        const double result   = res->getCoefficients()[i];

        CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, result, 1e-3);
    }
}

//------------------------------------------------------------------------------

void MatrixRegressorTest::avgTranslationTest()
{
    ::fwData::TransformationMatrix3D::sptr id     = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr trans1 = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr trans2 = ::fwData::TransformationMatrix3D::New();

    ::glm::dmat4 t1 = ::glm::translate(::glm::dmat4(1.), ::glm::dvec3(3, 3, 3));
    ::glm::dmat4 t2 = ::glm::translate(::glm::dmat4(1.), ::glm::dvec3(5, 5, 5));

    ::fwDataTools::TransformationMatrix3D::setTF3DFromMatrix(trans1, t1);
    ::fwDataTools::TransformationMatrix3D::setTF3DFromMatrix(trans2, t2);

    ::fwData::Vector::sptr matList = ::fwData::Vector::New();

    matList->getContainer().push_back(trans1);
    matList->getContainer().push_back(trans2);

    const std::vector< MatrixRegressor::PointType > ptList = {{
        { 0., 0., 0., 1. },
        { 0., 0., 1., 1. },
        { 0., 1., 0., 1. },
        { 0., 1., 1., 1. },
        { 1., 0., 0., 1. },
        { 1., 0., 1., 1. },
        { 1., 1., 0., 1. },
        { 1., 1., 1., 1. }
    }};

    MatrixRegressor regressor(matList, ptList);

    ::fwData::TransformationMatrix3D::sptr res = regressor.minimize(id);

    const ::glm::dmat4 transExpected = ::glm::translate(::glm::dmat4(1.), ::glm::dvec3(4, 4, 4));
    ::fwData::TransformationMatrix3D::sptr expectedMat = ::fwData::TransformationMatrix3D::New();

    ::fwDataTools::TransformationMatrix3D::setTF3DFromMatrix(expectedMat, transExpected);

    for(int i = 0; i < 16; ++i)
    {
        const double expected = expectedMat->getCoefficients()[i];
        const double result   = res->getCoefficients()[i];

        CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, result, 1e-3);
    }

}

//------------------------------------------------------------------------------

void MatrixRegressorTest::avgRotationTest()
{
    ::fwData::TransformationMatrix3D::sptr id  = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr rot = ::fwData::TransformationMatrix3D::New();

    ::glm::dmat4 r1 = ::glm::rotate(::glm::dmat4(1.), ::glm::pi<double>() / 2., ::glm::dvec3(0., 0., 1.));

    ::fwDataTools::TransformationMatrix3D::setTF3DFromMatrix(rot, r1);

    ::fwData::Vector::sptr matList = ::fwData::Vector::New();

    matList->getContainer().push_back(id);
    matList->getContainer().push_back(rot);

    const std::vector< MatrixRegressor::PointType > ptList = {{
        { 0., 0., 0., 1. },
        { 0., 0., 1., 1. },
        { 0., 1., 0., 1. },
        { 0., 1., 1., 1. },
        { 1., 0., 0., 1. },
        { 1., 0., 1., 1. },
        { 1., 1., 0., 1. },
        { 1., 1., 1., 1. }
    }};

    MatrixRegressor regressor(matList, ptList);

    ::fwData::TransformationMatrix3D::sptr res = regressor.minimize(id, 1., 1e-5, 1e-5);

    ::glm::dmat4 glmRes = ::fwDataTools::TransformationMatrix3D::getMatrixFromTF3D(res);

    // Extract the rotation from the result.
    double scale = std::pow(::glm::determinant(glmRes), 1./3.);

    // Remove the scale from the matrix. This is required by the ::glm::toQuat() function.
    ::glm::dvec3 angles = ::glm::eulerAngles(::glm::toQuat(glmRes / scale));

    CPPUNIT_ASSERT(::glm::all( ::glm::epsilonEqual(angles, ::glm::dvec3(0., 0., ::glm::pi<double>() / 4.), 1e-3)));
}

//------------------------------------------------------------------------------

} //namespace ut.
} //namespace imageFilterOp.
