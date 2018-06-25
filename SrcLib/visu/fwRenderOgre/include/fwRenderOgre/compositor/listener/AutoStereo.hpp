/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
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

class AutoStereoCompositorListener : public ::Ogre::MaterialManager::Listener
{
public:

    /// Constructor, needs to know how many views are needed.
    AutoStereoCompositorListener(std::uint8_t _viewpointNumber);

    /// Destructor, destroys created techniques.
    ~AutoStereoCompositorListener();

    /// Callback called each time a scheme is not found
    virtual ::Ogre::Technique* handleSchemeNotFound(unsigned short _schemeIndex, const ::Ogre::String& _schemeName,
                                                    ::Ogre::Material* _originalMaterial, unsigned short _lodIndex,
                                                    const ::Ogre::Renderable* _renderable);

private:

    /// list of techniques to be removed from the material when destroying the listener
    std::vector< ::Ogre::Technique*> m_createdTechniques;

    /// Number of viewpoints used for stereo rendering.
    const std::uint8_t m_viewpointNumber;
};

} // namespace listener

} // namespace compositor

} // namespace fwRenderOgre
