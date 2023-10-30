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

constexpr static auto IS_VISIBLE {"IsVisible"};
constexpr static auto ORGAN_NAME {"OrganName"};
constexpr static auto STRUCTURE_TYPE {"structure_t"};
constexpr static auto MATERIAL {"Material"};
constexpr static auto IMAGE {"image"};
constexpr static auto MESH {"Mesh"};
constexpr static auto COMPUTED_MASK_VOLUME {"ComputedMaskVolume"};

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& _children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto reconstruction = helper::safe_cast<data::reconstruction>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::reconstruction>(_tree, 1);

    // Serialize attributes
    _tree.put(IS_VISIBLE, reconstruction->get_is_visible());
    helper::write_string(_tree, ORGAN_NAME, reconstruction->get_organ_name());
    helper::write_string(_tree, STRUCTURE_TYPE, reconstruction->get_structure_type());

    _children[MATERIAL] = reconstruction->get_material();
    _children[IMAGE]    = reconstruction->get_image();
    _children[MESH]     = reconstruction->get_mesh();

    _tree.put(COMPUTED_MASK_VOLUME, reconstruction->get_computed_mask_volume());
}

//------------------------------------------------------------------------------

inline static data::reconstruction::sptr read(
    zip::archive_reader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& _children,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto reconstruction = helper::cast_or_create<data::reconstruction>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::reconstruction>(_tree, 0, 1);

    // Deserialize attributes
    reconstruction->set_is_visible(_tree.get<bool>(IS_VISIBLE));
    reconstruction->set_organ_name(helper::read_string(_tree, ORGAN_NAME));
    reconstruction->set_structure_type(helper::read_string(_tree, STRUCTURE_TYPE));

    reconstruction->set_material(std::dynamic_pointer_cast<data::material>(_children.at(MATERIAL)));
    reconstruction->set_image(std::dynamic_pointer_cast<data::image>(_children.at(IMAGE)));
    reconstruction->set_mesh(std::dynamic_pointer_cast<data::mesh>(_children.at(MESH)));

    reconstruction->set_computed_mask_volume(_tree.get<double>(COMPUTED_MASK_VOLUME));

    return reconstruction;
}

SIGHT_REGISTER_SERIALIZER(data::reconstruction, write, read);

} // namespace sight::io::session::detail::reconstruction
