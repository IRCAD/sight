/************************************************************************
 *
 * Copyright (C) 2015-2022 IRCAD France
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

#include "viz/scene3d/config.hpp"

#include <data/Image.hpp>

#include <viz/scene3d/IResource.hpp>

#include <OGRE/OgrePass.h>
#include <OGRE/OgreTexture.h>

#include <optional>
namespace sight::viz::scene3d
{

/**
 * @brief This binds a sight::data::Image to a shared Ogre texture. It allows to share the GPU memory from multiple
 * callers that use the same image, and only update the GPU buffer when necessary.
 */
class VIZ_SCENE3D_CLASS_API Texture : public IResource<data::Image,
                                                       Ogre::Texture,
                                                       Texture>
{
public:

    /// Instantiates a texture. If a texture matching the image name does not exist, it creates a new one, otherwise
    /// it wraps the existing one.
    /// @param object input image
    /// @param suffixId optional suffix to the object identifier, this allows to create different copies of the object
    VIZ_SCENE3D_API Texture(const data::Image::csptr& _image, const std::string& suffixId = "");
    VIZ_SCENE3D_API ~Texture() override;

    /// Updates the TF texture according to the source image content, if any modification occurred since the last
    /// update.
    VIZ_SCENE3D_API void update() override;

    /// Binds the texture in the given texture unit state
    VIZ_SCENE3D_API void bind(
        Ogre::TextureUnitState* _texUnit,
        Ogre::TextureType _type,
        Ogre::TextureFilterOptions _filterType,
        Ogre::TextureAddressingMode _addressMode
    );

    /// Binds the texture in the given pass
    VIZ_SCENE3D_API void bind(
        Ogre::Pass* _pass,
        const std::string& _samplerName,
        std::optional<Ogre::TextureFilterOptions> _filterType   = std::nullopt,
        std::optional<Ogre::TextureAddressingMode> _addressMode = std::nullopt
    );

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
