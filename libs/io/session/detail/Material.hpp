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

#include <data/Color.hpp>
#include <data/Image.hpp>
#include <data/Material.hpp>

namespace sight::io::session
{

namespace detail::Material
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

inline static void serialize(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& = ""
)
{
    const auto material = Helper::safeCast<data::Material>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Material>(tree, 1);

    tree.put(s_ShadingMode, material->getShadingMode());
    tree.put(s_RepresentationMode, material->getRepresentationMode());
    tree.put(s_OptionsMode, material->getOptionsMode());
    tree.put(s_DiffuseTextureFiltering, material->getDiffuseTextureFiltering());
    tree.put(s_DiffuseTextureWrapping, material->getDiffuseTextureWrapping());

    children[s_Ambient]        = material->ambient();
    children[s_Diffuse]        = material->diffuse();
    children[s_DiffuseTexture] = material->getDiffuseTexture();
}

//------------------------------------------------------------------------------

inline static data::Material::sptr deserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& = ""
)
{
    // Create or reuse the object
    auto material = Helper::safeCast<data::Material>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Material>(tree, 0, 1);

    material->setShadingMode(static_cast<data::Material::ShadingType>(tree.get<int>(s_ShadingMode)));
    material->setRepresentationMode(
        static_cast<data::Material::RepresentationType>(
            tree.get<int>(s_RepresentationMode)
        )
    );
    material->setOptionsMode(static_cast<data::Material::OptionsType>(tree.get<int>(s_OptionsMode)));
    material->setDiffuseTextureFiltering(
        static_cast<data::Material::FilteringType>(
            tree.get<int>(s_DiffuseTextureFiltering)
        )
    );
    material->setDiffuseTextureWrapping(
        static_cast<data::Material::WrappingType>(
            tree.get<int>(s_DiffuseTextureWrapping)
        )
    );

    material->setAmbient(std::dynamic_pointer_cast<data::Color>(children.at(s_Ambient)));
    material->setDiffuse(std::dynamic_pointer_cast<data::Color>(children.at(s_Diffuse)));
    material->setDiffuseTexture(std::dynamic_pointer_cast<data::Image>(children.at(s_DiffuseTexture)));

    return material;
}

} // namespace detail::Material

} // namespace sight::io
