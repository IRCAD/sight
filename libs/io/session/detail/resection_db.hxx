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

#include <data/resection.hpp>
#include <data/resection_db.hpp>

namespace sight::io::session::detail::resection_db
{

constexpr static auto s_SafeResection {"SafeResection"};
constexpr static auto s_PlaneList {"PlaneList"};
constexpr static auto s_IsSafePart {"IsSafePart"};
constexpr static auto s_IsValid {"IsValid"};
constexpr static auto s_IsVisible {"IsVisible"};

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& _children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto resection_db = helper::safe_cast<data::resection_db>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::resection_db>(_tree, 1);

    // Serialize attributes
    _children[s_SafeResection] = resection_db->getSafeResection();

    std::size_t index = 0;
    for(const auto& resection : resection_db->getResections())
    {
        _children[data::resection::classname() + std::to_string(index++)] = resection;
    }
}

//------------------------------------------------------------------------------

inline static data::resection_db::sptr read(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& _children,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto resection_db = helper::cast_or_create<data::resection_db>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::resection_db>(_tree, 0, 1);

    resection_db->setSafeResection(std::dynamic_pointer_cast<data::resection>(_children.at(s_SafeResection)));

    // Clearing is required in case the object is reused
    resection_db->setResections(data::resection_db::resection_container_t());

    for(std::size_t index = 0, end = _children.size() ; index < end ; ++index)
    {
        const auto& it = _children.find(data::resection::classname() + std::to_string(index));

        if(it == _children.cend())
        {
            break;
        }

        resection_db->addResection(std::dynamic_pointer_cast<data::resection>(it->second));
    }

    return resection_db;
}

SIGHT_REGISTER_SERIALIZER(data::resection_db, write, read);

} // namespace sight::io::session::detail::resection_db
