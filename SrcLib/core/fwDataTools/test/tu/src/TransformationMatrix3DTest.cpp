/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>
#include <cmath>
#include <fwTools/Type.hpp>

#include <fwComEd/helper/Array.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include "TransformationMatrix3DTest.hpp"

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

bool isIdentity(::fwData::TransformationMatrix3D::sptr tm)
{
    bool isIdentity = true;

    for (size_t i = 0; i<4; ++i)
    {
        for (size_t j = 0; j<4; ++j)
        {
            isIdentity &= (i==j ? 1. : 0.) == tm->getCoefficient(i,j);
        }
    }
    return isIdentity;
}

void TransformationMatrix3DTest::identityMatrixTest()
{
    ::fwData::TransformationMatrix3D::sptr tm1 = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr tm2 = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr tm3 = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr tm4 = ::fwData::TransformationMatrix3D::New();

    ::fwData::Point::sptr p1 = ::fwData::Point::New(1.0f, 2.3f, 5.1f);
    ::fwData::Point::sptr p2 = ::fwData::Point::New();

    tm1->setCoefficient(0, 2, 3.4);
    tm1->setCoefficient(1, 3, 18);
    ::fwDataTools::TransformationMatrix3D::identity(tm1);
    ::fwDataTools::TransformationMatrix3D::identity(tm2);
    CPPUNIT_ASSERT(isIdentity(tm1));
    CPPUNIT_ASSERT(isIdentity(tm2));

    ::fwDataTools::TransformationMatrix3D::multiply(tm1, tm2, tm3);
    CPPUNIT_ASSERT(isIdentity(tm3));

    ::fwDataTools::TransformationMatrix3D::invert(tm1, tm4);
    CPPUNIT_ASSERT(isIdentity(tm4));

    ::fwDataTools::TransformationMatrix3D::multiply(tm1, p1, p2);
    CPPUNIT_ASSERT_EQUAL(p1->getCoord()[0], p2->getCoord()[0]);
    CPPUNIT_ASSERT_EQUAL(p1->getCoord()[1], p2->getCoord()[1]);
    CPPUNIT_ASSERT_EQUAL(p1->getCoord()[2], p2->getCoord()[2]);
}

void TransformationMatrix3DTest::matrixTest()
{
    ::fwData::TransformationMatrix3D::sptr tm1 = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr tm2 = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr tm3 = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr tm4 = ::fwData::TransformationMatrix3D::New();

    ::fwDataTools::TransformationMatrix3D::identity(tm2);

    ::fwData::TransformationMatrix3D::TMCoefArray tm1Coefs(16);
    for (int i = 0; i<16; ++i)
    {
        tm1Coefs[i] = i+1;
    }

    for (int i = 0; i<4; ++i)
    {
        for (int j = 0; j<4; ++j)
        {
            tm2->setCoefficient(i,j, fabs(static_cast<double>(i-j)) + 1);
        }
    }
    tm1->setCoefficients(tm1Coefs);
    tm4->setCoefficients(tm1Coefs);

    ::fwData::Point::sptr p1 = ::fwData::Point::New(1.0f, 2.3f, 5.1f);
    ::fwData::Point::sptr p2 = ::fwData::Point::New();

    ::fwDataTools::TransformationMatrix3D::multiply(tm1, tm2, tm3);
    for (size_t i = 0; i<4; ++i)
    {
        for (size_t j = 0; j<4; ++j)
        {
            double val = 0;
            for (size_t k = 0; k<4; ++k)
            {
                val += tm1->getCoefficient(i,k)*tm2->getCoefficient(k,j);
            }
            CPPUNIT_ASSERT_EQUAL(val, tm3->getCoefficient(i,j));
        }
    }

    ::fwDataTools::TransformationMatrix3D::invert(tm2, tm4);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.4, tm4->getCoefficient(0,0), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,  tm4->getCoefficient(0,1), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.,   tm4->getCoefficient(0,2), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.1,  tm4->getCoefficient(0,3), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,  tm4->getCoefficient(1,0), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.,  tm4->getCoefficient(1,1), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,  tm4->getCoefficient(1,2), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.,   tm4->getCoefficient(1,3), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.,   tm4->getCoefficient(2,0), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,  tm4->getCoefficient(2,1), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.,  tm4->getCoefficient(2,2), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,  tm4->getCoefficient(2,3), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.1,  tm4->getCoefficient(3,0), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.,   tm4->getCoefficient(3,1), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,  tm4->getCoefficient(3,2), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.4, tm4->getCoefficient(3,3), 0.00001);

    ::fwDataTools::TransformationMatrix3D::multiply(tm1, p1, p2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(24.9,  p2->getCoord()[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(62.5,  p2->getCoord()[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(100.1, p2->getCoord()[2], 0.00001);

    ::fwDataTools::TransformationMatrix3D::identity(tm1);
    ::fwDataTools::TransformationMatrix3D::identity(tm2);
    CPPUNIT_ASSERT(isIdentity(tm1));
    CPPUNIT_ASSERT(isIdentity(tm2));
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwDataTools
