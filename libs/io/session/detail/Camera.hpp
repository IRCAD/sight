/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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
#include "io/session/detail/Helper.hpp"

#include <data/Camera.hpp>

namespace sight::io::session
{

namespace detail::Camera
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
constexpr static auto s_Description {"Description"};
constexpr static auto s_CameraID {"CameraID"};
constexpr static auto s_MaximumFrameRate {"MaximumFrameRate"};
constexpr static auto s_PixelFormat {"PixelFormat"};
constexpr static auto s_VideoFile {"VideoFile"};
constexpr static auto s_StreamUrl {"StreamUrl"};
constexpr static auto s_CameraSource {"CameraSource"};
constexpr static auto s_Scale {"Scale"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& password = ""
)
{
    const auto camera = Helper::safeCast<data::Camera>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Camera>(tree, 1);

    tree.put(s_Width, camera->getWidth());
    tree.put(s_Height, camera->getHeight());

    tree.put(s_Fx, camera->getFx());
    tree.put(s_Fy, camera->getFy());
    tree.put(s_Cx, camera->getCx());
    tree.put(s_Cy, camera->getCy());

    const auto& distortionCoefficient = camera->getDistortionCoefficient();
    tree.put(s_K1, distortionCoefficient[0]);
    tree.put(s_K2, distortionCoefficient[1]);
    tree.put(s_P1, distortionCoefficient[2]);
    tree.put(s_P2, distortionCoefficient[3]);
    tree.put(s_K3, distortionCoefficient[4]);

    tree.put(s_Skew, camera->getSkew());

    tree.put(s_IsCalibrated, camera->getIsCalibrated());
    Helper::writeString(tree, s_Description, camera->getDescription(), password);
    Helper::writeString(tree, s_CameraID, camera->getCameraID(), password);
    tree.put(s_MaximumFrameRate, camera->getMaximumFrameRate());
    tree.put(s_PixelFormat, camera->getPixelFormat());
    Helper::writeString(tree, s_VideoFile, camera->getVideoFile().string(), password);
    Helper::writeString(tree, s_StreamUrl, camera->getStreamUrl(), password);
    tree.put(s_CameraSource, camera->getCameraSource());
    tree.put(s_Scale, camera->getScale());
}

//------------------------------------------------------------------------------

inline static data::Camera::sptr deserialize(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto camera = Helper::safeCast<data::Camera>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Camera>(tree, 0, 1);

    camera->setWidth(tree.get<size_t>(s_Width));
    camera->setHeight(tree.get<size_t>(s_Height));

    camera->setFx(tree.get<double>(s_Fx));
    camera->setFy(tree.get<double>(s_Fy));
    camera->setCx(tree.get<double>(s_Cx));
    camera->setCy(tree.get<double>(s_Cy));

    camera->setDistortionCoefficient(
        tree.get<double>(s_K1),
        tree.get<double>(s_K2),
        tree.get<double>(s_P1),
        tree.get<double>(s_P2),
        tree.get<double>(s_K3)
    );

    camera->setSkew(tree.get<double>(s_Skew));

    camera->setIsCalibrated(tree.get<bool>(s_IsCalibrated));
    camera->setDescription(Helper::readString(tree, s_Description, password));
    camera->setCameraID(Helper::readString(tree, s_CameraID, password));
    camera->setMaximumFrameRate(tree.get<float>(s_MaximumFrameRate));
    camera->setPixelFormat(static_cast<data::Camera::PixelFormat>(tree.get<int>(s_PixelFormat)));
    camera->setVideoFile(Helper::readString(tree, s_VideoFile, password));
    camera->setStreamUrl(Helper::readString(tree, s_StreamUrl, password));
    camera->setCameraSource(static_cast<data::Camera::SourceType>(tree.get<int>(s_CameraSource)));
    camera->setScale(tree.get<double>(s_Scale));

    return camera;
}

} // namespace detail::Camera

} // namespace sight::io
