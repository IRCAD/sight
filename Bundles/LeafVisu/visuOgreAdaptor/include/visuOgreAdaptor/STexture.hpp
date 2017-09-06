/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_STEXTURE_HPP__
#define __VISUOGREADAPTOR_STEXTURE_HPP__

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
 * @brief   Adaptor to map a texture on a mesh. This is done via ::visuOgreAdaptor::SMaterial. In the configuration we
 *  don't specify the material adaptor since it is automatically created by the ::visuOgreAdaptor::SMesh adaptor.
 *  The mesh adaptor isn't specified too because the texture can be applied on several meshes.
 *
 * @section Signals Signals
 * - \b textureSwapped() : Emitted when the texture is modified.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::visuOgreAdaptor::STexture">
            <in key="image" uid="..." />
            <config textureName="texName" filtering="linear" wrapping="repeat" useAlpha="false" />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b key1 [::fwData::Image]: .
 * @subsection Configuration Configuration:
 *  - \b textureName (optional) : the name of the ogre texture managed by the adaptor
 *  - \b filtering (optional nearest/linear, default=linear) : filtering of the texture
 *  - \b wrapping (optional, clamp/repeat, default=repeat) : wrapping of the texture
 *  - \b useAlpha (optional, true/false, default=true) : whether or not the alpha channel is used
 *  - \b dynamic (optional, true/false, default=false) : whether or not the texture is updated frequently
 */
class VISUOGREADAPTOR_CLASS_API STexture : public ::fwRenderOgre::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (STexture)(::fwRenderOgre::IAdaptor) );

    VISUOGREADAPTOR_API STexture() noexcept;
    VISUOGREADAPTOR_API virtual ~STexture() noexcept;

    /**
     * @name Signals API
     * @{
     */
    VISUOGREADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_TEXTURE_SWAPPED_SIG;
    typedef ::fwCom::Signal< void () > TextureSwappedSignalType;
    /** @} */

    /// Name of the default texture
    VISUOGREADAPTOR_API static const std::string DEFAULT_TEXTURE_FILENAME;

    VISUOGREADAPTOR_API ::Ogre::TexturePtr getTexture() const;
    VISUOGREADAPTOR_API std::string getTextureName() const;
    VISUOGREADAPTOR_API void setTextureName(std::string texName);

    VISUOGREADAPTOR_API bool getUseAlpha() const;
    VISUOGREADAPTOR_API void setUseAlpha(bool _useAlpha);

    /// Return true if a valid image is stored in this texture
    VISUOGREADAPTOR_API bool isValid() const;

protected:

    /// Configure the adaptor.
    VISUOGREADAPTOR_API void configuring() override;

    /// Creates the managed Ogre texture
    VISUOGREADAPTOR_API void starting() override;

    /// Updates the attached
    VISUOGREADAPTOR_API void updating() override;

    /// Does nothing
    VISUOGREADAPTOR_API void stopping() override;

    /// Returns proposals to connect service slots to associated object signals
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

private:

    /// Pointer to the Ogre texture
    ::Ogre::TexturePtr m_texture;
    /// Texture's name in the Ogre Ressource manager
    std::string m_textureName;

    /// How to filter this texture
    std::string m_filtering;

    /// How to wrap the texture
    std::string m_wrapping;

    /// Used as a flag to know if we have to use 1.0 as the alpha value (no transparency) or another value
    bool m_useAlpha;

    /// Signal emitted when the texture has to be changed on the associated material
    TextureSwappedSignalType::sptr m_sigTextureSwapped;

    /// defines if the texture changes dynamically, defined in m_configuration
    bool m_isDynamic;
};

//------------------------------------------------------------------------------
// Inline method(s)

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

inline void STexture::setTextureName(std::string texName)
{
    m_textureName = texName;
}

//------------------------------------------------------------------------------

inline bool STexture::getUseAlpha() const
{
    return m_useAlpha;
}

//------------------------------------------------------------------------------

inline void STexture::setUseAlpha(bool _useAlpha)
{
    m_useAlpha = _useAlpha;
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor

#endif // __VISUOGREADAPTOR_STEXTURE_HPP__
