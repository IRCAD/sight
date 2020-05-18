/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "DicomSeriesWriterTest.hpp"

#include <fwGdcmIO/helper/DicomAnonymizer.hpp>
#include <fwGdcmIO/helper/DicomSeriesWriter.hpp>
#include <fwGdcmIO/reader/SeriesDB.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwMedDataCamp/Version.hpp>

#include <fwTest/Data.hpp>
#include <fwTest/helper/compare.hpp>
#include <fwTest/Slow.hpp>

#include <fwTools/System.hpp>

#include <fwZip/ReadZipArchive.hpp>
#include <fwZip/WriteDirArchive.hpp>
#include <fwZip/WriteZipArchive.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwGdcmIO::ut::DicomSeriesWriterTest );

namespace fwGdcmIO
{
namespace ut
{

//------------------------------------------------------------------------------

void DicomSeriesWriterTest::setUp()
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        std::cout << std::endl << "Ignoring slow " << std::endl;
        return;
    }
    else
    {
        std::cout << std::endl << "Executing slow tests.." << std::endl;
    }
    // HACK: force link with fwMedDataCamp. Needed when calling ::fwDataCamp::visitor::CompareObjects::compare.
    m_medDataCampVersion = ::fwMedDataCamp::Version::s_CURRENT_VERSION;

    // HACK, we don't want the compiler to optimize 'm_medDataCampVersion' out.
    CPPUNIT_ASSERT_EQUAL(::fwMedDataCamp::Version::s_CURRENT_VERSION, m_medDataCampVersion);

    // Set up context before running a test.
    ::fwMedData::SeriesDB::sptr srcSeriesDB = ::fwMedData::SeriesDB::New();
    const std::filesystem::path srcPath = ::fwTest::Data::dir() / "sight/Patient/Dicom/DicomDB/01-CT-DICOM_LIVER";

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + srcPath.string() + "' does not exist",
                           std::filesystem::exists(srcPath));

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

void DicomSeriesWriterTest::checkDicomSeries(const std::filesystem::path& p, bool anonymized)
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
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
    excludeSetPrefix.insert("dicom_container");

    ExcludeSetType excludeSet;
    if(anonymized)
    {
        excludeSet.insert("instance_uid");
        excludeSet.insert("study.instance_uid");
        excludeSet.insert("study.date");
        excludeSet.insert("patient.birth_date");
    }

    CPPUNIT_ASSERT_MESSAGE("Series not equal",
                           ::fwTest::helper::compare(m_srcDicomSeries, destDicomSeries, excludeSet, excludeSetPrefix));
}

// TODO: This test is disabled as DicomSeries doesn't store Binaries anymore.
void DicomSeriesWriterTest::writeReadTest()
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    CPPUNIT_ASSERT_MESSAGE("Failed to set up source Dicom series", m_srcDicomSeries);

    const std::filesystem::path destPath = ::fwTools::System::getTemporaryFolder("writeReadTest") /
                                           "dicomSeriesTest";
    std::filesystem::create_directories(destPath);

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
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    CPPUNIT_ASSERT_MESSAGE("Failed to set up source Dicom series", m_srcDicomSeries);

    // Anonymize series
    ::fwGdcmIO::helper::DicomAnonymizer::sptr anonymizer
        = ::fwGdcmIO::helper::DicomAnonymizer::New();
    anonymizer->addExceptionTag(0x0010, 0x0010, "ANONYMIZED^ANONYMIZED "); // Patient's name

    const std::filesystem::path destPath
        = ::fwTools::System::getTemporaryFolder("writeReadAnonymiseTest") / "dicomSeriesTest";
    std::filesystem::create_directories(destPath);

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
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    CPPUNIT_ASSERT_MESSAGE("Failed to set up source Dicom series", m_srcDicomSeries);

    const std::filesystem::path destPath
        = ::fwTools::System::getTemporaryFolder("writeReadDirArchiveTest") / "dicomSeriesTest";
    std::filesystem::create_directories(destPath);

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
