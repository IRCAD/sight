/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwRenderOgre/config.hpp"

#include "fwRenderOgre/compositor/types.hpp"

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreViewport.h>
//#include <OGRE/OgreRenderTargetListener.h>
//#include <OGRE/OgreRenderObjectListener.h>

#include <memory>

namespace fwRenderOgre
{

namespace compositor
{

/**
 * Manages principal compositor for a layer's 3D scene
 */
class FWRENDEROGRE_CLASS_API Core   //TODO : Manage occlusion query
                                    //    The current commented ligns in this class are standing for occlusion query
                                    // purposes
                                    //    An example of working occlusion query is used in the experimental branch
                                    // flavien_sg rev 76
                                    //public ::Ogre::FrameListener,
                                    //public ::Ogre::RenderTargetListener,
                                    //public ::Ogre::RenderObjectListener

{
public:

    // Render queue group for surface rendering.
    static const std::uint8_t s_SURFACE_RQ_GROUP_ID = ::Ogre::RenderQueueGroupID::RENDER_QUEUE_MAIN;

    // Render queue group for volume rendering. Leave an extra render queue between
    // surfaces and volumes for some effects.
    static const std::uint8_t s_VOLUME_RQ_GROUP_ID = s_SURFACE_RQ_GROUP_ID + 2;

    typedef std::shared_ptr < Core > sptr;

    /// Default Compositor, one per "default" layer
    FWRENDEROGRE_API Core(::Ogre::Viewport* viewport);

    /// Destructor of default compositor
    FWRENDEROGRE_API ~Core();

    /// Return the OIT selected
    FWRENDEROGRE_API transparencyTechnique getTransparencyTechnique();

    /// Return the number of peels computed by Depth Peeling or x2 Dual Depth Peeling
    FWRENDEROGRE_API int getTransparencyDepth();

    /// Set the OIT desired
    /// Deactivate OIT compositor
    FWRENDEROGRE_API bool setTransparencyTechnique(transparencyTechnique technique);

    /// Set the number of peels computed by Depth Peeling or x2 Dual Depth Peeling
    /// Deactivate OIT compositor
    FWRENDEROGRE_API void setTransparencyDepth(int depth);

    /// Re/Activate OIT compositor
    FWRENDEROGRE_API void update();

private:

    //virtual void notifyRenderSingleObject(::Ogre::Renderable* rend, const ::Ogre::Pass* pass,
    //                                      const ::Ogre::AutoParamDataSource* source,
    //                                      const ::Ogre::LightList* pLightList, bool suppressRenderStateChanges);

    /// Set number of ping pong peels for Depth Peeling compositor
    FWRENDEROGRE_API void setTransparencyDepthOfDepthPeeling(int depth);

    /// Set number of ping pong peels for Dual Depth Peeling compositor
    FWRENDEROGRE_API void setTransparencyDepthOfDualDepthPeeling(int depth);

    /// Set number of Depth Peeling ping pong peels for Hybrid Transparency compositor
    /// - other peels computed with Weighted Blended OIT
    FWRENDEROGRE_API void setTransparencyDepthOfHybridTransparency(int depth);

    /// Setup Default compositor (without OIT)
    FWRENDEROGRE_API void setupDefaultTransparency();

    /// Setup OIT current compositor
    FWRENDEROGRE_API void setupTransparency();

    //FWRENDEROGRE_API void setupQueries();

    //FWRENDEROGRE_API virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

    //int m_transparencyTechniqueMaxDepth;

    /// OIT used
    transparencyTechnique m_transparencyTechnique;

    /// OIT used - string name
    ::Ogre::String m_transparencyTechniqueName;

    /// OIT compositor instance used
    ::Ogre::CompositorInstance* m_compositorInstance;

    /// Cel shading activated
    ::Ogre::String m_celShadingName;

    //bool m_useOcclusionQuery;

    //bool m_doOcclusionQuery;

    //::Ogre::HardwareOcclusionQuery* m_OITQuery;

    //::Ogre::HardwareOcclusionQuery* m_activeQuery;

    /// Number of peels computed for Depth Peeling or 2x Dual Depth Peeling
    int m_numPass;

    //int m_currNumPass;

    //bool m_isPing;

    //bool m_isPong;

    /// Viewport linked to compositor
    ::Ogre::Viewport* m_viewport;

    /// Name of the last compositor put in the compositor chain.
    /// This compositor is used to have a blend in order to get a correct final render
    static const std::string FINAL_CHAIN_COMPOSITOR;
};

} // namespace compositor

} // namespace fwRenderOgre
