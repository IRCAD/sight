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
#include <data/image.hpp>
#include <data/material.hpp>

namespace sight::io::session::detail::material
{

constexpr static auto s_ShadingMode {"ShadingMode"};
constexpr static auto s_RepresentationMode {"RepresentationMode"};
constexpr static auto s_OptionsMode {"OptionsMode"};
constexpr static auto s_DiffuseTextureFiltering {"DiffuseTextureFiltering"};
constexpr static auto s_DiffuseTextureWrapping {"DiffuseTextureWrapping"};
constexpr static auto s_Ambient {"Ambient"};
constexpr static auto s_Diffuse {"Diffuse"};
constexpr static auto s_DiffuseTexture {"DiffuseTexture"};

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& _children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto material = helper::safe_cast<data::material>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::material>(_tree, 1);

    _tree.put(s_ShadingMode, material->getShadingMode());
    _tree.put(s_RepresentationMode, material->getRepresentationMode());
    _tree.put(s_OptionsMode, material->getOptionsMode());
    _tree.put(s_DiffuseTextureFiltering, material->getDiffuseTextureFiltering());
    _tree.put(s_DiffuseTextureWrapping, material->getDiffuseTextureWrapping());

    _children[s_Ambient]        = material->ambient();
    _children[s_Diffuse]        = material->diffuse();
    _children[s_DiffuseTexture] = material->getDiffuseTexture();
}

//------------------------------------------------------------------------------

inline static data::material::sptr read(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& _children,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto material = helper::cast_or_create<data::material>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::material>(_tree, 0, 1);

    material->setShadingMode(static_cast<data::material::shading_t>(_tree.get<int>(s_ShadingMode)));
    material->setRepresentationMode(
        static_cast<data::material::representation_t>(
            _tree.get<int>(s_RepresentationMode)
        )
    );
    material->setOptionsMode(static_cast<data::material::options_t>(_tree.get<int>(s_OptionsMode)));
    material->setDiffuseTextureFiltering(
        static_cast<data::material::filtering_t>(
            _tree.get<int>(s_DiffuseTextureFiltering)
        )
    );
    material->setDiffuseTextureWrapping(
        static_cast<data::material::wrapping_t>(
            _tree.get<int>(s_DiffuseTextureWrapping)
        )
    );

    material->setAmbient(std::dynamic_pointer_cast<data::color>(_children.at(s_Ambient)));
    material->setDiffuse(std::dynamic_pointer_cast<data::color>(_children.at(s_Diffuse)));
    material->setDiffuseTexture(std::dynamic_pointer_cast<data::image>(_children.at(s_DiffuseTexture)));

    return material;
}

SIGHT_REGISTER_SERIALIZER(data::material, write, read);

} // namespace sight::io::session::detail::material
