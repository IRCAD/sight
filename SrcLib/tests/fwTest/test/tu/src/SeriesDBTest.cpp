/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>

#include <fwTest/generator/SeriesDB.hpp>

#include "SeriesDBTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTest::ut::SeriesDBTest );

namespace fwTest
{
namespace ut
{

//------------------------------------------------------------------------------

void SeriesDBTest::setUp()
{
    // Set up context before running a test.
    srand(time(NULL));
}

//------------------------------------------------------------------------------

void SeriesDBTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SeriesDBTest::testGetterSetter3D()
{
    const unsigned char nbImgSeries = 3;
    const unsigned char nbModelSeries = 4;
    const unsigned char nbActivitySeries = 5;
    ::fwMedData::SeriesDB::sptr seriesDB;
    seriesDB = ::fwTest::generator::SeriesDB::createSeriesDB(nbImgSeries, nbModelSeries, nbActivitySeries);

    ::fwMedData::SeriesDB::ContainerType seriesContainer = seriesDB->getContainer();
    CPPUNIT_ASSERT_EQUAL((size_t) (nbImgSeries + nbModelSeries + nbActivitySeries), seriesContainer.size());

    CPPUNIT_ASSERT(::fwMedData::ImageSeries::dynamicCast(seriesContainer[0]));

    ::fwMedData::DicomValuesType performingPhysiciansName;
    performingPhysiciansName.push_back("Dr Jekyl");
    performingPhysiciansName.push_back("Dr House");
    performingPhysiciansName.push_back("Dr Einstein");
    BOOST_FOREACH(::fwMedData::Series::sptr series, seriesContainer)
    {
        CPPUNIT_ASSERT_EQUAL(std::string("1346357.1664.482101.421337.4123403") , series->getInstanceUID());
        CPPUNIT_ASSERT_EQUAL(std::string("CT") , series->getModality());
        CPPUNIT_ASSERT_EQUAL(std::string("04-18-2013") , series->getDate());
        CPPUNIT_ASSERT_EQUAL(std::string("10:10") , series->getTime());
        CPPUNIT_ASSERT_EQUAL(std::string("Description") , series->getDescription());
        CPPUNIT_ASSERT(performingPhysiciansName == series->getPerformingPhysiciansName());

        ::fwMedData::Patient::sptr patient = series->getPatient();
        CPPUNIT_ASSERT_EQUAL(std::string("NomSeriesDB1^PrenomSeriesDB1") , patient->getName());
        CPPUNIT_ASSERT_EQUAL(std::string("456438375") , patient->getPatientId());
        CPPUNIT_ASSERT_EQUAL(std::string("04-18-2013 09:53") , patient->getBirthdate());
        CPPUNIT_ASSERT_EQUAL(std::string("O") , patient->getSex());

        ::fwMedData::Study::sptr study = series->getStudy();
        CPPUNIT_ASSERT_EQUAL(std::string("1346357.1664.643101.421337.4123403") , study->getInstanceUID());
        CPPUNIT_ASSERT_EQUAL(std::string("04-18-2013") , study->getDate());
        CPPUNIT_ASSERT_EQUAL(std::string("09:59") , study->getTime());
        CPPUNIT_ASSERT_EQUAL(std::string("Dr Jekyl") , study->getReferringPhysicianName());
        CPPUNIT_ASSERT_EQUAL(std::string("Say 33.") , study->getDescription());
        CPPUNIT_ASSERT_EQUAL(std::string("42") , study->getPatientAge());

        ::fwMedData::Equipment::sptr equipement = series->getEquipment();
        CPPUNIT_ASSERT_EQUAL(std::string("hospital"), equipement->getInstitutionName());
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwTest
