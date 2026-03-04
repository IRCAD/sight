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

#pragma once

#include <core/type.hpp>

#include <data/image_series.hpp>
#include <data/series_set.hpp>

namespace sight::io::dicom::ut
{

/// Read DICOM files from a path and return a series_set
/// @param _path Path to the DICOM folder
/// @param _process_enhanced_us If true, process Enhanced US Volume (ensure frame positions)
/// @return The loaded series_set
sight::data::series_set::sptr read(
    const std::filesystem::path& _path,
    bool _process_enhanced_us = false
);

/// Compare two Enhanced US Volume image series
/// @param _expected Expected image series
/// @param _actual Actual image series
/// @param _check_frame_metadata If true, check frame acquisition date/time and private values
void compare_enhanced_us_volume(
    const data::image_series::sptr& _expected,
    const data::image_series::sptr& _actual,
    bool _check_frame_metadata = false
);

/// Compare two series sets containing Enhanced US Volume image series
/// @param _expected Expected series set
/// @param _actual Actual series set
/// @param _check_frame_metadata If true, check frame acquisition date/time and private values
void compare_enhanced_us_volume(
    const data::series_set::sptr& _expected,
    const data::series_set::sptr& _actual,
    bool _check_frame_metadata = false
);

/// Test reading and writing a DICOM image
/// @param _name Path relative to utest_data::dir()
void test_image(const std::string& _name);

/// Format a date/time to DICOM DateTime format (YYYYMMDDHHMMSS.FFFFFF)
/// @param _now Time point to format
/// @return Formatted date/time string
std::string format_date_time(const std::chrono::time_point<std::chrono::system_clock>& _now);

/// Generate a test US volume image
/// @param _seed Random seed
/// @param _num_frames Number of frames
/// @param _type Pixel data type
/// @param _format Pixel format
/// @return Generated image series
data::image_series::sptr get_us_volume_image(
    std::uint32_t _seed                      = 0,
    std::size_t _num_frames                  = 1,
    core::type _type                         = core::type::UINT8,
    enum data::image::pixel_format_t _format = data::image::rgb,
    bool _copy                               = false
);

data::image_series::sptr get_us_volume_image_copy(
    std::uint32_t _seed                      = 0,
    std::size_t _num_frames                  = 1,
    core::type _type                         = core::type::UINT8,
    enum data::image::pixel_format_t _format = data::image::rgb
);

} // namespace sight::io::dicom::ut
