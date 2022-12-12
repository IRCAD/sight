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
#include "io/session/macros.hpp"

#include <data/Image.hpp>
#include <data/Material.hpp>
#include <data/Mesh.hpp>
#include <data/Reconstruction.hpp>

namespace sight::io::session::detail::Reconstruction
{

constexpr static auto s_IsVisible {"IsVisible"};
constexpr static auto s_OrganName {"OrganName"};
constexpr static auto s_StructureType {"StructureType"};
constexpr static auto s_Material {"Material"};
constexpr static auto s_Image {"Image"};
constexpr static auto s_Mesh {"Mesh"};
constexpr static auto s_ComputedMaskVolume {"ComputedMaskVolume"};

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto reconstruction = Helper::safe_cast<data::Reconstruction>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Reconstruction>(tree, 1);

    // Serialize attributes
    tree.put(s_IsVisible, reconstruction->getIsVisible());
    Helper::writeString(tree, s_OrganName, reconstruction->getOrganName());
    Helper::writeString(tree, s_StructureType, reconstruction->getStructureType());

    children[s_Material] = reconstruction->getMaterial();
    children[s_Image]    = reconstruction->getImage();
    children[s_Mesh]     = reconstruction->getMesh();

    tree.put(s_ComputedMaskVolume, reconstruction->getComputedMaskVolume());
}

//------------------------------------------------------------------------------

inline static data::Reconstruction::sptr read(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto reconstruction = Helper::cast_or_create<data::Reconstruction>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Reconstruction>(tree, 0, 1);

    // Deserialize attributes
    reconstruction->setIsVisible(tree.get<bool>(s_IsVisible));
    reconstruction->setOrganName(Helper::readString(tree, s_OrganName));
    reconstruction->setStructureType(Helper::readString(tree, s_StructureType));

    reconstruction->setMaterial(std::dynamic_pointer_cast<data::Material>(children.at(s_Material)));
    reconstruction->setImage(std::dynamic_pointer_cast<data::Image>(children.at(s_Image)));
    reconstruction->setMesh(std::dynamic_pointer_cast<data::Mesh>(children.at(s_Mesh)));

    reconstruction->setComputedMaskVolume(tree.get<double>(s_ComputedMaskVolume));

    return reconstruction;
}

SIGHT_REGISTER_SERIALIZER(data::Reconstruction, write, read);

} // namespace sight::io::session::detail::Reconstruction
