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

#include <data/CalibrationInfo.hpp>
#include <data/Image.hpp>
#include <data/Point.hpp>

namespace sight::io::session
{

namespace detail::CalibrationInfo
{

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& = ""
)
{
    const auto calibrationInfo = Helper::safeCast<data::CalibrationInfo>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::CalibrationInfo>(tree, 1);

    // Images
    std::size_t index = 0;
    for(const auto& image : calibrationInfo->getImageContainer())
    {
        children[image->getClassname() + std::to_string(index++)] = image;
    }

    // PointLists
    index = 0;
    for(const auto& pointList : calibrationInfo->getPointListContainer())
    {
        children[pointList->getClassname() + std::to_string(index++)] = pointList;
    }
}

//------------------------------------------------------------------------------

inline static data::CalibrationInfo::sptr deserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& = ""
)
{
    // Create or reuse the object
    auto calibrationInfo = Helper::safeCast<data::CalibrationInfo>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::CalibrationInfo>(tree, 0, 1);

    // Deserialize children
    // Clearing is required in case the object is reused
    calibrationInfo->resetRecords();

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        // Find the image ad the associated pointList
        const std::string& indexString = std::to_string(index);
        const auto& imageIt            = children.find(data::Image::classname() + indexString);
        const auto& pointListIt        = children.find(data::PointList::classname() + indexString);

        // If we didn't found a matching image and pointList, exit the loop
        if(imageIt == children.cend() || pointListIt == children.cend())
        {
            break;
        }

        auto image     = std::dynamic_pointer_cast<data::Image>(imageIt->second);
        auto pointList = std::dynamic_pointer_cast<data::PointList>(pointListIt->second);

        calibrationInfo->addRecord(image, pointList);
    }

    return calibrationInfo;
}

} // namespace detail::CalibrationInfo

} // namespace sight::io
