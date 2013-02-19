/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWACTIVITIES_UT_ACTIVITYSERIESTEST_HPP__
#define __FWACTIVITIES_UT_ACTIVITYSERIESTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwActivities
{
    class ActivitySeries;

namespace ut
{

class ActivitySeriesTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ActivitySeriesTest );

    CPPUNIT_TEST(activityConfigIdTest);
    CPPUNIT_TEST(dataTest);

    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void activityConfigIdTest();
    void dataTest();

protected:

    SPTR(::fwActivities::ActivitySeries) m_series;

};

} //namespace ut
} //namespace fwActivities

#endif //__FWACTIVITIES_UT_ACTIVITYSERIESTEST_HPP__

