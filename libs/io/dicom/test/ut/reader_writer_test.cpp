/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

// cspell:ignore Acuson

#include "reader_writer_test.hpp"

#include <core/tools/system.hpp>
#include <core/tools/uuid.hpp>

#include <data/image_series.hpp>

#include <io/dicom/reader/file.hpp>
#include <io/dicom/writer/file.hpp>

#include <utest/filter.hpp>

#include <utest_data/data.hpp>
#include <utest_data/generator/image.hpp>

#include <TestAssert.h>

#include <chrono>
#include <ctime>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::reader_writer_test);

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

inline static sight::data::series_set::sptr read(const std::filesystem::path _path)
{
    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + _path.string() + "' does not exist",
        std::filesystem::exists(_path)
    );

    auto series_set = std::make_shared<data::series_set>();

    auto reader = std::make_shared<io::dicom::reader::file>();
    reader->set_object(series_set);
    reader->set_folder(_path);

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    for(const auto& series : *series_set)
    {
        series->set_sop_keyword(data::dicom::sop::Keyword::EnhancedUSVolumeStorage);
    }

    return series_set;
}

//------------------------------------------------------------------------------

inline static std::filesystem::path create_temp_folder()
{
    auto tmp_folder = core::tools::system::get_temporary_folder() / core::tools::uuid::generate();
    std::filesystem::remove_all(tmp_folder);
    std::filesystem::create_directories(tmp_folder);

    return tmp_folder;
}

//------------------------------------------------------------------------------

inline static void compare_enhanced_us_volume(
    const data::image_series::sptr& _expected,
    const data::image_series::sptr& _actual
)
{
    CPPUNIT_ASSERT(_expected);
    CPPUNIT_ASSERT(_actual);

    // SOP Class UID
    CPPUNIT_ASSERT_EQUAL(_expected->get_sop_keyword(), _actual->get_sop_keyword());

    // Sizes
    const auto& expected_sizes = _expected->size();
    const auto& actual_sizes   = _actual->size();
    CPPUNIT_ASSERT_EQUAL(expected_sizes.size(), actual_sizes.size());

    for(std::size_t i = 0 ; i < expected_sizes.size() ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(expected_sizes[i], actual_sizes[i]);
    }

    // Type
    CPPUNIT_ASSERT_EQUAL(_expected->type(), _actual->type());

    // Pixel format
    CPPUNIT_ASSERT_EQUAL(_expected->pixel_format(), _actual->pixel_format());

    // Spacings
    const auto& expected_spacing = _expected->spacing();
    const auto& actual_spacing   = _actual->spacing();
    CPPUNIT_ASSERT_EQUAL(expected_spacing.size(), actual_spacing.size());

    for(std::size_t i = 0 ; i < expected_spacing.size() ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_spacing[i], actual_spacing[i], 0.0001);
    }

    // Origins
    const auto& expected_origin = _expected->origin();
    const auto& actual_origin   = _actual->origin();
    CPPUNIT_ASSERT_EQUAL(expected_origin.size(), actual_origin.size());

    for(std::size_t i = 0 ; i < expected_origin.size() ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_origin[i], actual_origin[i], 0.0001);
    }

    // Get Image Position Patient / Image Orientation Patient
    for(std::size_t frame_index = 0 ; frame_index < actual_sizes[2] ; ++frame_index)
    {
        // Image Position Patient
        const auto& expected_position = _expected->get_image_position_patient(frame_index);
        const auto& actual_position   = _actual->get_image_position_patient(frame_index);
        CPPUNIT_ASSERT_EQUAL(expected_position.size(), actual_position.size());

        for(std::size_t i = 0 ; i < expected_position.size() ; ++i)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_position[i], actual_position[i], 0.0001);
        }

        // Image Orientation Patient
        const auto& expected_orientation = _expected->get_image_orientation_patient(frame_index);
        const auto& actual_orientation   = _actual->get_image_orientation_patient(frame_index);
        CPPUNIT_ASSERT_EQUAL(expected_orientation.size(), actual_orientation.size());

        for(std::size_t i = 0 ; i < expected_orientation.size() ; ++i)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_orientation[i], actual_orientation[i], 0.0001);
        }
    }

    // Ensure that getting value outside the frame range returns std::nullopts
    CPPUNIT_ASSERT(
        !_expected->get_frame_acquisition_date_time(actual_sizes[2])
        && !_actual->get_frame_acquisition_date_time(actual_sizes[2])
    );

    // Compare buffer
    const auto expected_locked = _expected->dump_lock();
    const auto actual_locked   = _actual->dump_lock();
    CPPUNIT_ASSERT_EQUAL(0, std::memcmp(_expected->buffer(), _actual->buffer(), _expected->size_in_bytes()));
}

//------------------------------------------------------------------------------

inline static void compare_enhanced_us_volume(
    const data::series_set::sptr& _expected,
    const data::series_set::sptr& _actual
)
{
    CPPUNIT_ASSERT_EQUAL(_expected->size(), _actual->size());
    for(std::size_t i = 0 ; i < _expected->size() ; i++)
    {
        const auto& expected_image_series = std::dynamic_pointer_cast<data::image_series>((*_expected)[i]);
        const auto& actual_image_series   = std::dynamic_pointer_cast<data::image_series>((*_actual)[i]);
        compare_enhanced_us_volume(expected_image_series, actual_image_series);
    }
}

//------------------------------------------------------------------------------

void reader_writer_test::setUp()
{
    // Set up context before running a test.
    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::direct);
}

//------------------------------------------------------------------------------

static void test_image(const std::string& _name)
{
    const auto& folder   = create_temp_folder();
    const auto& expected = read(utest_data::dir() / _name);

    auto writer = std::make_shared<io::dicom::writer::file>();
    writer->set_object(expected);
    writer->set_folder(folder);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    auto actual = std::make_shared<data::series_set>();
    auto reader = std::make_shared<io::dicom::reader::file>();
    reader->set_object(actual);
    reader->set_folder(folder);
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    compare_enhanced_us_volume(expected, actual);
}

//------------------------------------------------------------------------------

void reader_writer_test::basic_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    test_image("us/Enhanced US Volume Storage/GE, 3D+t, lossy JPEG");
    test_image("us/Ultrasound Image Storage/Philips, RLE, palette color");
    test_image("us/Ultrasound Multi-frame Image Storage/Acuson, 2D+t, lossy JPEG");
    test_image("us/Ultrasound Multi-frame Image Storage/GE, 2D+t, RLE");
    test_image("us/Ultrasound Multi-frame Image Storage/Philips, 2D+t,  lossy JPEG");
}

} // namespace sight::io::dicom::ut
