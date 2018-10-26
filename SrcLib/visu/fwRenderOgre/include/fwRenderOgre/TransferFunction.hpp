/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwRenderOgre/config.hpp"

#include <fwData/TransferFunction.hpp>

#include <boost/noncopyable.hpp>

#include <OGRE/OgrePass.h>
#include <OGRE/OgreTexture.h>
#include <OGRE/OgreVector2.h>

namespace fwRenderOgre
{

/**
 * @brief Represents and manages a tranfer function from a GPU point of view
 */
class FWRENDEROGRE_CLASS_API TransferFunction : public ::boost::noncopyable
{
public:
    typedef std::shared_ptr<TransferFunction> sptr;
    typedef std::weak_ptr<TransferFunction> wptr;

    FWRENDEROGRE_API TransferFunction();
    FWRENDEROGRE_API virtual ~TransferFunction();

    /// Initialize the textures attributes.
    FWRENDEROGRE_API void createTexture(const ::Ogre::String& _parentId);

    /// Destroy the texture.
    FWRENDEROGRE_API void removeTexture();

    /// Update the TF texture according to the transfer function data.
    FWRENDEROGRE_API void updateTexture(const ::fwData::TransferFunction::csptr& _tf);

    /// Return the TF texture.
    FWRENDEROGRE_API ::Ogre::TexturePtr getTexture() const;

    /// Set the sample distance.
    FWRENDEROGRE_API void setSampleDistance(const float& _sampleDistance);

    /// Bind the texture and the uniforms in a given pass
    template <class GPU_PARAMETERS>
    void bind(const ::Ogre::Pass* const _ogrePass, const std::string& _texUnitName,
              ::Ogre::SharedPtr<GPU_PARAMETERS> _params, const std::string& _uniform = "u_tfWindow") const;
private:

    /// Texture containing the interpolated nodes of the transfer function.
    ::Ogre::TexturePtr m_texture;

    /// Current sample distance used in the VR renderer.
    float m_sampleDistance { 1.f };

    /**
     *  @brief Defines interpolation mode on extremities. Copied from ::fwData::TransferFunction.
     *
     *  if m_isClamped == true then after extremity point, the returned TF color is TFColor(0,0,0,0).
     *  if m_isClamped == false then after extremity point, the returned TF color is one of the extremity color value.
     **/
    bool m_isClamped { false };

    /// Stores the tf window to upload it when necessary as a fragment shader uniform
    ::Ogre::Vector2 m_tfWindow;

    static std::uint32_t TEXTURE_SIZE;
    static std::uint32_t TEXTURE_PIXEL_COUNT;
};

//-----------------------------------------------------------------------------
// Inline functions

inline ::Ogre::TexturePtr TransferFunction::getTexture() const
{
    return m_texture;
}

//-----------------------------------------------------------------------------

inline void TransferFunction::setSampleDistance(const float& _sampleDistance)
{
    m_sampleDistance = _sampleDistance;
}

//------------------------------------------------------------------------------

template <class GPU_PARAMETERS>
inline void TransferFunction::bind(const ::Ogre::Pass* const _pass, const std::string& _texUnitName,
                                   ::Ogre::SharedPtr<GPU_PARAMETERS> _params, const std::string& _uniform) const
{
    SLM_ASSERT("Pass is null", _pass);
    SLM_ASSERT("Parameters pointer is null", _params);

    auto texUnitState = _pass->getTextureUnitState(_texUnitName);
    SLM_ASSERT("'" + _texUnitName + "' texture unit is not found", texUnitState);

    if(texUnitState->getTextureName() != m_texture->getName())
    {
        texUnitState->setTexture(m_texture);
    }
    texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);

    // Beware, "clamped" here means the border color is black (see ::fwData::TransferFunction), otherwise use the
    // last value, which corresponds to the "clamp" texture address mode
    if(m_isClamped)
    {
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_BORDER);
        texUnitState->setTextureBorderColour(::Ogre::ColourValue::Black);
    }
    else
    {
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
    }

    _params->setNamedConstant(_uniform, m_tfWindow);
}

//-----------------------------------------------------------------------------

} // Namespace fwRenderOgre
