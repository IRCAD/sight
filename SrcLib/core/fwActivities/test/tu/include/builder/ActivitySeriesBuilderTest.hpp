/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWACTIVITIES_UT_BUILDER_ACTIVITYSERIESBUILDERTEST_HPP__
#define __FWACTIVITIES_UT_BUILDER_ACTIVITYSERIESBUILDERTEST_HPP__

#include <fwActivities/registry/Activities.hpp>

#include <fwCore/macros.hpp>

#include <fwRuntime/Bundle.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwActivities
{

namespace ut
{

class ActivitySeriesBuilderTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ActivitySeriesBuilderTest );

CPPUNIT_TEST(builDataTest);

CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void builDataTest();

protected:
    SPTR( ::fwRuntime::Bundle ) m_bundle;
    SPTR( ::fwActivities::registry::Activities ) m_activities;

};

} //namespace ut
} //namespace fwActivities

#endif //__FWACTIVITIES_UT_BUILDER_ACTIVITYSERIESBUILDERTEST_HPP__

