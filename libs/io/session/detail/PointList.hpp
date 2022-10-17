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

#include <data/Point.hpp>
#include <data/PointList.hpp>

namespace sight::io::session::detail::PointList
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
    const auto pointList = Helper::safe_cast<data::PointList>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::PointList>(tree, 1);

    // Add points to children list
    int index = 0;
    for(const auto& point : pointList->getPoints())
    {
        children[data::Point::classname() + std::to_string(index++)] = point;
    }
}

//------------------------------------------------------------------------------

inline static data::PointList::sptr deserialize(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto pointList = Helper::cast_or_create<data::PointList>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::PointList>(tree, 0, 1);

    // Deserialize points
    // Clearing is required in case the object is reused
    pointList->getPoints().clear();

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& it = children.find(data::Point::classname() + std::to_string(index));

        if(it == children.cend())
        {
            break;
        }

        pointList->pushBack(Helper::cast_or_create<data::Point>(it->second));
    }

    return pointList;
}

} // namespace sight::io::session::detail::PointList
