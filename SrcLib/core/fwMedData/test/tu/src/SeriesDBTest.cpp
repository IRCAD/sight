/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "fwMedData/ModelSeries.hpp"

#include "SeriesDBTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedData::ut::SeriesDBTest );

namespace fwMedData
{
namespace ut
{

//------------------------------------------------------------------------------

void SeriesDBTest::setUp()
{
    // Set up context before running a test.
    m_series = ::fwMedData::SeriesDB::New();
}

//------------------------------------------------------------------------------

void SeriesDBTest::tearDown()
{
    // Clean up after the test run.
    m_series->getContainer().clear();
}

//------------------------------------------------------------------------------

void SeriesDBTest::containerTest()
{
    fwMedData::Series::sptr series1 = fwMedData::ModelSeries::New();
    fwMedData::Series::sptr series2 = fwMedData::ModelSeries::New();
    fwMedData::Series::sptr series3 = fwMedData::ModelSeries::New();

    CPPUNIT_ASSERT( m_series->empty() );

    m_series->getContainer().push_back(series1);
    m_series->getContainer().push_back(series2);
    m_series->getContainer().push_back(series3);

    CPPUNIT_ASSERT_EQUAL( static_cast<size_t> (3), m_series->size() );
    CPPUNIT_ASSERT_EQUAL( series1, (*m_series)[0] );
    CPPUNIT_ASSERT_EQUAL( series2, (*m_series)[1] );
    CPPUNIT_ASSERT_EQUAL( series3, (*m_series)[2] );

    m_series->getContainer().clear();
    CPPUNIT_ASSERT_EQUAL( static_cast<size_t> (0), m_series->size() );
    CPPUNIT_ASSERT( m_series->empty() );




    ::fwMedData::SeriesDB::ContainerType series;

    series.push_back(series1);
    series.push_back(series2);
    series.push_back(series3);

    m_series->setContainer(series);

    CPPUNIT_ASSERT_EQUAL( static_cast<size_t> (3), m_series->size() );
    CPPUNIT_ASSERT_EQUAL( series1, (*m_series)[0] );
    CPPUNIT_ASSERT_EQUAL( series2, (*m_series)[1] );
    CPPUNIT_ASSERT_EQUAL( series3, (*m_series)[2] );

    m_series->getContainer().clear();
    CPPUNIT_ASSERT_EQUAL( static_cast<size_t> (0), m_series->size() );
    CPPUNIT_ASSERT( m_series->empty() );


    CPPUNIT_ASSERT_EQUAL( static_cast<size_t> (3), series.size() );
    CPPUNIT_ASSERT_EQUAL( series1, series[0] );
    CPPUNIT_ASSERT_EQUAL( series2, series[1] );
    CPPUNIT_ASSERT_EQUAL( series3, series[2] );

}

//------------------------------------------------------------------------------



} //namespace ut
} //namespace fwMedData
