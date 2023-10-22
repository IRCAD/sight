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

#include <core/os/temp_path.hpp>

#include <data/dicom_series.hpp>

#include <io/dicom/helper/DicomAnonymizer.hpp>
#include <io/dicom/helper/DicomSeriesWriter.hpp>
#include <io/dicom/reader/series_set.hpp>
#include <io/zip/WriteDirArchive.hpp>

#include <utest/Filter.hpp>

#include <utest_data/Data.hpp>

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
    auto src_series_set                  = std::make_shared<data::series_set>();
    const std::filesystem::path src_path = utest_data::Data::dir() / "sight/Patient/Dicom/DicomDB/01-CT-DICOM_LIVER";

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + src_path.string() + "' does not exist",
        std::filesystem::exists(src_path)
    );

    // Read source Dicom
    auto reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(src_series_set);
    reader->set_folder(src_path);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), src_series_set->size());

    m_srcDicomSeries = std::dynamic_pointer_cast<data::dicom_series>(src_series_set->front());
}

//------------------------------------------------------------------------------

void DicomSeriesWriterTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void DicomSeriesWriterTest::checkDicomSeries(const std::filesystem::path& _p, bool _anonymized)
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    auto dest_series_set = std::make_shared<data::series_set>();

    auto reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(dest_series_set);
    reader->set_folder(_p);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), dest_series_set->size());
    auto dest_dicom_series = std::dynamic_pointer_cast<data::dicom_series>(dest_series_set->front());

    // Compare Source and Destination Series
    if(_anonymized)
    {
        dest_dicom_series->setSeriesInstanceUID(m_srcDicomSeries->getSeriesInstanceUID());
        dest_dicom_series->copyPatientModule(m_srcDicomSeries);
        dest_dicom_series->copyGeneralStudyModule(m_srcDicomSeries);
        dest_dicom_series->copyPatientStudyModule(m_srcDicomSeries);
        dest_dicom_series->copyGeneralEquipmentModule(m_srcDicomSeries);
    }

    dest_dicom_series->setDicomContainer(m_srcDicomSeries->getDicomContainer());

    CPPUNIT_ASSERT_MESSAGE("Series not equal", *m_srcDicomSeries == *dest_dicom_series);
}

// TODO: This test is disabled as DicomSeries doesn't store Binaries anymore.
void DicomSeriesWriterTest::writeReadTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    CPPUNIT_ASSERT_MESSAGE("Failed to set up source Dicom series", m_srcDicomSeries);

    core::os::temp_dir tmp_dir;
    std::filesystem::create_directories(tmp_dir);

    // Write Dicom
    io::dicom::helper::DicomSeriesWriter::sptr writer = std::make_shared<io::dicom::helper::DicomSeriesWriter>();
    writer->set_object(m_srcDicomSeries);
    writer->set_folder(tmp_dir);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    this->checkDicomSeries(tmp_dir);
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
    auto anonymizer = std::make_shared<io::dicom::helper::DicomAnonymizer>();
    anonymizer->addExceptionTag(0x0010, 0x0010, "ANONYMIZED^ANONYMIZED "); // Patient's name

    core::os::temp_dir tmp_dir;

    // Write Dicom
    auto writer = std::make_shared<io::dicom::helper::DicomSeriesWriter>();
    writer->set_object(m_srcDicomSeries);
    writer->set_folder(tmp_dir);
    writer->setAnonymizer(anonymizer);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    this->checkDicomSeries(tmp_dir, true);
}

//------------------------------------------------------------------------------

void DicomSeriesWriterTest::writeReadDirArchiveTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    CPPUNIT_ASSERT_MESSAGE("Failed to set up source Dicom series", m_srcDicomSeries);

    core::os::temp_dir tmp_dir;
    auto write_archive = io::zip::WriteDirArchive::make(tmp_dir);

    // Write Dicom
    auto writer = std::make_shared<io::dicom::helper::DicomSeriesWriter>();
    writer->set_object(m_srcDicomSeries);
    writer->setOutputArchive(write_archive);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    this->checkDicomSeries(tmp_dir);
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::ut
