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
    AutoStereoCompositorListener(unsigned int _numViewPoints, ::Ogre::Camera& _camera);

    /// Constructor for volume rendering
    AutoStereoCompositorListener(unsigned int _numViewPoints, ::Ogre::Camera& _camera,
                                 std::vector< ::Ogre::TexturePtr>* renderTargets);

    /// Callback called each time a scheme is not found
    virtual ::Ogre::Technique* handleSchemeNotFound(unsigned short _schemeIndex, const ::Ogre::String& _schemeName,
                                                    ::Ogre::Material* _originalMaterial, unsigned short _lodIndex,
                                                    const ::Ogre::Renderable* _renderable);

private:
    Ogre::Matrix4 frustumShearTransform(float angle) const;
    Ogre::Technique* copyTechnique(::Ogre::Technique* _tech, const::Ogre::String& _schemeName,
                                   ::Ogre::Material* _originalMaterial);

    /// Number of viewpoints
    unsigned int m_numViewPoints;

    /// Camera used for rendering.
    ::Ogre::Camera& m_camera;

    std::vector< ::Ogre::TexturePtr>* m_renderTargets;
};

} // namespace listener

} // namespace compositor

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_COMPOSITOR_LISTENER_AUTOSTEREO_HPP__
