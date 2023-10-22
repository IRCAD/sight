/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/config.hpp"

#include <core/com/signals.hpp>
#include <core/com/slot.hpp>

#include <data/image.hpp>
#include <data/material.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/texture.hpp>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor maps a texture on a mesh. This is done via module::viz::scene3d::adaptor::material.
 * In the configuration we don't specify the material adaptor since it is automatically created by the
 * module::viz::scene3d::adaptor::mesh adaptor.
 * The mesh adaptor isn't specified either because the texture can be applied on several meshes.
 *
 * @section Signals Signals
 * - \b textureSwapped() : Emitted when the texture is modified.

 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::texture">
        <in key="image" uid="..." />
        <config textureName="texName" filtering="linear" wrapping="repeat" useAlpha="false" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b image [sight::data::image]: the image to map as a texture.
 *
 * @subsection Configuration Configuration:
 *  - \b textureName (optional, string, default="") : the name of the ogre texture managed by the adaptor
 *  - \b filtering (optional nearest/linear, default=linear) : filtering of the texture
 *  - \b wrapping (optional, clamp/repeat, default=repeat) : wrapping of the texture
 *  - \b useAlpha (optional, bool, default=true) : whether or not the alpha channel is used
 *  - \b dynamic (optional, bool, default=false) : whether or not the texture is updated frequently
 */
class MODULE_VIZ_SCENE3D_CLASS_API texture final : public sight::viz::scene3d::adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(texture, sight::viz::scene3d::adaptor);

    /// Defines the keys of the slot @ref textureSwapped().
    MODULE_VIZ_SCENE3D_API static const core::com::signals::key_t TEXTURE_SWAPPED_SIG;

    /// Creates the adaptor and the signal.
    MODULE_VIZ_SCENE3D_API texture() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~texture() noexcept final = default;

    /// Gets the Ogre texture.
    MODULE_VIZ_SCENE3D_API Ogre::TexturePtr getTexture() const;

    /// Gets the textures name.
    MODULE_VIZ_SCENE3D_API std::string getTextureName() const;

    /// Sets the texture name.
    MODULE_VIZ_SCENE3D_API void setTextureName(std::string _tex_name);

    /// Gets if 1.0 is used as the alpha value.
    MODULE_VIZ_SCENE3D_API bool getUseAlpha() const;

    /// Return true if a valid image is stored in this texture
    MODULE_VIZ_SCENE3D_API bool isValid() const;

protected:

    /// Configures the adaptor.
    MODULE_VIZ_SCENE3D_API void configuring() final;

    /// Creates the managed Ogre texture.
    MODULE_VIZ_SCENE3D_API void starting() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::image::BUFFER_MODIFIED_SIG of s_TEXTURE_INOUT to service::slots::UPDATE
     * Connect data::image::BUFFER_MODIFIED_SIG of s_TEXTURE_INOUT to service::slots::UPDATE
     */
    MODULE_VIZ_SCENE3D_API service::connections_t auto_connections() const final;

    /// Updates the attached texture.
    MODULE_VIZ_SCENE3D_API void updating() final;

    /// Destroys Ogre resources.
    MODULE_VIZ_SCENE3D_API void stopping() final;

private:

    /// Contains the Ogre texture.
    sight::viz::scene3d::texture::sptr m_texture;

    /// Defines the texture's name in the Ogre resource manager.
    std::string m_textureName;

    /// Defines the filtering method.
    std::string m_filtering {"linear"};

    /// Defines the wraping method.
    std::string m_wrapping {"repeat"};

    /// Defines if 1.0 is used as the alpha value (no transparency) or another value.
    bool m_useAlpha {true};

    /// Defines if the texture changes dynamically, defined in m_configuration.
    bool m_isDynamic {false};

    /// Defines the signal emitted when the texture has to be changed on the associated material.
    typedef core::com::signal<void ()> texture_swapped_signal_t;
    texture_swapped_signal_t::sptr m_sigTextureSwapped;

    static constexpr std::string_view s_TEXTURE_INOUT = "image";
    data::ptr<data::image, data::Access::in> m_image {this, s_TEXTURE_INOUT, true};
};

//------------------------------------------------------------------------------

inline Ogre::TexturePtr texture::getTexture() const
{
    return m_texture->get();
}

//------------------------------------------------------------------------------

inline std::string texture::getTextureName() const
{
    return m_textureName;
}

//------------------------------------------------------------------------------

inline void texture::setTextureName(std::string _tex_name)
{
    m_textureName = _tex_name;
}

//------------------------------------------------------------------------------

inline bool texture::getUseAlpha() const
{
    return m_useAlpha;
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
