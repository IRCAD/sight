/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include "writer_test.hpp"

#include <core/os/temp_path.hpp>

#include <data/image_series.hpp>

#include <io/bitmap/backend.hpp>
#include <io/dicom/reader/file.hpp>
#include <io/dicom/writer/file.hpp>

#include <utest/filter.hpp>
#include <utest/profiling.hpp>

#include <utest_data/data.hpp>
#include <utest_data/generator/image.hpp>

#include <chrono>
#include <ctime>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::writer_test);

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

/// @note this function is only needed because std::format (c++20) is not supported by clang 14 / gcc 12
/// It is bold C API to format a date / time to something like "YYYYMMDDHHMMSS.FFFFFF" (DICOM DateTime format)
inline static std::string format_date_time(const std::chrono::time_point<std::chrono::system_clock>& _now)
{
    time_t now_time = std::chrono::system_clock::to_time_t(_now);

    struct tm timeinfo {};

#ifdef WIN32
    localtime_s(&timeinfo, &now_time);
#else
    localtime_r(&now_time, &timeinfo);
#endif

    const auto truncated_now = std::chrono::system_clock::from_time_t(now_time);
    const auto ms            = (_now - truncated_now).count();

    std::string buffer("YYYYMMDDHHMMSS.");
    const auto size = strftime(buffer.data(), buffer.size(), "%Y%m%d%H%M%S", &timeinfo);
    buffer.resize(size);

    return buffer + "." + std::to_string(ms).substr(0, 6);
}

//------------------------------------------------------------------------------

inline static data::image_series::sptr get_us_volume_image(
    std::uint32_t _seed                    = 0,
    std::size_t _num_frames                = 1,
    core::type _type                       = core::type::UINT8,
    enum data::image::pixel_format _format = data::image::rgb
)
{
    using key_t = std::tuple<std::size_t, core::type, enum data::image::pixel_format, std::uint32_t>;
    static std::map<key_t, data::image_series::sptr> generated;

    const key_t key {_num_frames, _type, _format, _seed};

    const auto& it = generated.find(key);

    if(it == generated.end())
    {
        auto image = std::make_shared<data::image_series>();

        utest_data::generator::image::generate_image(
            image,
            {64, 64, _num_frames},
            {1.0, 1.0, 1.0},
            {0, 0, 0},
            _type,
            _format,
            _seed
        );

        if(_seed == 0xFFFF)
        {
            // De-randomize a bit the image to allow compression, otherwise we cannot check the effectiveness.
            const auto dump_lock = image->dump_lock();

            const auto& sizes     = image->size();
            std::size_t index     = 0;
            std::size_t max_index = (sizes[0] * sizes[1] * sizes[2] * image->num_components() / 10);
            auto image_it         = image->begin<std::uint8_t>();
            const auto& end       = image->end<std::uint8_t>();

            for( ; image_it != end ; ++image_it)
            {
                std::uint8_t value = 0;

                if(index++ < max_index)
                {
                    // This is for RLE
                    value = 0xA0;
                }
                else
                {
                    value = (*image_it % 0x20) + 0xA0;
                }

                *image_it = value;
            }
        }

        // We want an Enhanced US Volume
        image->set_sop_keyword(data::dicom::sop::Keyword::EnhancedUSVolumeStorage);

        // Set Image Position Patient / Image Orientation Patient
        for(std::size_t frame_index = 0 ; frame_index < _num_frames ; ++frame_index)
        {
            // ..Image Position / Orientation Patient is what we want
            image->set_image_position_patient(
                {
                    double(_seed + 1) * 0.1,
                    double(_seed + 1) * 0.2,
                    double(_seed + 1) * 0.3
                },
                frame_index
            );

            image->set_image_orientation_patient(
                {
                    double(_seed + 1) * 0.4,
                    double(_seed + 1) * 0.5,
                    double(_seed + 1) * 0.6,
                    double(_seed + 1) * 0.7,
                    double(_seed + 1) * 0.8,
                    double(_seed + 1) * 0.9,
                },
                frame_index
            );

            // set the Frame Acquisition Date Time, which is our "timestamp"
            auto now = std::chrono::system_clock::now();
            now += std::chrono::milliseconds(frame_index);

            //YYYYMMDDHHMMSS.FFFFFF
            image->set_frame_acquisition_date_time(format_date_time(now), frame_index);

            // Add a private custom attribute
            image->set_multi_frame_private_value(
                format_date_time(now),
                0x42,
                frame_index
            );
        }

        generated[key] = image;

        return image;
    }

    return it->second;
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

        // FrameAcquisitionDateTime
        CPPUNIT_ASSERT_EQUAL(
            *_expected->get_frame_acquisition_date_time(frame_index),
            *_actual->get_frame_acquisition_date_time(frame_index)
        );

        // Private custom attribute
        CPPUNIT_ASSERT_EQUAL(
            *_expected->get_multi_frame_private_value(0x42, frame_index),
            *_actual->get_multi_frame_private_value(0x42, frame_index)
        );
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

void writer_test::setUp()
{
    // Set up context before running a test.
    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::direct);
}

