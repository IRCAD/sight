/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
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

#include "ProjectionTest.hpp"

#include <depthMapOp/Projection.hpp>

#include <cmath>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::depthMapOp::ut::ProjectionTest );

namespace depthMapOp
{
namespace ut
{

//------------------------------------------------------------------------------

void ProjectionTest::setUp()
{
}

//------------------------------------------------------------------------------

void ProjectionTest::tearDown()
{
}

//------------------------------------------------------------------------------

void ProjectionTest::projectionTest()
{
    {
        const size_t pX    = 42;
        const size_t pY    = 53;
        const size_t depth = 155;

        const size_t width  = 640;
        const size_t height = 480;
        const double cx     = 321.3;
        const double cy     = 239.3;
        const double fx     = 565.53;
        const double fy     = 563.25;

        double x, y, z;
        ::depthMapOp::Projection::projectPixel(pX, pY, static_cast<double>(depth), cx, cy, fx, fy, x, y, z);
        size_t pX2, pY2;
        bool success = ::depthMapOp::Projection::projectPoint(x, y, z, cx, cy, fx, fy, width, height, pX2, pY2);

        CPPUNIT_ASSERT(success);
        CPPUNIT_ASSERT_EQUAL(pX, pX2);
        CPPUNIT_ASSERT_EQUAL(pY, pY2);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(depth* (pX-cx) / fx, x, 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(depth* (pY-cy)/ fy, y, 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(double(depth), z, 0.0001);
    }

    {
        const double x = 63.45;
        const double y = 25.4;
        const double z = 156;

        const size_t width  = 640;
        const size_t height = 480;
        const double cx     = 321.3;
        const double cy     = 239.3;
        const double fx     = 565.53;
        const double fy     = 563.25;

        size_t pX, pY;
        bool success = ::depthMapOp::Projection::projectPoint(x, y, z, cx, cy, fx, fy, width, height, pX, pY);
        CPPUNIT_ASSERT(success);
        double x2, y2, z2;
        ::depthMapOp::Projection::projectPixel(pX, pY, z, cx, cy, fx, fy, x2, y2, z2);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(x, x2, 0.1);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(y, y2, 0.1);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(z, z2, 0.001);
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace registrationOp
