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

constexpr static auto SAFE_RESECTION {"SafeResection"};
constexpr static auto PLANE_LIST {"PlaneList"};
constexpr static auto IS_SAFE_PART {"IsSafePart"};
constexpr static auto IS_VALID {"IsValid"};
constexpr static auto IS_VISIBLE {"IsVisible"};

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& /*unused*/,
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
    _children[SAFE_RESECTION] = resection_db->get_safe_resection();

    std::size_t index = 0;
    for(const auto& resection : resection_db->get_resections())
    {
        _children[data::resection::classname() + std::to_string(index++)] = resection;
    }
}

//------------------------------------------------------------------------------

inline static data::resection_db::sptr read(
    zip::archive_reader& /*unused*/,
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

    resection_db->set_safe_resection(std::dynamic_pointer_cast<data::resection>(_children.at(SAFE_RESECTION)));

    // Clearing is required in case the object is reused
    resection_db->set_resections(data::resection_db::resection_container_t());

    for(std::size_t index = 0, end = _children.size() ; index < end ; ++index)
    {
        const auto& it = _children.find(data::resection::classname() + std::to_string(index));

        if(it == _children.cend())
        {
            break;
        }

        resection_db->add_resection(std::dynamic_pointer_cast<data::resection>(it->second));
    }

    return resection_db;
}

SIGHT_REGISTER_SERIALIZER(data::resection_db, write, read);

} // namespace sight::io::session::detail::resection_db
