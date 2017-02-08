/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
