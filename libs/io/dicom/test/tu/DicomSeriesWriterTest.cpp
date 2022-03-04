/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/tools/System.hpp>

#include <data/DicomSeries.hpp>
#include <data/SeriesDB.hpp>

#include <io/dicom/helper/DicomAnonymizer.hpp>
#include <io/dicom/helper/DicomSeriesWriter.hpp>
#include <io/dicom/reader/SeriesDB.hpp>
#include <io/zip/WriteDirArchive.hpp>

#include <utest/Filter.hpp>

#include <utestData/Data.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::DicomSeriesWriterTest);

namespace sight::io::dicom
{

namespace ut
{

//------------------------------------------------------------------------------

void DicomSeriesWriterTest::setUp()
{
    if(utest::Filter::ignoreSlowTests())
    {
        std::cout << std::endl << "Ignoring slow " << std::endl;
        return;
    }
    else
    {
        std::cout << std::endl << "Executing slow tests.." << std::endl;
    }

    // Set up context before running a test.
    data::SeriesDB::sptr srcSeriesDB    = data::SeriesDB::New();
    const std::filesystem::path srcPath = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB/01-CT-DICOM_LIVER";

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + srcPath.string() + "' does not exist",
        std::filesystem::exists(srcPath)
    );

    // Read source Dicom
    io::dicom::reader::SeriesDB::sptr reader = io::dicom::reader::SeriesDB::New();
    reader->setObject(srcSeriesDB);
    reader->setFolder(srcPath);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), srcSeriesDB->size());

    m_srcDicomSeries =
        data::DicomSeries::dynamicCast(srcSeriesDB->getContainer().front());
}

//------------------------------------------------------------------------------

void DicomSeriesWriterTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void DicomSeriesWriterTest::checkDicomSeries(const std::filesystem::path& p, bool anonymized)
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    data::SeriesDB::sptr destSeriesDB = data::SeriesDB::New();

    io::dicom::reader::SeriesDB::sptr reader = io::dicom::reader::SeriesDB::New();
    reader->setObject(destSeriesDB);
    reader->setFolder(p);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), destSeriesDB->size());
    data::DicomSeries::sptr destDicomSeries =
        data::DicomSeries::dynamicCast(destSeriesDB->getContainer().front());

    // Compare Source and Destination Series
    if(anonymized)
    {
        destDicomSeries->setInstanceUID(m_srcDicomSeries->getInstanceUID());
        destDicomSeries->setStudy(m_srcDicomSeries->getStudy());
        destDicomSeries->setPatient(m_srcDicomSeries->getPatient());
    }

    destDicomSeries->setDicomContainer(m_srcDicomSeries->getDicomContainer());

    CPPUNIT_ASSERT_MESSAGE("Series not equal", *m_srcDicomSeries == *destDicomSeries);
}

// TODO: This test is disabled as DicomSeries doesn't store Binaries anymore.
void DicomSeriesWriterTest::writeReadTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    CPPUNIT_ASSERT_MESSAGE("Failed to set up source Dicom series", m_srcDicomSeries);

    const std::filesystem::path destPath = core::tools::System::getTemporaryFolder("writeReadTest")
                                           / "dicomSeriesTest";
    std::filesystem::create_directories(destPath);

    // Write Dicom
    io::dicom::helper::DicomSeriesWriter::sptr writer = io::dicom::helper::DicomSeriesWriter::New();
    writer->setObject(m_srcDicomSeries);
    writer->setFolder(destPath);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    this->checkDicomSeries(destPath);
}

//------------------------------------------------------------------------------

void DicomSeriesWriterTest::writeReadAnonymiseTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    CPPUNIT_ASSERT_MESSAGE("Failed to set up source Dicom series", m_srcDicomSeries);

    // Anonymize series
    io::dicom::helper::DicomAnonymizer::sptr anonymizer =
        io::dicom::helper::DicomAnonymizer::New();
    anonymizer->addExceptionTag(0x0010, 0x0010, "ANONYMIZED^ANONYMIZED "); // Patient's name

    const std::filesystem::path destPath =
        core::tools::System::getTemporaryFolder("writeReadAnonymiseTest") / "dicomSeriesTest";
    std::filesystem::create_directories(destPath);

    // Write Dicom
    io::dicom::helper::DicomSeriesWriter::sptr writer = io::dicom::helper::DicomSeriesWriter::New();
    writer->setObject(m_srcDicomSeries);
    writer->setFolder(destPath);
    writer->setAnonymizer(anonymizer);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    this->checkDicomSeries(destPath, true);
}

//------------------------------------------------------------------------------

void DicomSeriesWriterTest::writeReadDirArchiveTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    CPPUNIT_ASSERT_MESSAGE("Failed to set up source Dicom series", m_srcDicomSeries);

    const std::filesystem::path destPath =
        core::tools::System::getTemporaryFolder("writeReadDirArchiveTest") / "dicomSeriesTest";
    std::filesystem::create_directories(destPath);

    io::zip::WriteDirArchive::sptr writeArchive = io::zip::WriteDirArchive::New(destPath);

    // Write Dicom
    io::dicom::helper::DicomSeriesWriter::sptr writer = io::dicom::helper::DicomSeriesWriter::New();
    writer->setObject(m_srcDicomSeries);
    writer->setOutputArchive(writeArchive);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    this->checkDicomSeries(destPath);
}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace sight::io::dicom
