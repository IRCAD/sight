/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "TransformationMatrix3DTest.hpp"

#include <fwDataTools/helper/Array.hpp>

#define FW_PROFILING_DISABLED
#include <fwCore/Profiling.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <sstream>
#include <cmath>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataTools::ut::TransformationMatrix3DTest );

namespace fwDataTools
{
namespace ut
{

//------------------------------------------------------------------------------

void TransformationMatrix3DTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void TransformationMatrix3DTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void TransformationMatrix3DTest::identityMatrixTest()
{
    ::fwData::TransformationMatrix3D::sptr tm1 = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr tm2 = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr tm3 = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr tm4 = ::fwData::TransformationMatrix3D::New();

    ::fwData::Point::sptr p1 = ::fwData::Point::New(1.0f, 2.3f, 5.1f);
    ::fwData::Point::sptr p2 = ::fwData::Point::New();

    //test identity
    bool shouldBeTrue = ::fwDataTools::TransformationMatrix3D::isIdentity(tm1);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("TransformationMatrix3D should be identity", true, shouldBeTrue);
    // dummy precision
    shouldBeTrue = ::fwDataTools::TransformationMatrix3D::isIdentity(tm1, 1);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("TransformationMatrix3D should be identity", true, shouldBeTrue);
    // high precision
    shouldBeTrue = ::fwDataTools::TransformationMatrix3D::isIdentity(tm1, 1e-17);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("TransformationMatrix3D should be identity", true, shouldBeTrue);

    tm1->setCoefficient(0, 2, 3.4);
    tm1->setCoefficient(1, 3, 18);

    bool shouldBeFalse = ::fwDataTools::TransformationMatrix3D::isIdentity(tm1);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("TransformationMatrix3D should not be identity", false, shouldBeFalse);
    //Test with dummy precision
    shouldBeTrue = ::fwDataTools::TransformationMatrix3D::isIdentity(tm1, 20);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("TransformationMatrix3D should be identity with a precision of 20", true,
                                 shouldBeTrue);
    //Test with nice precision
    shouldBeFalse = ::fwDataTools::TransformationMatrix3D::isIdentity(tm1, 1e-14);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("TransformationMatrix3D should not be identity", false, shouldBeFalse);

    ::fwDataTools::TransformationMatrix3D::identity(tm1);
    ::fwDataTools::TransformationMatrix3D::identity(tm2);
    CPPUNIT_ASSERT( ::fwDataTools::TransformationMatrix3D::isIdentity(tm1));
    CPPUNIT_ASSERT( ::fwDataTools::TransformationMatrix3D::isIdentity(tm2));

#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::fwDataTools::TransformationMatrix3D::multiply");
        for(int i = 0; i < 1000000; ++i)
        {
            ::fwDataTools::TransformationMatrix3D::multiply(tm1, tm2, tm3);
        }
    }
#else
    ::fwDataTools::TransformationMatrix3D::multiply(tm1, tm2, tm3);
#endif
    CPPUNIT_ASSERT( ::fwDataTools::TransformationMatrix3D::isIdentity(tm3));

#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::fwDataTools::TransformationMatrix3D::invert");
        for(int i = 0; i < 1000000; ++i)
        {
            ::fwDataTools::TransformationMatrix3D::invert(tm1, tm4);
        }
    }
#else
    ::fwDataTools::TransformationMatrix3D::invert(tm1, tm4);
#endif
    CPPUNIT_ASSERT( ::fwDataTools::TransformationMatrix3D::isIdentity(tm4));

#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::fwDataTools::TransformationMatrix3D::multiply");
        for(int i = 0; i < 1000000; ++i)
        {
            ::fwDataTools::TransformationMatrix3D::multiply(tm1, p1, p2);
        }
    }
#else
    ::fwDataTools::TransformationMatrix3D::multiply(tm1, p1, p2);
#endif
    CPPUNIT_ASSERT_EQUAL(p1->getCoord()[0], p2->getCoord()[0]);
    CPPUNIT_ASSERT_EQUAL(p1->getCoord()[1], p2->getCoord()[1]);
    CPPUNIT_ASSERT_EQUAL(p1->getCoord()[2], p2->getCoord()[2]);
}

//------------------------------------------------------------------------------

