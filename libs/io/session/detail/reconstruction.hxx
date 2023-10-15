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

#include <data/image.hpp>
#include <data/material.hpp>
#include <data/mesh.hpp>
#include <data/reconstruction.hpp>

namespace sight::io::session::detail::reconstruction
{

constexpr static auto s_IsVisible {"IsVisible"};
constexpr static auto s_OrganName {"OrganName"};
constexpr static auto s_StructureType {"StructureType"};
constexpr static auto s_Material {"Material"};
constexpr static auto s_Image {"image"};
constexpr static auto s_Mesh {"Mesh"};
constexpr static auto s_ComputedMaskVolume {"ComputedMaskVolume"};

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::object::csptr object,
    std::map<std::string, data::object::csptr>& children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto reconstruction = helper::safe_cast<data::reconstruction>(object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::reconstruction>(tree, 1);

    // Serialize attributes
    tree.put(s_IsVisible, reconstruction->getIsVisible());
    helper::write_string(tree, s_OrganName, reconstruction->getOrganName());
    helper::write_string(tree, s_StructureType, reconstruction->getStructureType());

    children[s_Material] = reconstruction->getMaterial();
    children[s_Image]    = reconstruction->getImage();
    children[s_Mesh]     = reconstruction->getMesh();

    tree.put(s_ComputedMaskVolume, reconstruction->getComputedMaskVolume());
}

//------------------------------------------------------------------------------

inline static data::reconstruction::sptr read(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>& children,
    data::object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto reconstruction = helper::cast_or_create<data::reconstruction>(object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::reconstruction>(tree, 0, 1);

    // Deserialize attributes
    reconstruction->setIsVisible(tree.get<bool>(s_IsVisible));
    reconstruction->setOrganName(helper::read_string(tree, s_OrganName));
    reconstruction->setStructureType(helper::read_string(tree, s_StructureType));

    reconstruction->setMaterial(std::dynamic_pointer_cast<data::material>(children.at(s_Material)));
    reconstruction->setImage(std::dynamic_pointer_cast<data::image>(children.at(s_Image)));
    reconstruction->setMesh(std::dynamic_pointer_cast<data::mesh>(children.at(s_Mesh)));

    reconstruction->setComputedMaskVolume(tree.get<double>(s_ComputedMaskVolume));

    return reconstruction;
}

SIGHT_REGISTER_SERIALIZER(data::reconstruction, write, read);

} // namespace sight::io::session::detail::reconstruction
