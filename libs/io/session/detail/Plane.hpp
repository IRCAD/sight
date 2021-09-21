/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include <data/Plane.hpp>
#include <data/Point.hpp>

namespace sight::io::session
{

namespace detail::Plane
{

constexpr static auto s_IsIntersection {"IsIntersection"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& = ""
)
{
    const auto plane = Helper::safeCast<data::Plane>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Plane>(tree, 1);

    tree.put(s_IsIntersection, plane->getIsIntersection());

    std::size_t index = 0;
    for(const auto& point : plane->getPoints())
    {
        children[data::Point::classname() + std::to_string(index++)] = point;
    }
}

//------------------------------------------------------------------------------

inline static data::Plane::sptr deserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& = ""
)
{
    // Create or reuse the object
    auto plane = Helper::safeCast<data::Plane>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Plane>(tree, 0, 1);

    plane->setIsIntersection(tree.get<bool>(s_IsIntersection));

    auto& points = plane->getPoints();

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& it = children.find(data::Point::classname() + std::to_string(index));

        if(it == children.cend())
        {
            break;
        }

        points[index] = std::dynamic_pointer_cast<data::Point>(it->second);
    }

    return plane;
}

} // namespace detail::Plane

} // namespace sight::io