//------------------------------------------------------------------------------

void writer_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void writer_test::write_enhanced_us_volume_test()
{
    {
        core::os::temp_dir tmp_dir;
        const auto& expected = get_us_volume_image(0);

        // Write a single RGB uint8 frame image
        {
            auto series_set = std::make_shared<data::series_set>();
            series_set->push_back(expected);

            auto writer = std::make_shared<io::dicom::writer::file>();
            writer->set_object(series_set);
            writer->set_folder(tmp_dir);

            CPPUNIT_ASSERT_NO_THROW(writer->write());
        }

        // Read the previously written single frame image
        {
            auto series_set = std::make_shared<data::series_set>();
            auto reader     = std::make_shared<io::dicom::reader::file>();
            reader->set_object(series_set);
            reader->set_folder(tmp_dir);

            CPPUNIT_ASSERT_NO_THROW(reader->read());
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

            compare_enhanced_us_volume(expected, std::dynamic_pointer_cast<data::image_series>(series_set->front()));
        }
    }

    {
        core::os::temp_dir tmp_dir;
        const auto& expected = get_us_volume_image(1, 4);

        // Write a 4 frames RGB uint8 image
        {
            auto series_set = std::make_shared<data::series_set>();
            series_set->push_back(expected);

            auto writer = std::make_shared<io::dicom::writer::file>();
            writer->set_object(series_set);
            writer->set_folder(tmp_dir);

            CPPUNIT_ASSERT_NO_THROW(writer->write());
        }

        // Read the previously written 4 frames image
        {
            auto series_set = std::make_shared<data::series_set>();
            auto reader     = std::make_shared<io::dicom::reader::file>();
            reader->set_object(series_set);
            reader->set_folder(tmp_dir);

            CPPUNIT_ASSERT_NO_THROW(reader->read());
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

            compare_enhanced_us_volume(expected, std::dynamic_pointer_cast<data::image_series>(series_set->front()));
        }
    }

    {
        core::os::temp_dir tmp_dir;
        const auto& expected = get_us_volume_image(2, 4, core::type::UINT16, data::image::pixel_format::gray_scale);

        // Write a 4 frames monochrome uint16 image
        {
            auto series_set = std::make_shared<data::series_set>();
            series_set->push_back(expected);

            auto writer = std::make_shared<io::dicom::writer::file>();
            writer->set_object(series_set);
            writer->set_folder(tmp_dir);

            CPPUNIT_ASSERT_NO_THROW(writer->write());
        }

        // Read the previously written 4 frames image
        {
            auto series_set = std::make_shared<data::series_set>();
            auto reader     = std::make_shared<io::dicom::reader::file>();
            reader->set_object(series_set);
            reader->set_folder(tmp_dir);

            CPPUNIT_ASSERT_NO_THROW(reader->read());
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

            compare_enhanced_us_volume(expected, std::dynamic_pointer_cast<data::image_series>(series_set->front()));
        }
    }

    {
        core::os::temp_dir tmp_dir;
        const auto& expected = get_us_volume_image(2, 4);

        // Write a 4 frames RGB uint8 image, with a custom filename
        {
            auto series_set = std::make_shared<data::series_set>();
            series_set->push_back(expected);

            auto writer = std::make_shared<io::dicom::writer::file>();
            writer->set_object(series_set);
            writer->set_folder(tmp_dir);
            writer->set_file("custom_filename.dcm");

            CPPUNIT_ASSERT_NO_THROW(writer->write());
            CPPUNIT_ASSERT(std::filesystem::exists(tmp_dir / "custom_filename.dcm"));
        }

        // Read the previously written 4 frames image
        {
            auto series_set = std::make_shared<data::series_set>();
            auto reader     = std::make_shared<io::dicom::reader::file>();
            reader->set_object(series_set);
            reader->set_folder(tmp_dir);

            CPPUNIT_ASSERT_NO_THROW(reader->read());
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

            compare_enhanced_us_volume(expected, std::dynamic_pointer_cast<data::image_series>(series_set->front()));
        }
    }

    {
        core::os::temp_dir tmp_dir;
        const auto& expected0 = get_us_volume_image(0, 4);
        const auto& expected1 = get_us_volume_image(1, 4);
        const auto& expected2 = get_us_volume_image(2, 4);

        // Write a three 4 frames RGB uint8 image, with a custom filename
        {
            auto series_set = std::make_shared<data::series_set>();
            series_set->push_back(expected0);
            series_set->push_back(expected1);
            series_set->push_back(expected2);

            auto writer = std::make_shared<io::dicom::writer::file>();
            writer->set_object(series_set);
            writer->set_folder(tmp_dir);
            writer->set_file("custom_filename.dcm");

            CPPUNIT_ASSERT_NO_THROW(writer->write());
            CPPUNIT_ASSERT(std::filesystem::exists(tmp_dir / "000-custom_filename.dcm"));
            CPPUNIT_ASSERT(std::filesystem::exists(tmp_dir / "001-custom_filename.dcm"));
            CPPUNIT_ASSERT(std::filesystem::exists(tmp_dir / "002-custom_filename.dcm"));
        }
    }

    // test resized ImageSeries
    {
        core::os::temp_dir tmp_dir;
        const auto& expected = get_us_volume_image(2, 4);

        // Write a 4 frames RGB uint8 image, with a custom filename
        {
            auto series_set = std::make_shared<data::series_set>();

            auto resized = std::make_shared<data::image_series>();
            resized->deep_copy(expected);

            // Resize and set a value at the end...
            auto new_size = expected->size();
            new_size[2] += 1;
            resized->resize(new_size, expected->type(), expected->pixel_format());
            resized->set_frame_acquisition_date_time(
                format_date_time(std::chrono::system_clock::now()),
                new_size[2] - 1
            );

            // Resize back to original size
            resized->resize(expected->size(), expected->type(), expected->pixel_format());
            series_set->push_back(resized);

            auto writer = std::make_shared<io::dicom::writer::file>();
            writer->set_object(series_set);
            writer->set_folder(tmp_dir);
            writer->set_file("custom_filename.dcm");

            CPPUNIT_ASSERT_NO_THROW(writer->write());
            CPPUNIT_ASSERT(std::filesystem::exists(tmp_dir / "custom_filename.dcm"));
        }

        // Read the previously written 4 frames image
        {
            auto series_set = std::make_shared<data::series_set>();
            auto reader     = std::make_shared<io::dicom::reader::file>();
            reader->set_object(series_set);
            reader->set_folder(tmp_dir);

            CPPUNIT_ASSERT_NO_THROW(reader->read());
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

            compare_enhanced_us_volume(expected, std::dynamic_pointer_cast<data::image_series>(series_set->front()));
        }
    }
}

