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

#include <data/Resection.hpp>
#include <data/ResectionDB.hpp>

namespace sight::io::session::detail::ResectionDB
{

constexpr static auto s_SafeResection {"SafeResection"};
constexpr static auto s_PlaneList {"PlaneList"};
constexpr static auto s_IsSafePart {"IsSafePart"};
constexpr static auto s_IsValid {"IsValid"};
constexpr static auto s_IsVisible {"IsVisible"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto resectionDB = Helper::safe_cast<data::ResectionDB>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::ResectionDB>(tree, 1);

    // Serialize attributes
    children[s_SafeResection] = resectionDB->getSafeResection();

    std::size_t index = 0;
    for(const auto& resection : resectionDB->getResections())
    {
        children[data::Resection::classname() + std::to_string(index++)] = resection;
    }
}

//------------------------------------------------------------------------------

inline static data::ResectionDB::sptr deserialize(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto resectionDB = Helper::cast_or_create<data::ResectionDB>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::ResectionDB>(tree, 0, 1);

    resectionDB->setSafeResection(std::dynamic_pointer_cast<data::Resection>(children.at(s_SafeResection)));

    // Clearing is required in case the object is reused
    resectionDB->setResections(data::ResectionDB::ResectionContainerType());

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& it = children.find(data::Resection::classname() + std::to_string(index));

        if(it == children.cend())
        {
            break;
        }

        resectionDB->addResection(std::dynamic_pointer_cast<data::Resection>(it->second));
    }

    return resectionDB;
}

} // namespace sight::io::session::detail::ResectionDB
