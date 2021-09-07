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
#include "io/session/detail/Helper.hpp"

#include <data/Color.hpp>
#include <data/StructureTraitsDictionary.hpp>

namespace sight::io::session
{

namespace detail::StructureTraitsDictionary
{

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& = ""
)
{
    const auto structureTraitsDictionary = Helper::safeCast<data::StructureTraitsDictionary>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::StructureTraitsDictionary>(tree, 1);

    // Serialize attributes
    for(const auto& name : structureTraitsDictionary->getStructureTypeNames())
    {
        children[name] = structureTraitsDictionary->getStructure(name);
    }
}

//------------------------------------------------------------------------------

inline static data::StructureTraitsDictionary::sptr deserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& = ""
)
{
    // Create or reuse the object
    auto structureTraitsDictionary = Helper::safeCast<data::StructureTraitsDictionary>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::StructureTraitsDictionary>(tree, 0, 1);

    // Deserialize attributes
    std::map<std::string, data::StructureTraits::sptr> structures;

    for(const auto& child : children)
    {
        auto structure = std::dynamic_pointer_cast<data::StructureTraits>(child.second);
        if(structure)
        {
            structures[child.first] = structure;
        }
    }

    structureTraitsDictionary->setStructureTraitsMap(structures);
    return structureTraitsDictionary;
}

} // namespace detail::StructureTraitsDictionary

} // namespace sight::io
