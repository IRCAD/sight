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

#include <data/Node.hpp>
#include <data/ReconstructionTraits.hpp>
#include <data/StructureTraits.hpp>

namespace sight::io::session
{

namespace detail::ReconstructionTraits
{

constexpr static auto s_Identifier {"Identifier"};
constexpr static auto s_MaskOpNode {"MaskOpNode"};
constexpr static auto s_MeshOpNode {"MeshOpNode"};
constexpr static auto s_StructureTraits {"StructureTraits"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& password = ""
)
{
    const auto reconstructionTraits = Helper::safeCast<data::ReconstructionTraits>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::ReconstructionTraits>(tree, 1);

    // Serialize attributes
    Helper::writeString(tree, s_Identifier, reconstructionTraits->getIdentifier(), password);
    children[s_MaskOpNode]      = reconstructionTraits->getMaskOpNode();
    children[s_MeshOpNode]      = reconstructionTraits->getMeshOpNode();
    children[s_StructureTraits] = reconstructionTraits->getStructureTraits();
}

//------------------------------------------------------------------------------

inline static data::ReconstructionTraits::sptr deserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto reconstructionTraits = Helper::safeCast<data::ReconstructionTraits>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::ReconstructionTraits>(tree, 0, 1);

    // Deserialize attributes
    reconstructionTraits->setIdentifier(Helper::readString(tree, s_Identifier, password));
    reconstructionTraits->setMaskOpNode(std::dynamic_pointer_cast<data::Node>(children.at(s_MaskOpNode)));
    reconstructionTraits->setMeshOpNode(std::dynamic_pointer_cast<data::Node>(children.at(s_MeshOpNode)));
    reconstructionTraits->setStructureTraits(
        std::dynamic_pointer_cast<data::StructureTraits>(
            children.at(
                s_StructureTraits
            )
        )
    );

    return reconstructionTraits;
}

} // namespace detail::ReconstructionTraits

} // namespace sight::io
