/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_TRANSFERFUNCTION_HPP__
#define __FWRENDEROGRE_TRANSFERFUNCTION_HPP__

#include <OGRE/OgreTexture.h>

#include <fwData/TransferFunction.hpp>

#include "fwRenderOgre/config.hpp"

namespace fwRenderOgre
{

/**
 * @class TransferFunction
 * @brief Represents and manages a tranfer function from a GPU point of view
 */
class FWRENDEROGRE_CLASS_API TransferFunction
{
public:

    FWRENDEROGRE_API TransferFunction();
    FWRENDEROGRE_API virtual ~TransferFunction();

    /// Initalizes the textures attributes
    FWRENDEROGRE_API void createTexture(const ::Ogre::String& _parentId);

    /// Updates the TF texture according to the transfer function datas
    FWRENDEROGRE_API void updateTexture(const ::fwData::TransferFunction::sptr& _tf);

    /// Returns the TF texture
    FWRENDEROGRE_API ::Ogre::TexturePtr getTexture() const;

private:

    /// Texture containing the interpolated nodes of the transfer function
    ::Ogre::TexturePtr m_texture;
};

//------------------------------------------------------------------------------
// Inline functions

inline ::Ogre::TexturePtr TransferFunction::getTexture() const
{
    return m_texture;
}

//------------------------------------------------------------------------------

} // Namespace fwRenderOgre

#endif // __FWRENDEROGRE_TRANSFERFUNCTION_HPP__
