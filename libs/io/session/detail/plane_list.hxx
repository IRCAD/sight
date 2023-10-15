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

#include <data/plane_list.hpp>
#include <data/point.hpp>

namespace sight::io::session::detail::plane_list
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
    const auto planeList = helper::safe_cast<data::plane_list>(object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::plane_list>(tree, 1);

    // Add points to children list
    std::size_t index = 0;
    for(const auto& plane : planeList->getPlanes())
    {
        children[data::plane::classname() + std::to_string(index++)] = plane;
    }
}

//------------------------------------------------------------------------------

inline static data::plane_list::sptr read(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>& children,
    data::object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto planeList = helper::cast_or_create<data::plane_list>(object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::plane_list>(tree, 0, 1);

    // Deserialize planes
    auto& planes = planeList->getPlanes();

    // Clearing is required in case the object is reused
    planes.clear();

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& it = children.find(data::plane::classname() + std::to_string(index));

        if(it == children.cend())
        {
            break;
        }

        planes.push_back(std::dynamic_pointer_cast<data::plane>(it->second));
    }

    return planeList;
}

SIGHT_REGISTER_SERIALIZER(data::plane_list, write, read);

} // namespace sight::io::session::detail::plane_list
