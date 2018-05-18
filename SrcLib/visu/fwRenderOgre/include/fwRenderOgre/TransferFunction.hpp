/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwRenderOgre/config.hpp"

#include <fwData/TransferFunction.hpp>

#include <OGRE/OgreTexture.h>

namespace fwRenderOgre
{

/**
 * @brief Represents and manages a tranfer function from a GPU point of view
 */
class FWRENDEROGRE_CLASS_API TransferFunction
{
public:

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

private:

    /// Texture containing the interpolated nodes of the transfer function.
    ::Ogre::TexturePtr m_texture;

    /// Current sample distance used in the VR renderer.
    float m_sampleDistance;

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

//-----------------------------------------------------------------------------

} // Namespace fwRenderOgre
