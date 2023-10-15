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

#include <data/calibration_info.hpp>
#include <data/image.hpp>
#include <data/point.hpp>

namespace sight::io::session::detail::calibration_info
{

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::object::csptr object,
    std::map<std::string, data::object::csptr>& children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto calibrationInfo = helper::safe_cast<data::calibration_info>(object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::calibration_info>(tree, 1);

    // Images
    std::size_t index = 0;
    for(const auto& image : calibrationInfo->getImageContainer())
    {
        children[image->get_classname() + std::to_string(index++)] = image;
    }

    // PointLists
    index = 0;
    for(const auto& pointList : calibrationInfo->getPointListContainer())
    {
        children[pointList->get_classname() + std::to_string(index++)] = pointList;
    }
}

//------------------------------------------------------------------------------

inline static data::calibration_info::sptr read(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>& children,
    data::object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto calibrationInfo = helper::cast_or_create<data::calibration_info>(object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::calibration_info>(tree, 0, 1);

    // Deserialize children
    // Clearing is required in case the object is reused
    calibrationInfo->resetRecords();

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        // Find the image ad the associated pointList
        const std::string& indexString = std::to_string(index);
        const auto& imageIt            = children.find(data::image::classname() + indexString);
        const auto& pointListIt        = children.find(data::point_list::classname() + indexString);

        // If we didn't found a matching image and pointList, exit the loop
        if(imageIt == children.cend() || pointListIt == children.cend())
        {
            break;
        }

        auto image     = std::dynamic_pointer_cast<data::image>(imageIt->second);
        auto pointList = std::dynamic_pointer_cast<data::point_list>(pointListIt->second);

        calibrationInfo->addRecord(image, pointList);
    }

    return calibrationInfo;
}

SIGHT_REGISTER_SERIALIZER(data::calibration_info, write, read);

} // namespace sight::io::session::detail::calibration_info
