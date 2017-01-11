/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "DicomSeriesWriterTest.hpp"

#include <fwData/Object.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwGdcmIO/helper/DicomAnonymizer.hpp>
#include <fwGdcmIO/helper/DicomSeriesWriter.hpp>
#include <fwGdcmIO/reader/SeriesDB.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwTest/Data.hpp>
#include <fwTest/helper/compare.hpp>

#include <fwTools/System.hpp>

#include <fwZip/ReadZipArchive.hpp>
#include <fwZip/WriteDirArchive.hpp>
#include <fwZip/WriteZipArchive.hpp>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwGdcmIO::ut::DicomSeriesWriterTest );

namespace fwGdcmIO
{
namespace ut
{

void DicomSeriesWriterTest::setUp()
{
    // Set up context before running a test.
    ::fwMedData::SeriesDB::sptr srcSeriesDB = ::fwMedData::SeriesDB::New();
    const ::boost::filesystem::path srcPath = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/DicomDB/01-CT-DICOM_LIVER";

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + srcPath.string() + "' does not exist",
                           ::boost::filesystem::exists(srcPath));


    // Read source Dicom
    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(srcSeriesDB);
    reader->setFolder(srcPath);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), srcSeriesDB->size());

    m_srcDicomSeries =
        ::fwMedData::DicomSeries::dynamicCast(srcSeriesDB->getContainer().front());
}

//------------------------------------------------------------------------------

void DicomSeriesWriterTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void DicomSeriesWriterTest::checkDicomSeries(const ::boost::filesystem::path& p, bool anonymized)
{
    ::fwMedData::SeriesDB::sptr destSeriesDB = ::fwMedData::SeriesDB::New();

    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(destSeriesDB);
    reader->setFolder(p);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), destSeriesDB->size());
    ::fwMedData::DicomSeries::sptr destDicomSeries =
        ::fwMedData::DicomSeries::dynamicCast(destSeriesDB->getContainer().front());

    // Compare Source and Destination Series
    ExcludeSetType excludeSetPrefix;
    excludeSetPrefix.insert("local_dicom_paths");

    ExcludeSetType excludeSet;
    if(anonymized)
    {
        excludeSet.insert("instance_uid");
        excludeSet.insert("study.instance_uid");
    }

    CPPUNIT_ASSERT_MESSAGE("Series not equal",
                           ::fwTest::helper::compare(m_srcDicomSeries, destDicomSeries, excludeSet, excludeSetPrefix));
}

// TODO: This test is disabled as DicomSeries doesn't store Binaries anymore.
void DicomSeriesWriterTest::writeReadTest()
{
    CPPUNIT_ASSERT_MESSAGE("Failed to set up source Dicom series", m_srcDicomSeries);

    const ::boost::filesystem::path destPath = ::fwTools::System::getTemporaryFolder("writeReadTest") /
                                               "dicomSeriesTest";
    ::boost::filesystem::create_directories(destPath);

    // Write Dicom
    ::fwGdcmIO::helper::DicomSeriesWriter::sptr writer = ::fwGdcmIO::helper::DicomSeriesWriter::New();
    writer->setObject(m_srcDicomSeries);
    writer->setFolder(destPath);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    this->checkDicomSeries(destPath);
}

//------------------------------------------------------------------------------

void DicomSeriesWriterTest::writeReadAnonymiseTest()
{
    CPPUNIT_ASSERT_MESSAGE("Failed to set up source Dicom series", m_srcDicomSeries);

    // Anonymize series
    ::fwGdcmIO::helper::DicomAnonymizer::sptr anonymizer
        = ::fwGdcmIO::helper::DicomAnonymizer::New();
    anonymizer->addExceptionTag(0x0010, 0x0010, "ANONYMIZED^ANONYMIZED "); // Patient's name

    const ::boost::filesystem::path destPath
        = ::fwTools::System::getTemporaryFolder("writeReadAnonymiseTest") / "dicomSeriesTest";
    ::boost::filesystem::create_directories(destPath);

    // Write Dicom
    ::fwGdcmIO::helper::DicomSeriesWriter::sptr writer = ::fwGdcmIO::helper::DicomSeriesWriter::New();
    writer->setObject(m_srcDicomSeries);
    writer->setFolder(destPath);
    writer->setAnonymizer(anonymizer);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    this->checkDicomSeries(destPath, true);
}

//------------------------------------------------------------------------------

void DicomSeriesWriterTest::writeReadDirArchiveTest()
{
    CPPUNIT_ASSERT_MESSAGE("Failed to set up source Dicom series", m_srcDicomSeries);

    const ::boost::filesystem::path destPath
        = ::fwTools::System::getTemporaryFolder("writeReadDirArchiveTest") / "dicomSeriesTest";
    ::boost::filesystem::create_directories(destPath);

    ::fwZip::WriteDirArchive::sptr writeArchive = ::fwZip::WriteDirArchive::New(destPath);

    // Write Dicom
    ::fwGdcmIO::helper::DicomSeriesWriter::sptr writer = ::fwGdcmIO::helper::DicomSeriesWriter::New();
    writer->setObject(m_srcDicomSeries);
    writer->setOutputArchive(writeArchive);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    this->checkDicomSeries(destPath);
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwGdcmIO
