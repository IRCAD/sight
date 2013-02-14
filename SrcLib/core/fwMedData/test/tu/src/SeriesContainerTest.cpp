/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMedData/SeriesContainer.hpp"
#include "fwMedData/Series.hpp"

#include "SeriesContainerTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedData::ut::SeriesContainerTest );

namespace fwMedData
{
namespace ut
{

//------------------------------------------------------------------------------

void SeriesContainerTest::setUp()
{
    // Set up context before running a test.
    m_series = ::fwMedData::SeriesContainer::New();
}

//------------------------------------------------------------------------------

void SeriesContainerTest::tearDown()
{
    // Clean up after the test run.
    m_series->getContainer().clear();
}

//------------------------------------------------------------------------------

void SeriesContainerTest::containerTest()
{
    fwMedData::Series::sptr series1 = fwMedData::Series::New();
    fwMedData::Series::sptr series2 = fwMedData::Series::New();
    fwMedData::Series::sptr series3 = fwMedData::Series::New();

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




    ::fwMedData::SeriesContainer::ContainerType series;

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
