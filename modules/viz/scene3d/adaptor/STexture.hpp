/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include <core/com/Signals.hpp>
#include <core/com/Slot.hpp>

#include <data/Image.hpp>
#include <data/Material.hpp>

#include <viz/scene3d/IAdaptor.hpp>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor maps a texture on a mesh. This is done via module::viz::scene3d::adaptor::SMaterial.
 * In the configuration we don't specify the material adaptor since it is automatically created by the
 * module::viz::scene3d::adaptor::SMesh adaptor.
 * The mesh adaptor isn't specified either because the texture can be applied on several meshes.
 *
 * @section Signals Signals
 * - \b textureSwapped() : Emitted when the texture is modified.

 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::STexture">
        <in key="image" uid="..." />
        <config layer="..." textureName="texName" filtering="linear" wrapping="repeat" useAlpha="false" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b image [sight::data::Image]: the image to map as a texture.
 *
 * @subsection Configuration Configuration:
 *  - \b layer (mandatory, string): defines the texture's layer
 *  - \b textureName (optional, string, default="") : the name of the ogre texture managed by the adaptor
 *  - \b filtering (optional nearest/linear, default=linear) : filtering of the texture
 *  - \b wrapping (optional, clamp/repeat, default=repeat) : wrapping of the texture
 *  - \b useAlpha (optional, bool, default=true) : whether or not the alpha channel is used
 *  - \b dynamic (optional, bool, default=false) : whether or not the texture is updated frequently
 */
class MODULE_VIZ_SCENE3D_CLASS_API STexture final : public sight::viz::scene3d::IAdaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(STexture, ::sight::viz::scene3d::IAdaptor);

    /// Defines the keys of the slot @ref textureSwapped().
    MODULE_VIZ_SCENE3D_API static const core::com::Signals::SignalKeyType s_TEXTURE_SWAPPED_SIG;

    /// Creates the adaptor and the signal.
    MODULE_VIZ_SCENE3D_API STexture() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~STexture() noexcept override;

    /// Gets the Ogre texture.
    MODULE_VIZ_SCENE3D_API ::Ogre::TexturePtr getTexture() const;

    /// Gets the textures name.
    MODULE_VIZ_SCENE3D_API std::string getTextureName() const;

    /// Sets the texture name.
    MODULE_VIZ_SCENE3D_API void setTextureName(std::string _texName);

    /// Gets if 1.0 is used as the alpha value.
    MODULE_VIZ_SCENE3D_API bool getUseAlpha() const;

    /// Return true if a valid image is stored in this texture
    MODULE_VIZ_SCENE3D_API bool isValid() const;

protected:

    /// Configures the adaptor.
    MODULE_VIZ_SCENE3D_API void configuring() override;

    /// Creates the managed Ogre texture.
    MODULE_VIZ_SCENE3D_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Image::s_BUFFER_MODIFIED_SIG of s_TEXTURE_INOUT to s_UPDATE_SLOT
     * Connect data::Image::s_BUFFER_MODIFIED_SIG of s_TEXTURE_INOUT to s_UPDATE_SLOT
     */
    MODULE_VIZ_SCENE3D_API service::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Updates the attached texture.
    MODULE_VIZ_SCENE3D_API void updating() override;

    /// Destroys Ogre resources.
    MODULE_VIZ_SCENE3D_API void stopping() override;

private:

    /// Contains the Ogre texture.
    ::Ogre::TexturePtr m_texture;

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
    typedef core::com::Signal<void ()> TextureSwappedSignalType;
    TextureSwappedSignalType::sptr m_sigTextureSwapped;

    static const service::key_t s_TEXTURE_INOUT;
    data::ptr<data::Image, data::Access::in> m_image {this, s_TEXTURE_INOUT, true};
};

//------------------------------------------------------------------------------

inline ::Ogre::TexturePtr STexture::getTexture() const
{
    return m_texture;
}

//------------------------------------------------------------------------------

inline std::string STexture::getTextureName() const
{
    return m_textureName;
}

//------------------------------------------------------------------------------

inline void STexture::setTextureName(std::string _texName)
{
    m_textureName = _texName;
}

//------------------------------------------------------------------------------

inline bool STexture::getUseAlpha() const
{
    return m_useAlpha;
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
