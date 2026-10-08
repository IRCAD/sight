/************************************************************************
 *
 * Copyright (C) 2021-2026 IRCAD France
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

#include <data/color.hpp>
#include <data/image.hpp>
#include <data/material.hpp>

#include <type_traits>

namespace sight::io::session::detail::material
{

constexpr static auto SHADING_MODE {"ShadingMode"};
constexpr static auto REPRESENTATION_MODE {"RepresentationMode"};
constexpr static auto OPTIONS_MODE {"OptionsMode"};
constexpr static auto DIFFUSE_TEXTURE_FILTERING {"DiffuseTextureFiltering"};
constexpr static auto DIFFUSE_TEXTURE_WRAPPING {"DiffuseTextureWrapping"};
constexpr static auto AMBIENT {"Ambient"};
constexpr static auto DIFFUSE {"Diffuse"};
constexpr static auto DIFFUSE_TEXTURE {"DiffuseTexture"};

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& _children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto material = helper::safe_cast<data::material>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::material>(_tree, 2);

    _tree.put<std::underlying_type_t<data::material::shading_t> >(
        SHADING_MODE,
        static_cast<std::underlying_type_t<data::material::shading_t> >(material->get_shading_mode())
    );
    _tree.put<std::underlying_type_t<data::material::representation_t> >(
        REPRESENTATION_MODE,
        static_cast<std::underlying_type_t<data::material::representation_t> >(material->get_representation_mode())
    );
    _tree.put<std::underlying_type_t<data::material::options_t> >(
        OPTIONS_MODE,
        static_cast<std::underlying_type_t<data::material::options_t> >(material->get_options_mode())
    );
    _tree.put<std::underlying_type_t<data::material::filtering_t> >(
        DIFFUSE_TEXTURE_FILTERING,
        static_cast<std::underlying_type_t<data::material::filtering_t> >(material->get_diffuse_texture_filtering())
    );
    _tree.put<std::underlying_type_t<data::material::wrapping_t> >(
        DIFFUSE_TEXTURE_WRAPPING,
        static_cast<std::underlying_type_t<data::material::wrapping_t> >(material->get_diffuse_texture_wrapping())
    );

    _children[AMBIENT]         = material->ambient();
    _children[DIFFUSE]         = material->diffuse();
    _children[DIFFUSE_TEXTURE] = material->get_diffuse_texture();
}

//------------------------------------------------------------------------------

inline static data::material::sptr read(
    zip::archive_reader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& _children,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto material = helper::cast_or_create<data::material>(_object);

    // Check version number. Not mandatory, but could help for future release
    const auto version = helper::read_version<data::material>(_tree, 0, 2);

    if(version < 2)
    {
        material->set_shading_mode(static_cast<data::material::shading_t>(_tree.get<int>(SHADING_MODE)));
    }
    else
    {
        material->set_shading_mode(static_cast<data::material::shading_t>(_tree.get<std::uint8_t>(SHADING_MODE)));
    }

    if(version < 2)
    {
        material->set_representation_mode(
            static_cast<data::material::representation_t>(
                _tree.get<int>(REPRESENTATION_MODE)
            )
        );
    }
    else
    {
        material->set_representation_mode(
            static_cast<data::material::representation_t>(
                _tree.get<std::uint8_t>(REPRESENTATION_MODE)
            )
        );
    }

    if(version < 2)
    {
        material->set_options_mode(static_cast<data::material::options_t>(_tree.get<int>(OPTIONS_MODE)));
    }
    else
    {
        material->set_options_mode(static_cast<data::material::options_t>(_tree.get<std::uint8_t>(OPTIONS_MODE)));
    }

    if(version < 2)
    {
        material->set_diffuse_texture_filtering(
            static_cast<data::material::filtering_t>(
                _tree.get<int>(DIFFUSE_TEXTURE_FILTERING)
            )
        );
    }
    else
    {
        material->set_diffuse_texture_filtering(
            static_cast<data::material::filtering_t>(
                _tree.get<std::uint8_t>(DIFFUSE_TEXTURE_FILTERING)
            )
        );
    }

    if(version < 2)
    {
        material->set_diffuse_texture_wrapping(
            static_cast<data::material::wrapping_t>(
                _tree.get<int>(DIFFUSE_TEXTURE_WRAPPING)
            )
        );
    }
    else
    {
        material->set_diffuse_texture_wrapping(
            static_cast<data::material::wrapping_t>(
                _tree.get<std::uint8_t>(DIFFUSE_TEXTURE_WRAPPING)
            )
        );
    }

    material->set_ambient(std::dynamic_pointer_cast<data::color>(_children.at(AMBIENT)));
    material->set_diffuse(std::dynamic_pointer_cast<data::color>(_children.at(DIFFUSE)));
    material->set_diffuse_texture(std::dynamic_pointer_cast<data::image>(_children.at(DIFFUSE_TEXTURE)));

    return material;
}

SIGHT_REGISTER_SERIALIZER(data::material, write, read);

} // namespace sight::io::session::detail::material
