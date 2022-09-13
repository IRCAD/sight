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

#include <data/Composite.hpp>

namespace sight::io::session::detail::Composite
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
    const auto composite = Helper::safe_cast<data::Composite>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Composite>(tree, 1);

    // Composite is map of child object..
    children = std::map<std::string, data::Object::csptr>(composite->cbegin(), composite->cend());
}

//------------------------------------------------------------------------------

inline static data::Composite::sptr deserialize(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto composite = Helper::cast_or_create<data::Composite>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Composite>(tree, 0, 1);

    composite->operator=(children);

    return composite;
}

} // namespace sight::io::session::detail::Composite
