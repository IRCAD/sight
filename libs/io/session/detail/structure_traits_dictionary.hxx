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

#include <data/color.hpp>
#include <data/structure_traits_dictionary.hpp>

namespace sight::io::session::detail::structure_traits_dictionary
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
    const auto structureTraitsDictionary = helper::safe_cast<data::structure_traits_dictionary>(object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::structure_traits_dictionary>(tree, 1);

    // Serialize attributes
    for(const auto& name : structureTraitsDictionary->getStructureTypeNames())
    {
        children[name] = structureTraitsDictionary->getStructure(name);
    }
}

//------------------------------------------------------------------------------

inline static data::structure_traits_dictionary::sptr read(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>& children,
    data::object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto structureTraitsDictionary = helper::cast_or_create<data::structure_traits_dictionary>(object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::structure_traits_dictionary>(tree, 0, 1);

    // Deserialize attributes
    std::map<std::string, data::structure_traits::sptr> structures;

    for(const auto& child : children)
    {
        auto structure = std::dynamic_pointer_cast<data::structure_traits>(child.second);
        if(structure)
        {
            structures[child.first] = structure;
        }
    }

    structureTraitsDictionary->setStructureTraitsMap(structures);
    return structureTraitsDictionary;
}

SIGHT_REGISTER_SERIALIZER(data::structure_traits_dictionary, write, read);

} // namespace sight::io::session::detail::structure_traits_dictionary
