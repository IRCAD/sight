/************************************************************************
 *
 * Copyright (C) 2015-2025 IRCAD France
 * Copyright (C) 2015-2019 IHU Strasbourg
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

#include <sight/viz/scene3d/config.hpp>

#include <data/image.hpp>

#include <viz/scene3d/resource.hpp>

#include <OGRE/OgrePass.h>
#include <OGRE/OgreTexture.h>

#include <optional>
namespace sight::viz::scene3d
{

/**
 * @brief This binds a sight::data::image to a shared Ogre texture. It allows to share the GPU memory from multiple
 * callers that use the same image, and only update the GPU buffer when necessary.
 */
class SIGHT_VIZ_SCENE3D_CLASS_API texture : public resource<data::image,
                                                            Ogre::Texture,
                                                            texture>
{
public:

    /// Instantiates a texture. If a texture matching the image name does not exist, it creates a new one, otherwise
    /// it wraps the existing one.
    /// @param _image input image
    /// @param _suffix_id optional suffix to the object identifier, this allows to create different copies of the object
    SIGHT_VIZ_SCENE3D_API texture(const data::image::csptr& _image, const std::string& _suffix_id = "");
    SIGHT_VIZ_SCENE3D_API ~texture() override;

    /// Updates the TF texture according to the source image content, if any modification occurred since the last
    /// update.
    SIGHT_VIZ_SCENE3D_API void update() override;

    SIGHT_VIZ_SCENE3D_API void set_dirty();

    /// Binds the texture in the given texture unit state
    SIGHT_VIZ_SCENE3D_API void bind(
        Ogre::TextureUnitState* _tex_unit,
        Ogre::TextureType _type,
        Ogre::TextureFilterOptions _filter_type,
        Ogre::TextureAddressingMode _address_mode
    ) const;

    /// Binds the texture in the given pass
    SIGHT_VIZ_SCENE3D_API void bind(
        Ogre::Pass* _pass,
        const std::string& _sampler_name,
        std::optional<Ogre::TextureFilterOptions> _filter_type   = std::nullopt,
        std::optional<Ogre::TextureAddressingMode> _address_mode = std::nullopt,
        bool _upload_window_uniform                              = true
    ) const;

    /// Create and binds the texture in the given pass and the given uniform
    SIGHT_VIZ_SCENE3D_API void create_binding(
        Ogre::Pass* _pass,
        const std::string& _sampler_name,
        const std::string& _uniform_name,
        std::optional<Ogre::TextureFilterOptions> _filter_type   = std::nullopt,
        std::optional<Ogre::TextureAddressingMode> _address_mode = std::nullopt
    ) const;

    /// Gets the width of this texture
    [[nodiscard]] std::uint32_t width() const
    {
        return this->get()->getWidth();
    }

    /// Gets the height of this texture
    [[nodiscard]] std::uint32_t height() const
    {
        return this->get()->getHeight();
    }

    /// Gets the depth of this texture
    [[nodiscard]] std::uint32_t depth() const
    {
        return this->get()->getDepth();
    }

    /// Gets the name of this texture
    [[nodiscard]] std::string name() const
    {
        return this->get()->getName();
    }

    /// Gets the name of this texture
    [[nodiscard]] Ogre::Vector2 window() const
    {
        return m_window;
    }

private:

    /// Stores the texture window to upload it when necessary as a fragment shader uniform
    Ogre::Vector2 m_window;
};

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d