//------------------------------------------------------------------------------

void writer_test::force_cpu_test()
{
    core::os::temp_dir tmp_dir;
    const auto& expected = get_us_volume_image(0);

    // Write a single RGB uint8 frame image
    {
        auto series_set = std::make_shared<data::series_set>();
        series_set->push_back(expected);

        auto writer = std::make_shared<io::dicom::writer::file>();
        writer->set_object(series_set);
        writer->set_folder(tmp_dir);

        writer->force_cpu(false);

        if(io::bitmap::nv_jpeg_2k())
        {
            CPPUNIT_ASSERT_NO_THROW(writer->write());
        }

#ifdef SIGHT_ENABLE_NVJPEG2K
        else
        {
            CPPUNIT_ASSERT_THROW(writer->write(), core::exception);
        }
#endif

        writer->force_cpu(true);

        CPPUNIT_ASSERT_NO_THROW(writer->write());
    }
}

//------------------------------------------------------------------------------

void writer_test::transfer_syntax_test()
{
    const auto& test =
        [](io::dicom::writer::file::transfer_syntax _transfer_syntax)
        {
            core::os::temp_dir tmp_dir;
            const auto& expected = get_us_volume_image(0xFFFF, 6);

            // Write a 4 frames RGB uint8 image
            {
                auto series_set = std::make_shared<data::series_set>();
                series_set->push_back(expected);

                auto writer = std::make_shared<io::dicom::writer::file>();
                writer->set_object(series_set);
                writer->set_folder(tmp_dir);
                writer->set_transfer_syntax(_transfer_syntax);

                SIGHT_PROFILE_FUNC(
                    [&](std::size_t)
                {
                    CPPUNIT_ASSERT_NO_THROW(writer->write());
                },
                    3,
                    "Write (" + std::string(
                        io::dicom::writer::file::transfer_syntax_to_string(
                            _transfer_syntax
                        )
                    ) + "): ",
                    0.1
                );
            }

            // Read the previously written 4 frames image
            {
                auto series_set = std::make_shared<data::series_set>();
                auto reader     = std::make_shared<io::dicom::reader::file>();
                reader->set_object(series_set);
                reader->set_folder(tmp_dir);

                SIGHT_PROFILE_FUNC(
                    [&](std::size_t)
                {
                    CPPUNIT_ASSERT_NO_THROW(reader->read());
                },
                    3,
                    "Read (" + std::string(
                        io::dicom::writer::file::transfer_syntax_to_string(
                            _transfer_syntax
                        )
                    ) + "): ",
                    0.1
                );

                CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

                compare_enhanced_us_volume(
                    expected,
                    std::dynamic_pointer_cast<data::image_series>(series_set->front())
                );
            }

            for(auto const& entry : std::filesystem::directory_iterator {tmp_dir})
            {
                if(entry.is_regular_file())
                {
                    const auto size = entry.file_size();

                    SIGHT_INFO(
                        "File size (" << io::dicom::writer::file::transfer_syntax_to_string(
                            _transfer_syntax
                        ) << "): " << size
                    );

                    return size;
                }
            }

            return std::size_t(0);
        };

    // First the biggest file size
    const auto raw_size = test(io::dicom::writer::file::transfer_syntax::raw);

    for(const auto& transfer_syntax : {
            io::dicom::writer::file::transfer_syntax::rle,
            io::dicom::writer::file::transfer_syntax::jpeg_lossless,
            io::dicom::writer::file::transfer_syntax::jpeg_ls_lossless,
            io::dicom::writer::file::transfer_syntax::jpeg_2000_lossless
        })
    {
        // Then others (which should be smaller)
        const auto size = test(transfer_syntax);
        CPPUNIT_ASSERT_MESSAGE(
            "RAW (" + std::to_string(raw_size) + ") < "
            + std::string(io::dicom::writer::file::transfer_syntax_to_string(transfer_syntax))
            + " (" + std::to_string(size) + ")",
            raw_size >= size
        );
    }
}

} // namespace sight::io::dicom::ut
