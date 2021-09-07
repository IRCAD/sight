/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ActivitySeriesTest.hpp"

#include <data/Composite.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::ActivitySeriesTest);

namespace sight::data
{

namespace ut
{

//------------------------------------------------------------------------------

void ActivitySeriesTest::setUp()
{
    // Set up context before running a test.
    m_series = data::ActivitySeries::New();
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
    const data::ActivitySeries::ConfigIdType activityConfigId = "Visu2D";
    CPPUNIT_ASSERT(m_series);
    m_series->setActivityConfigId(activityConfigId);
    CPPUNIT_ASSERT_EQUAL(activityConfigId, m_series->getActivityConfigId());
}

//------------------------------------------------------------------------------

void ActivitySeriesTest::dataTest()
{
    data::Composite::sptr data = data::Composite::New();
    CPPUNIT_ASSERT(m_series);
    CPPUNIT_ASSERT(data);
    m_series->setData(data);
    CPPUNIT_ASSERT_EQUAL(data, m_series->getData());
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::data
