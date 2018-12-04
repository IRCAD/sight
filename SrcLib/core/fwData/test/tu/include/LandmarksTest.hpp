/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#ifndef __FWDATA_UT_LANDMARKSTEST_HPP__
#define __FWDATA_UT_LANDMARKSTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwData
{
namespace ut
{
/**
 * @brief The PointListTest class
 * This class is used to test ::fwData::PointList
 */
class LandmarksTest : public CPPUNIT_NS::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( LandmarksTest );
    CPPUNIT_TEST( copyTest );
    CPPUNIT_TEST( groupTest );
    CPPUNIT_TEST( pointsTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    /// test deep and shallow copy
    void copyTest();

    /// test to add/remove groups and change properties
    void groupTest();

    /// test to add/ remove points in group
    void pointsTest();

};

} //namespace ut
} //namespace fwData
#endif //__FWDATA_UT_LANDMARKSTEST_HPP__
