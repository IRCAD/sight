/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_COMPOSITOR_SAOCHAINMANAGER_HPP__
#define __FWRENDEROGRE_COMPOSITOR_SAOCHAINMANAGER_HPP__

// used to view the content of the Ogre Compositor Chain
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgreCompositor.h>
#include <OGRE/OgrePrerequisites.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreTexture.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgrePass.h>

#include <fwRenderOgre/config.hpp>
#include <vector>

namespace Ogre
{
class CompositorManager;
class Viewport;
}

namespace fwRenderOgre
{
namespace compositor
{

class FWRENDEROGRE_CLASS_API SaoChainManager : ::boost::noncopyable
{
public:
    typedef std::unique_ptr < SaoChainManager > uptr;

    typedef std::string SaoCompositorIdType;
    typedef std::vector<SaoCompositorIdType> SaoCompositorChainType;

    FWRENDEROGRE_API SaoChainManager(::Ogre::Viewport* ogreViewport, ::Ogre::Camera* camera);

    FWRENDEROGRE_API void setOgreViewport(::Ogre::Viewport* viewport);

    /// enable or disable the effect
    FWRENDEROGRE_API void setSaoState(bool state);

    /// change the value of the sampling radius
    FWRENDEROGRE_API void setSaoRadius(double newRadius);

    /// get the radius
    FWRENDEROGRE_API double getSaoRadius();

    /// change the number of samples
    FWRENDEROGRE_API void setSaoSamples(int newSamplesNumber);

    /// get the Samples number
    FWRENDEROGRE_API int getSaoSamples();

    /// Enable/disable the blend
    FWRENDEROGRE_API void enableBlend(bool state);

    /// get the blend state
    FWRENDEROGRE_API bool getBlend();

    /// change the aoIntensity
    FWRENDEROGRE_API void setAoIntensity(double new_intensity);

    /// get the ao Intensity
    FWRENDEROGRE_API double getAoIntensity();

    /// get/set the camera used to render the scene
    FWRENDEROGRE_API const ::Ogre::Camera* getSceneCamera() const;
    FWRENDEROGRE_API void setSceneCamera(const ::Ogre::Camera* camera);

    /// inline function of the end
    FWRENDEROGRE_API SaoCompositorChainType getSaoCompositorChain();

private:
    /// Getter for the Ogre CompositorManager
    ::Ogre::CompositorManager& getCompositorManager();

    /// The parent layer's viewport.
    /// The ogre's compositor manager needs it in order to access the right compositor chain.
    ::Ogre::Viewport* m_ogreViewport;

    SaoCompositorChainType m_saoChain;

    /// radius
    double m_saoRadius;
    /// samples number
    int m_saoSamples;

    /// blend or not the scene
    bool m_blend;

    /// aoIntensity value
    double m_aoIntensity;

    /// Camera used to render the 3D scene
    const ::Ogre::Camera* m_camera;

};

//-----------------------------------------------------------------------------
// Inline method(s)

inline SaoChainManager::SaoCompositorChainType SaoChainManager::getSaoCompositorChain()
{
    return m_saoChain;
}

//-----------------------------------------------------------------------------

inline void SaoChainManager::setOgreViewport(::Ogre::Viewport* viewport)
{
    m_ogreViewport = viewport;
}

//-----------------------------------------------------------------------------

inline const ::Ogre::Camera* SaoChainManager::getSceneCamera() const
{
    return m_camera;
}

//-----------------------------------------------------------------------------

inline void SaoChainManager::setSceneCamera(const ::Ogre::Camera* camera)
{
    m_camera = camera;
}

//-----------------------------------------------------------------------------

} // namespace compositor

}// namespace fwRenderOgre


#endif // __FWRENDEROGRE_COMPOSITOR_SAOCHAINMANAGER_HPP__
