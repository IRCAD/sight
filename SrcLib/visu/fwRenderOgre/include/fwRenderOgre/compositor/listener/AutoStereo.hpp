/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_COMPOSITOR_LISTENER_AUTOSTEREO_HPP__
#define __FWRENDEROGRE_COMPOSITOR_LISTENER_AUTOSTEREO_HPP__

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
    /// Constructor for surface rendering
    AutoStereoCompositorListener();

    /// Constructor for volume rendering
    AutoStereoCompositorListener(std::vector< ::Ogre::TexturePtr>* renderTargets);

    /// Destructor
    ~AutoStereoCompositorListener();

    /// Callback called each time a scheme is not found
    virtual ::Ogre::Technique* handleSchemeNotFound(unsigned short _schemeIndex, const ::Ogre::String& _schemeName,
                                                    ::Ogre::Material* _originalMaterial, unsigned short _lodIndex,
                                                    const ::Ogre::Renderable* _renderable);

private:
    Ogre::Technique* copyTechnique(::Ogre::Technique* _tech, const::Ogre::String& _schemeName,
                                   ::Ogre::Material* _originalMaterial);

    /// render targets used for computing the entry points in VR
    std::vector< ::Ogre::TexturePtr>* m_renderTargets;

    /// list of techniques to be removed from the material when destroying the listener
    std::vector< ::Ogre::Technique*> m_createdTechniques;
};

} // namespace listener

} // namespace compositor

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_COMPOSITOR_LISTENER_AUTOSTEREO_HPP__
