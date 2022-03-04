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
#include "io/session/detail/Series.hpp"
#include "io/session/Helper.hpp"

#include <data/Camera.hpp>
#include <data/CameraSeries.hpp>

namespace sight::io::session
{

namespace detail::CameraSeries
{

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& password = ""
)
{
    const auto cameraSeries = Helper::safeCast<data::CameraSeries>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::CameraSeries>(tree, 1);

    // Since CameraSeries inherits from Series, we could use SeriesSerializer
    Series::serialize(archive, tree, cameraSeries, children, password);

    // Serialize the children camera
    for(std::size_t index = 0, end = cameraSeries->numCameras() ; index < end ; ++index)
    {
        children[data::Camera::classname() + std::to_string(index)] = cameraSeries->getCamera(index);
    }
}

//------------------------------------------------------------------------------

inline static data::CameraSeries::sptr deserialize(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto cameraSeries = Helper::safeCast<data::CameraSeries>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::CameraSeries>(tree, 0, 1);

    // Since CameraSeries inherits from Series, we could use SeriesDeserializer
    Series::deserialize(archive, tree, children, cameraSeries, password);

    // Deserialize children
    // Clearing is required in case the object is reused
    cameraSeries->clearCameras();

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& cameraIt = children.find(data::Camera::classname() + std::to_string(index));

        // If we didn't found a matching camera, exit the loop
        if(cameraIt == children.cend())
        {
            break;
        }

        cameraSeries->addCamera(std::dynamic_pointer_cast<data::Camera>(cameraIt->second));
    }

    return cameraSeries;
}

} // namespace detail::CameraSeries

} // namespace sight::io
