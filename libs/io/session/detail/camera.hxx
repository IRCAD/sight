/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "io/session/config.hpp"
#include "io/session/helper.hpp"
#include "io/session/macros.hpp"

#include <data/camera.hpp>

namespace sight::io::session::detail::camera
{

constexpr static auto WIDTH {"Width"};
constexpr static auto HEIGHT {"Height"};
constexpr static auto FX {"Fx"};
constexpr static auto FY {"Fy"};
constexpr static auto CX {"Cx"};
constexpr static auto CY {"Cy"};
constexpr static auto K1 {"K1"};
constexpr static auto K2 {"K2"};
constexpr static auto P1 {"P1"};
constexpr static auto P2 {"P2"};
constexpr static auto K3 {"K3"};
constexpr static auto SKEW {"Skew"};
constexpr static auto IS_CALIBRATED {"IsCalibrated"};
constexpr static auto CAMERA_ID {"cameraID"};
constexpr static auto MAXIMUM_FRAME_RATE {"MaximumFrameRate"};
constexpr static auto PIXEL_FORMAT {"PixelFormat"};
constexpr static auto VIDEO_FILE {"VideoFile"};
constexpr static auto STREAM_URL {"StreamUrl"};
constexpr static auto CAMERA_SOURCE {"cameraSource"};
constexpr static auto SCALE {"Scale"};

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto camera = helper::safe_cast<data::camera>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::camera>(_tree, 1);

    _tree.put(WIDTH, camera->get_width());
    _tree.put(HEIGHT, camera->get_height());

    _tree.put(FX, camera->get_fx());
    _tree.put(FY, camera->get_fy());
    _tree.put(CX, camera->get_cx());
    _tree.put(CY, camera->get_cy());

    const auto& distortion_coefficient = camera->get_distortion_coefficient();
    _tree.put(K1, distortion_coefficient[0]);
    _tree.put(K2, distortion_coefficient[1]);
    _tree.put(P1, distortion_coefficient[2]);
    _tree.put(P2, distortion_coefficient[3]);
    _tree.put(K3, distortion_coefficient[4]);

    _tree.put(SKEW, camera->get_skew());

    _tree.put(IS_CALIBRATED, camera->get_is_calibrated());
    helper::write_string(_tree, CAMERA_ID, camera->get_camera_id());
    _tree.put(MAXIMUM_FRAME_RATE, camera->get_maximum_frame_rate());
    _tree.put(PIXEL_FORMAT, camera->pixel_format());
    helper::write_string(_tree, VIDEO_FILE, camera->get_video_file().string());
    helper::write_string(_tree, STREAM_URL, camera->get_stream_url());
    _tree.put(CAMERA_SOURCE, camera->get_camera_source());
    _tree.put(SCALE, camera->get_scale());
}

//------------------------------------------------------------------------------

inline static data::camera::sptr read(
    zip::archive_reader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& /*unused*/,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto camera = helper::cast_or_create<data::camera>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::camera>(_tree, 0, 1);

    camera->set_width(_tree.get<std::size_t>(WIDTH));
    camera->set_height(_tree.get<std::size_t>(HEIGHT));

    camera->set_fx(_tree.get<double>(FX));
    camera->set_fy(_tree.get<double>(FY));
    camera->set_cx(_tree.get<double>(CX));
    camera->set_cy(_tree.get<double>(CY));

    camera->set_distortion_coefficient(
        _tree.get<double>(K1),
        _tree.get<double>(K2),
        _tree.get<double>(P1),
        _tree.get<double>(P2),
        _tree.get<double>(K3)
    );

    camera->set_skew(_tree.get<double>(SKEW));

    camera->set_is_calibrated(_tree.get<bool>(IS_CALIBRATED));
    camera->set_camera_id(helper::read_string(_tree, CAMERA_ID));
    camera->set_maximum_frame_rate(_tree.get<float>(MAXIMUM_FRAME_RATE));
    camera->set_pixel_format(static_cast<enum data::camera::pixel_format>(_tree.get<int>(PIXEL_FORMAT)));
    camera->set_video_file(helper::read_string(_tree, VIDEO_FILE));
    camera->set_stream_url(helper::read_string(_tree, STREAM_URL));
    camera->set_camera_source(static_cast<data::camera::source_t>(_tree.get<int>(CAMERA_SOURCE)));
    camera->set_scale(_tree.get<double>(SCALE));

    return camera;
}

SIGHT_REGISTER_SERIALIZER(data::camera, write, read);

} // namespace sight::io::session::detail::camera
