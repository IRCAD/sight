/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "helper.hpp"

#include <core/notification/observer.hpp>
#include <core/os/temp_path.hpp>
#include <core/tools/uuid.hpp>

#include <data/image_series.hpp>

#include <geometry/__/vector.hpp>

#include <io/dicom/reader/file.hpp>
#include <io/dicom/writer/file.hpp>

#include <utest_data/data.hpp>
#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

sight::data::series_set::sptr read(const std::filesystem::path& _path, bool _process_enhanced_us)
{
    CHECK(std::filesystem::exists(_path));

    auto series_set = std::make_shared<data::series_set>();

    auto reader = std::make_shared<io::dicom::reader::file>();
    reader->set_object(series_set);
    reader->set_folder(_path);

    const auto observer = std::make_shared<core::notification::observer>("DICOM Reader Test");
    CHECK_NOTHROW(reader->read(observer));

    if(_process_enhanced_us)
    {
        for(const auto& series : *series_set)
        {
            // Since we may modify the SOP to Enhanced US Volume, we need ensure each frame has a position
            if(series->get_ultrasound_acquisition_geometry() != data::dicom::ultrasound_acquisition_geometry_t::apex)
            {
                std::size_t end_index = series->num_frames();
                series->set_sop_keyword(data::dicom::sop::Keyword::EnhancedUSVolumeStorage);

                // We need to compute the frame position from image origin and z spacing
                for(std::size_t frame = 0 ; frame < end_index ; ++frame)
                {
                    if(series->get_image_position_patient(frame).empty())
                    {
                        series->set_image_position_patient({0, 0, 0}, frame);
                    }
                }
            }
        }
    }

    return series_set;
}

//------------------------------------------------------------------------------

void compare_enhanced_us_volume(
    const data::image_series::sptr& _expected,
    const data::image_series::sptr& _actual,
    bool _check_frame_metadata
)
{
    CHECK(_expected);
    CHECK(_actual);

    // SOP Class UID
    CHECK_EQ(_expected->get_sop_keyword(), _actual->get_sop_keyword());

    // Sizes
    const auto& expected_sizes = _expected->size();
    const auto& actual_sizes   = _actual->size();
    CHECK_EQ(expected_sizes.size(), actual_sizes.size());

    for(std::size_t i = 0 ; i < expected_sizes.size() ; ++i)
    {
        CHECK_EQ(expected_sizes[i], actual_sizes[i]);
    }

    // Type
    CHECK_EQ(_expected->type(), _actual->type());

    // Pixel format
    CHECK_EQ(_expected->pixel_format(), _actual->pixel_format());

    // Spacings
    const auto& expected_spacing = _expected->spacing();
    const auto& actual_spacing   = _actual->spacing();
    CHECK_EQ(expected_spacing.size(), actual_spacing.size());

    for(std::size_t i = 0 ; i < expected_spacing.size() ; ++i)
    {
        CHECK_LT(std::abs(expected_spacing[i] - actual_spacing[i]), 0.0001);
    }

    // Origins
    const auto& expected_origin = _expected->origin();
    const auto& actual_origin   = _actual->origin();
    CHECK_EQ(expected_origin.size(), actual_origin.size());

    for(std::size_t i = 0 ; i < expected_origin.size() ; ++i)
    {
        CHECK_LT(std::abs(expected_origin[i] - actual_origin[i]), 0.0001);
    }

    // Get Image Position Patient / Image Orientation Patient
    for(std::size_t frame_index = 0 ; frame_index < actual_sizes[2] ; ++frame_index)
    {
        // Image Position Patient
        const auto& expected_position = _expected->get_image_position_patient(frame_index);
        const auto& actual_position   = _actual->get_image_position_patient(frame_index);

        // Handle cases where one position might be empty
        if(expected_position.empty() && !actual_position.empty())
        {
            for(const auto& position : actual_position)
            {
                CHECK_LT(std::abs(position - 0.0), 0.0001);
            }
        }
        else if(!expected_position.empty() && actual_position.empty())
        {
            for(const auto& position : expected_position)
            {
                CHECK_LT(std::abs(position - 0.0), 0.0001);
            }
        }
        else
        {
            CHECK_EQ(expected_position.size(), actual_position.size());
            for(std::size_t i = 0 ; i < expected_position.size() ; ++i)
            {
                CHECK_LT(std::abs(expected_position[i] - actual_position[i]), 0.0001);
            }
        }

        // Image Orientation Patient
        const auto& expected_orientation = _expected->get_image_orientation_patient(frame_index);
        const auto& actual_orientation   = _actual->get_image_orientation_patient(frame_index);
        CHECK_EQ(expected_orientation.size(), actual_orientation.size());

        for(std::size_t i = 0 ; i < expected_orientation.size() ; ++i)
        {
            CHECK_LT(std::abs(expected_orientation[i] - actual_orientation[i]), 0.0001);
        }

        // Check frame metadata if requested
        if(_check_frame_metadata)
        {
            // FrameAcquisitionDateTime
            CHECK_EQ(
                *_expected->get_frame_acquisition_date_time(frame_index),
                *_actual->get_frame_acquisition_date_time(frame_index)
            );

            // Private custom attribute
            CHECK_EQ(
                *_expected->get_multi_frame_private_value(0x42, frame_index),
                *_actual->get_multi_frame_private_value(0x42, frame_index)
            );
        }
    }

    // Ensure that getting value outside the frame range returns std::nullopts
    CHECK(!_expected->get_frame_acquisition_date_time(actual_sizes[2]));
    CHECK(!_actual->get_frame_acquisition_date_time(actual_sizes[2]));

    // Compare buffer
    const auto expected_locked = _expected->dump_lock();
    const auto actual_locked   = _actual->dump_lock();
    CHECK_EQ(0, std::memcmp(_expected->buffer(), _actual->buffer(), _expected->size_in_bytes()));
}

