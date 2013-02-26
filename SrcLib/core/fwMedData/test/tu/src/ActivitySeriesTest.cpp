/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Composite.hpp>

#include <fwMedData/ActivitySeries.hpp>

#include "ActivitySeriesTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedData::ut::ActivitySeriesTest );

namespace fwMedData
{
namespace ut
{

//------------------------------------------------------------------------------

void ActivitySeriesTest::setUp()
{
    // Set up context before running a test.
    m_series = ::fwMedData::ActivitySeries::New();
}

//------------------------------------------------------------------------------

void ActivitySeriesTest::tearDown()
{
    // Clean up after the test run.
    m_series.reset();
}

//------------------------------------------------------------------------------

void ActivitySeriesTest::activityConfigIdTest()
{
    const ::fwMedData::ActivitySeries::ConfigIdType activityConfigId = "Visu2D";
    CPPUNIT_ASSERT(m_series);
    m_series->setActivityConfigId(activityConfigId);
    CPPUNIT_ASSERT_EQUAL(activityConfigId, m_series->getActivityConfigId());
}

//------------------------------------------------------------------------------

void ActivitySeriesTest::dataTest()
{
    ::fwData::Composite::sptr data = ::fwData::Composite::New();
    CPPUNIT_ASSERT(m_series);
    CPPUNIT_ASSERT(data);
    m_series->setData(data);
    CPPUNIT_ASSERT_EQUAL(data, m_series->getData());
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMedData
