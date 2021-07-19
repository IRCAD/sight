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

#include <data/Equipment.hpp>

namespace sight::io::session
{

namespace detail::Equipment
{

constexpr static auto s_InstitutionName {"InstitutionName"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& password = ""
)
{
    const auto equipment = Helper::safeCast<data::Equipment>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Equipment>(tree, 1);

    Helper::writeString(tree, s_InstitutionName, equipment->getInstitutionName(), password);
}

//------------------------------------------------------------------------------

inline static data::Equipment::sptr deserialize(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto equipment = Helper::safeCast<data::Equipment>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Equipment>(tree, 0, 1);

    equipment->setInstitutionName(Helper::readString(tree, s_InstitutionName, password));

    return equipment;
}

} // namespace detail::Equipment

} // namespace sight::io
