/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
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

#include "visuOgreAdaptor/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>

#include <fwData/Image.hpp>
#include <fwData/Material.hpp>

#include <fwRenderOgre/IAdaptor.hpp>

namespace visuOgreAdaptor
{
/**
 * @brief This adaptor maps a texture on a mesh. This is done via ::visuOgreAdaptor::SMaterial. In the configuration we
 *  don't specify the material adaptor since it is automatically created by the ::visuOgreAdaptor::SMesh adaptor.
 *  The mesh adaptor isn't specified too because the texture can be applied on several meshes.
 *
 * @section Signals Signals
 * - \b textureSwapped() : Emitted when the texture is modified.

 * @section XML XML Configuration
 * @code{.xml}
    <service type="::visuOgreAdaptor::STexture">
        <in key="image" uid="..." />
        <config layer="..." textureName="texName" filtering="linear" wrapping="repeat" useAlpha="false" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b image [::fwData::Image]: the image to map as a texture.
 *
 * @subsection Configuration Configuration:
 *  - \b layer (mandatory, string): defines the texture's layer
 *  - \b textureName (optional, string, default="") : the name of the ogre texture managed by the adaptor
 *  - \b filtering (optional nearest/linear, default=linear) : filtering of the texture
 *  - \b wrapping (optional, clamp/repeat, default=repeat) : wrapping of the texture
 *  - \b useAlpha (optional, bool, default=true) : whether or not the alpha channel is used
 *  - \b dynamic (optional, bool, default=false) : whether or not the texture is updated frequently
 */
class VISUOGREADAPTOR_CLASS_API STexture final : public ::fwRenderOgre::IAdaptor
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(STexture, ::fwRenderOgre::IAdaptor)

    /// Defines the keys of the slot @ref textureSwapped().
    VISUOGREADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_TEXTURE_SWAPPED_SIG;

    /// Creates the adaptor and the signal.
    VISUOGREADAPTOR_API STexture() noexcept;

    /// Destroys the adaptor.
    VISUOGREADAPTOR_API ~STexture() noexcept override;

    /// Gets the Ogre texture.
    VISUOGREADAPTOR_API ::Ogre::TexturePtr getTexture() const;

    /// Gets the textures name.
    VISUOGREADAPTOR_API std::string getTextureName() const;

    /// Sets the texture name.
    VISUOGREADAPTOR_API void setTextureName(std::string _texName);

    /// Gets if 1.0 is used as the alpha value.
    VISUOGREADAPTOR_API bool getUseAlpha() const;

    /// Return true if a valid image is stored in this texture
    VISUOGREADAPTOR_API bool isValid() const;

protected:

    /// Configures the adaptor.
    VISUOGREADAPTOR_API void configuring() override;

    /// Creates the managed Ogre texture.
    VISUOGREADAPTOR_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::Image::s_BUFFER_MODIFIED_SIG of s_TEXTURE_INOUT to s_UPDATE_SLOT
     * Connect ::fwData::Image::s_BUFFER_MODIFIED_SIG of s_TEXTURE_INOUT to s_UPDATE_SLOT
     */
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Updates the attached texture.
    VISUOGREADAPTOR_API void updating() override;

    /// Destroys Ogre resources.
    VISUOGREADAPTOR_API void stopping() override;

private:

    /// Contains the Ogre texture.
    ::Ogre::TexturePtr m_texture;

    /// Defines the texture's name in the Ogre resource manager.
    std::string m_textureName;

    /// Defines the filtering method.
    std::string m_filtering { "linear" };

    /// Defines the wraping method.
    std::string m_wrapping { "repeat" };

    /// Defines if 1.0 is used as the alpha value (no transparency) or another value.
    bool m_useAlpha { true };

    /// Defines if the texture changes dynamically, defined in m_configuration.
    bool m_isDynamic { false };

    /// Defines the signal emitted when the texture has to be changed on the associated material.
    typedef ::fwCom::Signal< void () > TextureSwappedSignalType;
    TextureSwappedSignalType::sptr m_sigTextureSwapped;

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

} // namespace visuOgreAdaptor.
