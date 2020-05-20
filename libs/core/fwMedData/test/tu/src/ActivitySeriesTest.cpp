/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "ActivitySeriesTest.hpp"

#include <fwData/Composite.hpp>

#include <fwDataCamp/Version.hpp>

#include <fwMedDataCamp/Version.hpp>

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

    //Force link with fwDataCamp
    m_fwDataVersion = ::fwDataCamp::Version::s_CURRENT_VERSION;
    FwCoreNotUsedMacro(m_fwDataVersion);

    //Force link with fwMedDataCamp
    m_fwMedDataVersion = ::fwMedDataCamp::Version::s_CURRENT_VERSION;
    FwCoreNotUsedMacro(m_fwMedDataVersion);
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
