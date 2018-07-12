/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwRenderOgre/config.hpp"

#include <OgreMaterialManager.h>

namespace fwRenderOgre
{
namespace compositor
{
namespace listener
{
//-----------------------------------------------------------------------------

/**
 * @brief Listener used in mixed surface/volume rendering.
 * Creates volume entry point computation techniques for meshes.
 */
class RayExitDepthListener : public ::Ogre::MaterialManager::Listener
{
public:

    /// Constructor.
    RayExitDepthListener();

    /// Destructor.
    ~RayExitDepthListener();

    /// Called when an entry point scheme isn't found, creates the missing technique.
    virtual ::Ogre::Technique* handleSchemeNotFound(unsigned short _schemeIndex, const ::Ogre::String& _schemeName,
                                                    ::Ogre::Material* _originalMaterial, unsigned short _lodIndex,
                                                    const ::Ogre::Renderable* _renderable);

};

} // namespace listener

} // namespace compositor

} // namespace fwRenderOgre
