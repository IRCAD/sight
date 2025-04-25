/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

#include <sight/io/session/config.hpp>

#include "io/session/helper.hpp"
#include "io/session/macros.hpp"

#include <data/Node.hpp>
#include <data/reconstruction_traits.hpp>
#include <data/structure_traits.hpp>

namespace sight::io::session::detail::reconstruction_traits
{

constexpr static auto s_Identifier {"Identifier"};
constexpr static auto s_MaskOpNode {"MaskOpNode"};
constexpr static auto s_MeshOpNode {"MeshOpNode"};
constexpr static auto s_StructureTraits {"structure_traits"};

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& /*unused*/,
    boost::property_tree::ptree& tree,
    data::object::csptr object,
    std::map<std::string, data::object::csptr>& children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto reconstructionTraits = helper::safe_cast<data::reconstruction_traits>(object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::reconstruction_traits>(tree, 1);

    // Serialize attributes
    helper::write_string(tree, s_Identifier, reconstructionTraits->getIdentifier());
    children[s_MaskOpNode]      = reconstructionTraits->getMaskOpNode();
    children[s_MeshOpNode]      = reconstructionTraits->getMeshOpNode();
    children[s_StructureTraits] = reconstructionTraits->getStructureTraits();
}

//------------------------------------------------------------------------------

inline static data::reconstruction_traits::sptr read(
    zip::archive_reader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>& children,
    data::object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto reconstructionTraits = helper::cast_or_create<data::reconstruction_traits>(object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::reconstruction_traits>(tree, 0, 1);

    // Deserialize attributes
    reconstructionTraits->setIdentifier(helper::read_string(tree, s_Identifier));
    reconstructionTraits->setMaskOpNode(std::dynamic_pointer_cast<data::Node>(children.at(s_MaskOpNode)));
    reconstructionTraits->setMeshOpNode(std::dynamic_pointer_cast<data::Node>(children.at(s_MeshOpNode)));
    reconstructionTraits->setStructureTraits(
        std::dynamic_pointer_cast<data::structure_traits>(
            children.at(
                s_StructureTraits
            )
        )
    );

    return reconstructionTraits;
}

SIGHT_REGISTER_SERIALIZER(data::reconstruction_traits, write, read);

} // namespace sight::io::session::detail::reconstruction_traits
