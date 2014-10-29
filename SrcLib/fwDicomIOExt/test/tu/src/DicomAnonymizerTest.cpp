/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwTools/System.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwData/Object.hpp>
#include <fwData/Image.hpp>

#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/Study.hpp>
#include <fwMedData/Patient.hpp>

#include <fwTest/generator/SeriesDB.hpp>
#include <fwTest/generator/Image.hpp>

#include <vtkGdcmIO/ImageSeriesWriter.hpp>
#include <vtkGdcmIO/SeriesDBReader.hpp>
#include <fwDicomIOExt/gdcm/helper/DicomAnonymizer.hpp>

#include "DicomAnonymizerTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDicomIOExt::ut::DicomAnonymizerTest );

namespace fwDicomIOExt
{
namespace ut
{

//------------------------------------------------------------------------------

void DicomAnonymizerTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void DicomAnonymizerTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void DicomAnonymizerTest::anonymizeDicomFolderTest()
{
    ::fwTest::generator::Image::initRand();
    ::fwMedData::ImageSeries::sptr imgSeries;
    imgSeries = ::fwTest::generator::SeriesDB::createImageSeries();

    const ::boost::filesystem::path path = ::fwTools::System::getTemporaryFolder() / "anonymizedDicomFolderTest";
    ::boost::filesystem::create_directories( path );

    // Write ImageSeries
    ::vtkGdcmIO::ImageSeriesWriter::sptr writer = ::vtkGdcmIO::ImageSeriesWriter::New();
    writer->setObject(imgSeries);
    writer->setFolder(path);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    // Anonamyze ImageSeries
    ::fwDicomIOExt::gdcm::helper::DicomAnonymizer anonymizer;
    CPPUNIT_ASSERT_NO_THROW(anonymizer.anonymize(path));

    // Load ImageSeries
    ::fwMedData::SeriesDB::sptr sdb = ::fwMedData::SeriesDB::New();
    ::vtkGdcmIO::SeriesDBReader::sptr reader = ::vtkGdcmIO::SeriesDBReader::New();
    reader->setObject(sdb);
    reader->setFolder(path);
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Check series
    CPPUNIT_ASSERT_EQUAL(size_t(1), sdb->getContainer().size());

    ::fwMedData::ImageSeries::sptr anonymizedSeries =
            ::fwMedData::ImageSeries::dynamicCast(sdb->getContainer().front());

    CPPUNIT_ASSERT(anonymizedSeries);

    // Check values
    // Spaces are added at the end of strings in order to have even lengths
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymizedSeries->getDate());
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymizedSeries->getTime());
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymizedSeries->getDescription());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0, anonymizedSeries->getPerformingPhysiciansName().size(),0);
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymizedSeries->getEquipment()->getInstitutionName());
    CPPUNIT_ASSERT_EQUAL(std::string("ANONYMIZED^ANONYMIZED "), anonymizedSeries->getPatient()->getName());
    CPPUNIT_ASSERT_EQUAL(std::string("ANONYMIZED"), anonymizedSeries->getPatient()->getPatientId());
    CPPUNIT_ASSERT_EQUAL(std::string("19700101"), anonymizedSeries->getPatient()->getBirthdate());
    CPPUNIT_ASSERT_EQUAL(std::string("O "), anonymizedSeries->getPatient()->getSex());
    CPPUNIT_ASSERT_EQUAL(std::string("19700101"), anonymizedSeries->getStudy()->getDate());
    CPPUNIT_ASSERT_EQUAL(std::string("000000.000000 "), anonymizedSeries->getStudy()->getTime());
    CPPUNIT_ASSERT_EQUAL(std::string("ANONYMIZED^ANONYMIZED "), anonymizedSeries->getStudy()->getReferringPhysicianName());
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymizedSeries->getStudy()->getDescription());
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymizedSeries->getStudy()->getPatientAge());
    CPPUNIT_ASSERT(imgSeries->getInstanceUID() != anonymizedSeries->getInstanceUID());
    CPPUNIT_ASSERT(imgSeries->getStudy()->getInstanceUID() != anonymizedSeries->getStudy()->getInstanceUID());

}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwDicomIOExt
