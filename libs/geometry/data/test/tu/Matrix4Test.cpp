/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "Matrix4Test.hpp"

#define FW_PROFILING_DISABLED
#include <core/Profiling.hpp>

#include <geometry/data/Matrix4.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <sstream>
#include <cmath>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::geometry::data::ut::Matrix4Test);

namespace sight::geometry::data
{

namespace ut
{

//------------------------------------------------------------------------------

void Matrix4Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void Matrix4Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void Matrix4Test::identityMatrixTest()
{
    sight::data::Matrix4 tm1;
    sight::data::Matrix4 tm2;
    sight::data::Matrix4 tm3;
    sight::data::Matrix4 tm4;

    auto p1 = sight::data::Point::New(1.0f, 2.3f, 5.1f);
    auto p2 = sight::data::Point::New();

    bool shouldBeTrue = geometry::data::isIdentity(tm1);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Matrix4 should be identity", true, shouldBeTrue);
    // dummy precision
    shouldBeTrue = geometry::data::isIdentity(tm1, 1);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Matrix4 should be identity", true, shouldBeTrue);
    // high precision
    shouldBeTrue = geometry::data::isIdentity(tm1, 1e-17);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Matrix4 should be identity", true, shouldBeTrue);

    tm1.setCoefficient(0, 2, 3.4);
    tm1.setCoefficient(1, 3, 18);

    bool shouldBeFalse = geometry::data::isIdentity(tm1);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Matrix4 should not be identity", false, shouldBeFalse);
    //Test with dummy precision
    shouldBeTrue = geometry::data::isIdentity(tm1, 20);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Matrix4 should be identity with a precision of 20",
        true,
        shouldBeTrue
    );
    //Test with nice precision
    shouldBeFalse = geometry::data::isIdentity(tm1, 1e-14);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Matrix4 should not be identity", false, shouldBeFalse);

    geometry::data::identity(tm1);
    geometry::data::identity(tm2);
    CPPUNIT_ASSERT(geometry::data::isIdentity(tm1));
    CPPUNIT_ASSERT(geometry::data::isIdentity(tm2));

#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::geometry::data::multiply");
        for(int i = 0 ; i < 1000000 ; ++i)
        {
            geometry::data::multiply(tm1, tm2, tm3);
        }
    }
#else
    geometry::data::multiply(tm1, tm2, tm3);
#endif
    CPPUNIT_ASSERT(geometry::data::isIdentity(tm3));

#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::geometry::data::invert");
        for(int i = 0 ; i < 1000000 ; ++i)
        {
            geometry::data::invert(tm1, tm4);
        }
    }
#else
    geometry::data::invert(tm1, tm4);
#endif
    CPPUNIT_ASSERT(geometry::data::isIdentity(tm4));

#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::geometry::data::multiply");
        for(int i = 0 ; i < 1000000 ; ++i)
        {
            geometry::data::multiply(tm1, p1, p2);
        }
    }
#else
    geometry::data::multiply(tm1, *p1, *p2);
#endif
    CPPUNIT_ASSERT_EQUAL(p1->getCoord()[0], p2->getCoord()[0]);
    CPPUNIT_ASSERT_EQUAL(p1->getCoord()[1], p2->getCoord()[1]);
    CPPUNIT_ASSERT_EQUAL(p1->getCoord()[2], p2->getCoord()[2]);
}

//------------------------------------------------------------------------------

void Matrix4Test::matrixTest()
{
    sight::data::Matrix4 tm1;
    sight::data::Matrix4 tm2;
    sight::data::Matrix4 tm3;
    sight::data::Matrix4 tm4;

    geometry::data::identity(tm2);

    sight::data::Matrix4::TMCoefArray tm1Coefs;
    for(int i = 0 ; i < 16 ; ++i)
    {
        tm1Coefs[i] = i + 1;
    }

    for(int i = 0 ; i < 4 ; ++i)
    {
        for(int j = 0 ; j < 4 ; ++j)
        {
            tm2.setCoefficient(i, j, fabs(static_cast<double>(i - j)) + 1);
        }
    }

    tm1.setCoefficients(tm1Coefs);
    tm4.setCoefficients(tm1Coefs);

    // Test matrix-matrix multiplication
    geometry::data::multiply(tm1, tm2, tm3);
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            double val = 0;
            for(std::size_t k = 0 ; k < 4 ; ++k)
            {
                val += tm1.getCoefficient(i, k) * tm2.getCoefficient(k, j);
            }

            CPPUNIT_ASSERT_EQUAL(val, tm3.getCoefficient(i, j));
        }
    }

    // Test invert
    geometry::data::invert(tm2, tm4);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.4, tm4.getCoefficient(0, 0), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, tm4.getCoefficient(0, 1), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., tm4.getCoefficient(0, 2), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.1, tm4.getCoefficient(0, 3), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, tm4.getCoefficient(1, 0), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-1., tm4.getCoefficient(1, 1), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, tm4.getCoefficient(1, 2), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., tm4.getCoefficient(1, 3), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., tm4.getCoefficient(2, 0), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, tm4.getCoefficient(2, 1), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-1., tm4.getCoefficient(2, 2), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, tm4.getCoefficient(2, 3), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.1, tm4.getCoefficient(3, 0), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., tm4.getCoefficient(3, 1), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, tm4.getCoefficient(3, 2), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.4, tm4.getCoefficient(3, 3), 0.00001);

    sight::data::Matrix4::TMCoefArray tm2Coefs = {3.1, 1., -7.9689, 4.9,
                                                  5., -21., -1.3646, 14.4,
                                                  9., -7.2, -23.36, 79.04,
                                                  0.1, -3., -1.234, -49.94
    };
    tm2.setCoefficients(tm2Coefs);
    geometry::data::invert(tm2, tm4);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.885131908604589, tm4.getCoefficient(0, 0), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.190212289294002, tm4.getCoefficient(0, 1), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.29581348830958, tm4.getCoefficient(0, 2), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.326489683580158, tm4.getCoefficient(0, 3), 0.0001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.185258037237573, tm4.getCoefficient(1, 0), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.00768780306376934, tm4.getCoefficient(1, 1), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.0586854220133396, tm4.getCoefficient(1, 2), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.0769210199757416, tm4.getCoefficient(1, 3), 0.0001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.232796743379289, tm4.getCoefficient(2, 0), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0724476129133489, tm4.getCoefficient(2, 1), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.118830425964518, tm4.getCoefficient(2, 2), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.144341353618837, tm4.getCoefficient(2, 3), 0.0001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.015108772570731, tm4.getCoefficient(3, 0), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.000947451265806253, tm4.getCoefficient(3, 1), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.00586927638865189, tm4.getCoefficient(3, 2), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.0124903619956978, tm4.getCoefficient(3, 3), 0.0001);

    // Test matrix-vector multiplication
    auto p1 = sight::data::Point::New(1.0f, 2.3f, 5.1f);
    auto p2 = sight::data::Point::New();

    geometry::data::multiply(tm1, *p1, *p2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(24.9, p2->getCoord()[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(62.5, p2->getCoord()[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(100.1, p2->getCoord()[2], 0.00001);

    geometry::data::identity(tm1);
    geometry::data::identity(tm2);
    CPPUNIT_ASSERT(geometry::data::isIdentity(tm1));
    CPPUNIT_ASSERT(geometry::data::isIdentity(tm2));
}

//------------------------------------------------------------------------------

void Matrix4Test::glmGetterSetterTest()
{
    sight::data::Matrix4::TMCoefArray coefs = {2, -2, .3, .12,
                                               4, 8.9, 4.2, 1.2,
                                               7.8, -12.1, 2.3, 1.2,
                                               .3, 1.21, -3.1, 1.2
    };

    sight::data::Matrix4 mat;
    mat.setCoefficients(coefs);

    // Test getter
    glm::dmat4x4 glmMat;

#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::geometry::data::getMatrixFromTF3D");
        for(int i = 0 ; i < 1000000 ; ++i)
        {
            glmMat = geometry::data::getMatrixFromTF3D(mat);
        }
    }
#else
    glmMat = geometry::data::getMatrixFromTF3D(mat);
#endif

    // Matrix4 is stored row-major
    // glm matrices are stored column-major
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(glmMat[i][j], coefs[i + j * 4]);
        }
    }

    // Test setter
    double coefs2[] = {11, -2, -.3, -.74,
                       .214, 82.9, 9.2, -5.2,
                       17.8, -2.1, 2.3, 1.2,
                       .13, 0.1, -0.1, 0.2
    };

    glmMat = glm::make_mat4<double>(coefs2);
#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::geometry::data::setTF3DFromMatrix");
        for(int i = 0 ; i < 1000000 ; ++i)
        {
            geometry::data::setTF3DFromMatrix(mat, glmMat);
        }
    }
#else
    geometry::data::setTF3DFromMatrix(mat, glmMat);
#endif

    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(mat.getCoefficient(i, j), coefs2[i + j * 4]);
        }
    }
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::geometry::data