//------------------------------------------------------------------------------

void compare_enhanced_us_volume(
    const data::series_set::sptr& _expected,
    const data::series_set::sptr& _actual,
    bool _check_frame_metadata
)
{
    CHECK_EQ(_expected->size(), _actual->size());
    for(std::size_t i = 0 ; i < _expected->size() ; i++)
    {
        const auto& expected_image_series = std::dynamic_pointer_cast<data::image_series>((*_expected)[i]);
        const auto& actual_image_series   = std::dynamic_pointer_cast<data::image_series>((*_actual)[i]);
        compare_enhanced_us_volume(expected_image_series, actual_image_series, _check_frame_metadata);
    }
}

//------------------------------------------------------------------------------

void test_image(const std::string& _name)
{
    const core::os::temp_dir folder;
    const auto& expected = read(utest_data::dir() / _name, true);

    auto writer = std::make_shared<io::dicom::writer::file>();
    writer->set_object(expected);
    writer->set_folder(folder);
    auto write_observer = std::make_shared<core::notification::observer>("Test write");
    CHECK_NOTHROW(writer->write(write_observer));

    auto actual = std::make_shared<data::series_set>();
    auto reader = std::make_shared<io::dicom::reader::file>();
    reader->set_object(actual);
    reader->set_folder(folder);
    auto read_observer = std::make_shared<core::notification::observer>("Test read");
    CHECK_NOTHROW(reader->read(read_observer));

    compare_enhanced_us_volume(expected, actual);
}

//------------------------------------------------------------------------------

std::string format_date_time(const std::chrono::time_point<std::chrono::system_clock>& _now)
{
    time_t now_time = std::chrono::system_clock::to_time_t(_now);

    struct tm timeinfo {};

#ifdef _WIN32
    if(const auto result = localtime_s(&timeinfo, &now_time); result != 0)
    {
        SIGHT_THROW("localtime_s failed");
    }
#else
    if(const auto* result = localtime_r(&now_time, &timeinfo); result == nullptr)
    {
        SIGHT_THROW("localtime_r failed");
    }
#endif

    const auto truncated_now = std::chrono::system_clock::from_time_t(now_time);
    const auto ms            = (_now - truncated_now).count();

    std::string buffer("YYYYMMDDHHMMSS.");
    const auto size = strftime(buffer.data(), buffer.size(), "%Y%m%d%H%M%S", &timeinfo);
    buffer.resize(size);

    return buffer + "." + std::to_string(ms).substr(0, 6);
}

//------------------------------------------------------------------------------

data::image_series::sptr get_us_volume_image(
    std::uint32_t _seed,
    std::size_t _num_frames,
    core::type _type,
    enum data::image::pixel_format_t _format,
    bool _copy
)
{
    using key_t = std::tuple<std::size_t, core::type, enum data::image::pixel_format_t, std::uint32_t>;
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
            {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6},
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
        image->set_sop_instance_uid(core::tools::uuid::generate());

        // Set Image Position Patient / Image Orientation Patient
        for(std::size_t frame_index = 0 ; frame_index < _num_frames ; ++frame_index)
        {
            // ..Image Position / Orientation Patient is what we want
            image->set_image_position_patient(
                {
                    static_cast<double>(_seed + 1) * 0.1,
                    static_cast<double>(_seed + 1) * 0.2,
                    static_cast<double>(_seed + 1) * 0.3
                },
                frame_index
            );

            glm::dvec3 u = {static_cast<double>(_seed + 1) * 0.4, static_cast<double>(_seed + 1) * 0.5,
                            static_cast<double>(_seed + 1) * 0.6
            };
            glm::dvec3 v = {static_cast<double>(_seed + 1) * 0.7, static_cast<double>(_seed + 1) * 0.8,
                            static_cast<double>(_seed + 1) * 0.9
            };

            // We really want orthogonal directions
            geometry::orthogonalize(u, v);
            image->set_image_orientation_patient({u[0], u[1], u[2], v[0], v[1], v[2]}, frame_index);

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

        if(_copy)
        {
            auto copy = std::make_shared<data::image_series>();
            copy->deep_copy(image);
            copy->set_sop_instance_uid(core::tools::uuid::generate());

            return copy;
        }

        return image;
    }

    if(_copy)
    {
        auto copy = std::make_shared<data::image_series>();
        copy->deep_copy(it->second);
        copy->set_sop_instance_uid(core::tools::uuid::generate());

        return copy;
    }

    return it->second;
}

//------------------------------------------------------------------------------

data::image_series::sptr get_us_volume_image_copy(
    std::uint32_t _seed,
    std::size_t _num_frames,
    core::type _type,
    enum data::image::pixel_format_t _format
)
{
    return get_us_volume_image(_seed, _num_frames, _type, _format, true);
}

} // namespace sight::io::dicom::ut