void TransformationMatrix3DTest::matrixTest()
{
    ::fwData::TransformationMatrix3D::sptr tm1 = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr tm2 = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr tm3 = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr tm4 = ::fwData::TransformationMatrix3D::New();

    ::fwDataTools::TransformationMatrix3D::identity(tm2);

    ::fwData::TransformationMatrix3D::TMCoefArray tm1Coefs;
    for (int i = 0; i < 16; ++i)
    {
        tm1Coefs[i] = i+1;
    }

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            tm2->setCoefficient(i, j, fabs(static_cast<double>(i-j)) + 1);
        }
    }
    tm1->setCoefficients(tm1Coefs);
    tm4->setCoefficients(tm1Coefs);

    // Test matrix-matrix multiplication
    ::fwDataTools::TransformationMatrix3D::multiply(tm1, tm2, tm3);
    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            double val = 0;
            for (size_t k = 0; k < 4; ++k)
            {
                val += tm1->getCoefficient(i, k)*tm2->getCoefficient(k, j);
            }
            CPPUNIT_ASSERT_EQUAL(val, tm3->getCoefficient(i, j));
        }
    }

    // Test invert
    ::fwDataTools::TransformationMatrix3D::invert(tm2, tm4);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.4, tm4->getCoefficient(0, 0), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,  tm4->getCoefficient(0, 1), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.,   tm4->getCoefficient(0, 2), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.1,  tm4->getCoefficient(0, 3), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,  tm4->getCoefficient(1, 0), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.,  tm4->getCoefficient(1, 1), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,  tm4->getCoefficient(1, 2), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.,   tm4->getCoefficient(1, 3), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.,   tm4->getCoefficient(2, 0), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,  tm4->getCoefficient(2, 1), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.,  tm4->getCoefficient(2, 2), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,  tm4->getCoefficient(2, 3), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.1,  tm4->getCoefficient(3, 0), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.,   tm4->getCoefficient(3, 1), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,  tm4->getCoefficient(3, 2), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.4, tm4->getCoefficient(3, 3), 0.00001);

    ::fwData::TransformationMatrix3D::TMCoefArray tm2Coefs = { 3.1, 1., -7.9689, 4.9,
                                                               5., -21., -1.3646, 14.4,
                                                               9., -7.2, -23.36, 79.04,
                                                               0.1, -3., -1.234, -49.94 };
    tm2->setCoefficients(tm2Coefs);
    ::fwDataTools::TransformationMatrix3D::invert(tm2, tm4);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.885131908604589, tm4->getCoefficient(0, 0), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.190212289294002, tm4->getCoefficient(0, 1), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.29581348830958, tm4->getCoefficient(0, 2), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.326489683580158, tm4->getCoefficient(0, 3), 0.0001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.185258037237573, tm4->getCoefficient(1, 0), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.00768780306376934, tm4->getCoefficient(1, 1), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.0586854220133396, tm4->getCoefficient(1, 2), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.0769210199757416, tm4->getCoefficient(1, 3), 0.0001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.232796743379289, tm4->getCoefficient(2, 0), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0724476129133489, tm4->getCoefficient(2, 1), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.118830425964518, tm4->getCoefficient(2, 2), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.144341353618837, tm4->getCoefficient(2, 3), 0.0001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.015108772570731, tm4->getCoefficient(3, 0), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.000947451265806253, tm4->getCoefficient(3, 1), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.00586927638865189, tm4->getCoefficient(3, 2), 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.0124903619956978, tm4->getCoefficient(3, 3), 0.0001);

    // Test matrix-vector multiplication
    ::fwData::Point::sptr p1 = ::fwData::Point::New(1.0f, 2.3f, 5.1f);
    ::fwData::Point::sptr p2 = ::fwData::Point::New();

    ::fwDataTools::TransformationMatrix3D::multiply(tm1, p1, p2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(24.9,  p2->getCoord()[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(62.5,  p2->getCoord()[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(100.1, p2->getCoord()[2], 0.00001);

    ::fwDataTools::TransformationMatrix3D::identity(tm1);
    ::fwDataTools::TransformationMatrix3D::identity(tm2);
    CPPUNIT_ASSERT( ::fwDataTools::TransformationMatrix3D::isIdentity(tm1));
    CPPUNIT_ASSERT( ::fwDataTools::TransformationMatrix3D::isIdentity(tm2));
}

//------------------------------------------------------------------------------

void TransformationMatrix3DTest::glmGetterSetterTest()
{
    ::fwData::TransformationMatrix3D::TMCoefArray coefs = { 2, -2, .3, .12,
                                                            4, 8.9, 4.2, 1.2,
                                                            7.8, -12.1, 2.3, 1.2,
                                                            .3, 1.21, -3.1, 1.2};

    ::fwData::TransformationMatrix3D::sptr mat = ::fwData::TransformationMatrix3D::New();
    mat->setCoefficients(coefs);

    // Test getter
    ::glm::dmat4x4 glmMat;

#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::fwDataTools::TransformationMatrix3D::getMatrixFromTF3D");
        for(int i = 0; i < 1000000; ++i)
        {
            glmMat = ::fwDataTools::TransformationMatrix3D::getMatrixFromTF3D(mat);
        }
    }
#else
    glmMat = ::fwDataTools::TransformationMatrix3D::getMatrixFromTF3D(mat);
#endif

    // TransformationMatrix3D is stored row-major
    // glm matrices are stored column-major
    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(glmMat[i][j], coefs[i + j * 4]);
        }
    }

    // Test setter
    double coefs2[] = { 11, -2, -.3, -.74,
                        .214, 82.9, 9.2, -5.2,
                        17.8, -2.1, 2.3, 1.2,
                        .13, 0.1, -0.1, 0.2};

    glmMat = ::glm::make_mat4<double>(coefs2);
#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::fwDataTools::TransformationMatrix3D::setTF3DFromMatrix");
        for(int i = 0; i < 1000000; ++i)
        {
            ::fwDataTools::TransformationMatrix3D::setTF3DFromMatrix(mat, glmMat);
        }
    }
#else
    ::fwDataTools::TransformationMatrix3D::setTF3DFromMatrix(mat, glmMat);
#endif

    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(mat->getCoefficient(i, j), coefs2[i + j * 4]);
        }
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwDataTools
