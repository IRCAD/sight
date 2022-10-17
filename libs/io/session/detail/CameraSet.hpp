/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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
#include "io/session/Helper.hpp"

#include <data/Camera.hpp>
#include <data/CameraSet.hpp>
#include <data/Matrix4.hpp>

namespace sight::io::session::detail::CameraSet
{

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto camera_set = Helper::safe_cast<data::CameraSet>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::CameraSet>(tree, 1);

    for(std::size_t index = 0, end = camera_set->size() ; index < end ; ++index)
    {
        const auto& pair = camera_set->at(index);

        const std::string& index_string = std::to_string(index);
        children[data::CameraSet::classname() + data::Camera::classname() + index_string]  = pair.first;
        children[data::CameraSet::classname() + data::Matrix4::classname() + index_string] = pair.second;
    }
}

//------------------------------------------------------------------------------

inline static data::CameraSet::sptr deserialize(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto camera_set = Helper::cast_or_create<data::CameraSet>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::CameraSet>(tree, 0, 1);

    // Deserialize vector
    // Clearing is required in case the object is reused
    camera_set->clear();

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const std::string& index_string = std::to_string(index);

        const auto& camera_it = children.find(data::CameraSet::classname() + data::Camera::classname() + index_string);

        if(camera_it == children.cend())
        {
            break;
        }

        const auto& camera = std::dynamic_pointer_cast<data::Camera>(camera_it->second);

        if(!camera)
        {
            break;
        }

        const auto& matrix_it = children.find(data::CameraSet::classname() + data::Matrix4::classname() + index_string);

        if(matrix_it == children.cend())
        {
            break;
        }

        const auto& matrix = std::dynamic_pointer_cast<data::Matrix4>(matrix_it->second);

        if(!matrix)
        {
            break;
        }

        camera_set->push_back(std::make_pair(camera, matrix));
    }

    return camera_set;
}

} // namespace sight::io::session::detail::CameraSet
