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

#include <data/Resection.hpp>
#include <data/ResectionDB.hpp>

namespace sight::io::session
{

namespace detail::ResectionDB
{

constexpr static auto s_SafeResection {"SafeResection"};
constexpr static auto s_PlaneList {"PlaneList"};
constexpr static auto s_IsSafePart {"IsSafePart"};
constexpr static auto s_IsValid {"IsValid"};
constexpr static auto s_IsVisible {"IsVisible"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& password = ""
)
{
    const auto resectionDB = Helper::safeCast<data::ResectionDB>(object);

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
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto resectionDB = Helper::safeCast<data::ResectionDB>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::ResectionDB>(tree, 0, 1);

    resectionDB->setSafeResection(data::Resection::dynamicCast(children.at(s_SafeResection)));

    // Clearing is required in case the object is reused
    resectionDB->setResections(data::ResectionDB::ResectionContainerType());

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& it = children.find(data::Resection::classname() + std::to_string(index));

        if(it == children.cend())
        {
            break;
        }

        resectionDB->addResection(data::Resection::dynamicCast(it->second));
    }

    return resectionDB;
}

} // namespace detail::ResectionDB

} // namespace sight::io
