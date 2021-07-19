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

#include <data/PlaneList.hpp>
#include <data/Point.hpp>

namespace sight::io::session
{

namespace detail::PlaneList
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
    const auto planeList = Helper::safeCast<data::PlaneList>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::PlaneList>(tree, 1);

    // Add points to children list
    std::size_t index = 0;
    for(const auto& plane : planeList->getPlanes())
    {
        children[data::Plane::classname() + std::to_string(index++)] = plane;
    }
}

//------------------------------------------------------------------------------

inline static data::PlaneList::sptr deserialize(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto planeList = Helper::safeCast<data::PlaneList>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::PlaneList>(tree, 0, 1);

    // Deserialize planes
    auto& planes = planeList->getPlanes();

    // Clearing is required in case the object is reused
    planes.clear();

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& it = children.find(data::Plane::classname() + std::to_string(index));

        if(it == children.cend())
        {
            break;
        }

        planes.push_back(data::Plane::dynamicCast(it->second));
    }

    return planeList;
}

} // namespace detail::PlaneList

} // namespace sight::io
