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


#include "SeriesTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedData::ut::SeriesTest );

namespace fwMedData
{
namespace ut
{

//------------------------------------------------------------------------------

void SeriesTest::setUp()
{
    // Set up context before running a test.
    m_series.push_back( ::fwMedData::ImageSeries::New() );
    m_series.push_back( ::fwMedData::ModelSeries::New() );
}

//------------------------------------------------------------------------------

void SeriesTest::tearDown()
{
    // Clean up after the test run.
    m_series.clear();
}

//------------------------------------------------------------------------------

void SeriesTest::attrPatientTest()
{
    for (SeriesVectorType::iterator iter = m_series.begin(); iter != m_series.end(); ++iter)
    {
        ::fwMedData::Series::sptr series = *iter;
        CPPUNIT_ASSERT(series->getPatient());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrStudyTest()
{
    for (SeriesVectorType::iterator iter = m_series.begin(); iter != m_series.end(); ++iter)
    {
        ::fwMedData::Series::sptr series = *iter;
        CPPUNIT_ASSERT(series->getStudy());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrEquipmentTest()
{
    for (SeriesVectorType::iterator iter = m_series.begin(); iter != m_series.end(); ++iter)
    {
        ::fwMedData::Series::sptr series = *iter;
        CPPUNIT_ASSERT(series->getEquipment());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrInstanceUIDTest()
{
    const std::string instanceUID = "1337.1664.42";

    for (SeriesVectorType::iterator iter = m_series.begin(); iter != m_series.end(); ++iter)
    {
        ::fwMedData::Series::sptr series = *iter;
        series->setInstanceUID( instanceUID );

        CPPUNIT_ASSERT_EQUAL( instanceUID, series->getInstanceUID() );
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrModalityTest()
{
    const std::string modality = "MR";

    for (SeriesVectorType::iterator iter = m_series.begin(); iter != m_series.end(); ++iter)
    {
        ::fwMedData::Series::sptr series = *iter;
        series->setModality( modality );

        CPPUNIT_ASSERT_EQUAL( modality, series->getModality() );
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrDateTest()
{
    const std::string date = "02-14-2015";

    for (SeriesVectorType::iterator iter = m_series.begin(); iter != m_series.end(); ++iter)
    {
        ::fwMedData::Series::sptr series = *iter;
        series->setDate( date );

        CPPUNIT_ASSERT_EQUAL( date, series->getDate() );
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrTimeTest()
{
    const std::string time = "14:02";

    for (SeriesVectorType::iterator iter = m_series.begin(); iter != m_series.end(); ++iter)
    {
        ::fwMedData::Series::sptr series = *iter;
        series->setTime( time );

        CPPUNIT_ASSERT_EQUAL( time, series->getTime() );
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrPerformingPhysiciansNameTest()
{
    fwMedData::DicomValuesType performingPhysiciansName;

    performingPhysiciansName.push_back("Dr Jekyl");
    performingPhysiciansName.push_back("Dr House");
    performingPhysiciansName.push_back("Dr Einstein");

    for (SeriesVectorType::iterator iter = m_series.begin(); iter != m_series.end(); ++iter)
    {
        ::fwMedData::Series::sptr series = *iter;
        series->setPerformingPhysiciansName( performingPhysiciansName );

        CPPUNIT_ASSERT( performingPhysiciansName == series->getPerformingPhysiciansName() );
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrDescriptionTest()
{
    const std::string description = "Series";

    for (SeriesVectorType::iterator iter = m_series.begin(); iter != m_series.end(); ++iter)
    {
        ::fwMedData::Series::sptr series = *iter;
        series->setDescription( description );

        CPPUNIT_ASSERT_EQUAL( description, series->getDescription() );
    }
}

//------------------------------------------------------------------------------



} //namespace ut
} //namespace fwMedData
