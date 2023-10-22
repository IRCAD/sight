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

constexpr static auto s_Width {"Width"};
constexpr static auto s_Height {"Height"};
constexpr static auto s_Fx {"Fx"};
constexpr static auto s_Fy {"Fy"};
constexpr static auto s_Cx {"Cx"};
constexpr static auto s_Cy {"Cy"};
constexpr static auto s_K1 {"K1"};
constexpr static auto s_K2 {"K2"};
constexpr static auto s_P1 {"P1"};
constexpr static auto s_P2 {"P2"};
constexpr static auto s_K3 {"K3"};
constexpr static auto s_Skew {"Skew"};
constexpr static auto s_IsCalibrated {"IsCalibrated"};
constexpr static auto s_cameraID {"cameraID"};
constexpr static auto s_MaximumFrameRate {"MaximumFrameRate"};
constexpr static auto s_PixelFormat {"PixelFormat"};
constexpr static auto s_VideoFile {"VideoFile"};
constexpr static auto s_StreamUrl {"StreamUrl"};
constexpr static auto s_cameraSource {"cameraSource"};
constexpr static auto s_Scale {"Scale"};

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto camera = helper::safe_cast<data::camera>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::camera>(_tree, 1);

    _tree.put(s_Width, camera->getWidth());
    _tree.put(s_Height, camera->getHeight());

    _tree.put(s_Fx, camera->getFx());
    _tree.put(s_Fy, camera->getFy());
    _tree.put(s_Cx, camera->getCx());
    _tree.put(s_Cy, camera->getCy());

    const auto& distortion_coefficient = camera->getDistortionCoefficient();
    _tree.put(s_K1, distortion_coefficient[0]);
    _tree.put(s_K2, distortion_coefficient[1]);
    _tree.put(s_P1, distortion_coefficient[2]);
    _tree.put(s_P2, distortion_coefficient[3]);
    _tree.put(s_K3, distortion_coefficient[4]);

    _tree.put(s_Skew, camera->getSkew());

    _tree.put(s_IsCalibrated, camera->getIsCalibrated());
    helper::write_string(_tree, s_cameraID, camera->getCameraID());
    _tree.put(s_MaximumFrameRate, camera->getMaximumFrameRate());
    _tree.put(s_PixelFormat, camera->getPixelFormat());
    helper::write_string(_tree, s_VideoFile, camera->getVideoFile().string());
    helper::write_string(_tree, s_StreamUrl, camera->getStreamUrl());
    _tree.put(s_cameraSource, camera->getCameraSource());
    _tree.put(s_Scale, camera->getScale());
}

//------------------------------------------------------------------------------

inline static data::camera::sptr read(
    zip::ArchiveReader& /*unused*/,
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

    camera->setWidth(_tree.get<std::size_t>(s_Width));
    camera->setHeight(_tree.get<std::size_t>(s_Height));

    camera->setFx(_tree.get<double>(s_Fx));
    camera->setFy(_tree.get<double>(s_Fy));
    camera->setCx(_tree.get<double>(s_Cx));
    camera->setCy(_tree.get<double>(s_Cy));

    camera->setDistortionCoefficient(
        _tree.get<double>(s_K1),
        _tree.get<double>(s_K2),
        _tree.get<double>(s_P1),
        _tree.get<double>(s_P2),
        _tree.get<double>(s_K3)
    );

    camera->setSkew(_tree.get<double>(s_Skew));

    camera->setIsCalibrated(_tree.get<bool>(s_IsCalibrated));
    camera->setCameraID(helper::read_string(_tree, s_cameraID));
    camera->setMaximumFrameRate(_tree.get<float>(s_MaximumFrameRate));
    camera->setPixelFormat(static_cast<data::camera::PixelFormat>(_tree.get<int>(s_PixelFormat)));
    camera->setVideoFile(helper::read_string(_tree, s_VideoFile));
    camera->setStreamUrl(helper::read_string(_tree, s_StreamUrl));
    camera->setCameraSource(static_cast<data::camera::source_t>(_tree.get<int>(s_cameraSource)));
    camera->setScale(_tree.get<double>(s_Scale));

    return camera;
}

SIGHT_REGISTER_SERIALIZER(data::camera, write, read);

} // namespace sight::io::session::detail::camera
