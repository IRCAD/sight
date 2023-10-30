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
#include <io/zip/write_dir_archive.hpp>

#include <utest/filter.hpp>

#include <utest_data/Data.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::dicom_series_writer_test);

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

void dicom_series_writer_test::setUp()
{
    if(utest::filter::ignore_slow_tests())
    {
        std::cout << std::endl << "Ignoring slow " << std::endl;
        return;
    }

    std::cout << std::endl << "Executing slow tests.." << std::endl;

    // Set up context before running a test.
    auto src_series_set                  = std::make_shared<data::series_set>();
    const std::filesystem::path src_path = utest_data::dir() / "sight/Patient/Dicom/DicomDB/01-CT-DICOM_LIVER";

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + src_path.string() + "' does not exist",
        std::filesystem::exists(src_path)
    );

    // Read source Dicom
    auto reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(src_series_set);
    reader->set_folder(src_path);
    CPPUNIT_ASSERT_NO_THROW(reader->read_dicom_series());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), src_series_set->size());

    m_src_dicom_series = std::dynamic_pointer_cast<data::dicom_series>(src_series_set->front());
}

//------------------------------------------------------------------------------

void dicom_series_writer_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void dicom_series_writer_test::check_dicom_series(const std::filesystem::path& _p, bool _anonymized)
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    auto dest_series_set = std::make_shared<data::series_set>();

    auto reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(dest_series_set);
    reader->set_folder(_p);
    CPPUNIT_ASSERT_NO_THROW(reader->read_dicom_series());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), dest_series_set->size());
    auto dest_dicom_series = std::dynamic_pointer_cast<data::dicom_series>(dest_series_set->front());

    // Compare Source and Destination Series
    if(_anonymized)
    {
        dest_dicom_series->set_series_instance_uid(m_src_dicom_series->get_series_instance_uid());
        dest_dicom_series->copy_patient_module(m_src_dicom_series);
        dest_dicom_series->copy_general_study_module(m_src_dicom_series);
        dest_dicom_series->copy_patient_study_module(m_src_dicom_series);
        dest_dicom_series->copy_general_equipment_module(m_src_dicom_series);
    }

    dest_dicom_series->set_dicom_container(m_src_dicom_series->get_dicom_container());

    CPPUNIT_ASSERT_MESSAGE("Series not equal", *m_src_dicom_series == *dest_dicom_series);
}

// TODO: This test is disabled as DicomSeries doesn't store Binaries anymore.
void dicom_series_writer_test::write_read_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    CPPUNIT_ASSERT_MESSAGE("Failed to set up source Dicom series", m_src_dicom_series);

    core::os::temp_dir tmp_dir;
    std::filesystem::create_directories(tmp_dir);

    // Write Dicom
    io::dicom::helper::dicom_series_writer::sptr writer = std::make_shared<io::dicom::helper::dicom_series_writer>();
    writer->set_object(m_src_dicom_series);
    writer->set_folder(tmp_dir);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    this->check_dicom_series(tmp_dir);
}

//------------------------------------------------------------------------------

void dicom_series_writer_test::write_read_anonymise_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    CPPUNIT_ASSERT_MESSAGE("Failed to set up source Dicom series", m_src_dicom_series);

    // Anonymize series
    auto anonymizer = std::make_shared<io::dicom::helper::dicom_anonymizer>();
    anonymizer->add_exception_tag(0x0010, 0x0010, "ANONYMIZED^ANONYMIZED "); // Patient's name

    core::os::temp_dir tmp_dir;

    // Write Dicom
    auto writer = std::make_shared<io::dicom::helper::dicom_series_writer>();
    writer->set_object(m_src_dicom_series);
    writer->set_folder(tmp_dir);
    writer->set_anonymizer(anonymizer);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    this->check_dicom_series(tmp_dir, true);
}

//------------------------------------------------------------------------------

void dicom_series_writer_test::write_read_dir_archive_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    CPPUNIT_ASSERT_MESSAGE("Failed to set up source Dicom series", m_src_dicom_series);

    core::os::temp_dir tmp_dir;
    auto write_archive = io::zip::write_dir_archive::make(tmp_dir);

    // Write Dicom
    auto writer = std::make_shared<io::dicom::helper::dicom_series_writer>();
    writer->set_object(m_src_dicom_series);
    writer->set_output_archive(write_archive);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    this->check_dicom_series(tmp_dir);
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::ut
