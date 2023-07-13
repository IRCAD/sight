/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <core/os/TempPath.hpp>

#include <data/DicomSeries.hpp>

#include <io/dicom/helper/DicomAnonymizer.hpp>
#include <io/dicom/helper/DicomSeriesWriter.hpp>
#include <io/dicom/reader/SeriesSet.hpp>
#include <io/zip/WriteDirArchive.hpp>

#include <utest/Filter.hpp>

#include <utestData/Data.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::DicomSeriesWriterTest);

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

void DicomSeriesWriterTest::setUp()
{
    if(utest::Filter::ignoreSlowTests())
    {
        std::cout << std::endl << "Ignoring slow " << std::endl;
        return;
    }

    std::cout << std::endl << "Executing slow tests.." << std::endl;

    // Set up context before running a test.
    auto src_series_set                 = data::SeriesSet::New();
    const std::filesystem::path srcPath = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB/01-CT-DICOM_LIVER";

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + srcPath.string() + "' does not exist",
        std::filesystem::exists(srcPath)
    );

    // Read source Dicom
    auto reader = io::dicom::reader::SeriesSet::New();
    reader->setObject(src_series_set);
    reader->setFolder(srcPath);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), src_series_set->size());

    m_srcDicomSeries = data::DicomSeries::dynamicCast(src_series_set->front());
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

    auto dest_series_set = data::SeriesSet::New();

    auto reader = io::dicom::reader::SeriesSet::New();
    reader->setObject(dest_series_set);
    reader->setFolder(p);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), dest_series_set->size());
    auto destDicomSeries = data::DicomSeries::dynamicCast(dest_series_set->front());

    // Compare Source and Destination Series
    if(anonymized)
    {
        destDicomSeries->setSeriesInstanceUID(m_srcDicomSeries->getSeriesInstanceUID());
        destDicomSeries->copyPatientModule(m_srcDicomSeries);
        destDicomSeries->copyGeneralStudyModule(m_srcDicomSeries);
        destDicomSeries->copyPatientStudyModule(m_srcDicomSeries);
        destDicomSeries->copyGeneralEquipmentModule(m_srcDicomSeries);
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

    core::os::TempDir tmpDir;
    std::filesystem::create_directories(tmpDir);

    // Write Dicom
    io::dicom::helper::DicomSeriesWriter::sptr writer = io::dicom::helper::DicomSeriesWriter::New();
    writer->setObject(m_srcDicomSeries);
    writer->setFolder(tmpDir);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    this->checkDicomSeries(tmpDir);
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

    core::os::TempDir tmpDir;

    // Write Dicom
    auto writer = io::dicom::helper::DicomSeriesWriter::New();
    writer->setObject(m_srcDicomSeries);
    writer->setFolder(tmpDir);
    writer->setAnonymizer(anonymizer);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    this->checkDicomSeries(tmpDir, true);
}

//------------------------------------------------------------------------------

void DicomSeriesWriterTest::writeReadDirArchiveTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    CPPUNIT_ASSERT_MESSAGE("Failed to set up source Dicom series", m_srcDicomSeries);

    core::os::TempDir tmpDir;
    auto writeArchive = io::zip::WriteDirArchive::New(tmpDir);

    // Write Dicom
    auto writer = io::dicom::helper::DicomSeriesWriter::New();
    writer->setObject(m_srcDicomSeries);
    writer->setOutputArchive(writeArchive);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    this->checkDicomSeries(tmpDir);
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::ut
