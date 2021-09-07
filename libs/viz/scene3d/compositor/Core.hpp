/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "viz/scene3d/config.hpp"

#include "viz/scene3d/compositor/types.hpp"

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreViewport.h>
//#include <OGRE/OgreRenderTargetListener.h>
//#include <OGRE/OgreRenderObjectListener.h>

#include <memory>

namespace sight::viz::scene3d
{

namespace compositor
{

/**
 * Manages principal compositor for a layer's 3D scene
 */
class VIZ_SCENE3D_CLASS_API Core //TODO : Manage occlusion query
    //    The current commented ligns in this class are standing for occlusion query
    // purposes
    //    An example of working occlusion query is used in the experimental branch
    // flavien_sg rev 76
    //public ::Ogre::FrameListener,
    //public ::Ogre::RenderTargetListener,
    //public ::Ogre::RenderObjectListener
{
public:

    enum class StereoModeType : std::uint8_t
    {
        NONE,
        AUTOSTEREO_5,
        AUTOSTEREO_8,
        STEREO
    };

    /// Render queue group for surface rendering.
    static constexpr std::uint8_t s_SURFACE_RQ_GROUP_ID = ::Ogre::RenderQueueGroupID::RENDER_QUEUE_MAIN;

    /// Render queue group for volume rendering. Leave an extra render queue between
    /// surfaces and volumes for some effects.
    static constexpr std::uint8_t s_VOLUME_RQ_GROUP_ID = s_SURFACE_RQ_GROUP_ID + 2;

    typedef std::shared_ptr<Core> sptr;

    /// Default Compositor, one per "default" layer
    VIZ_SCENE3D_API Core(::Ogre::Viewport* viewport);

    /// Destructor of default compositor
    VIZ_SCENE3D_API ~Core();

    /// Return the OIT selected
    VIZ_SCENE3D_API transparencyTechnique getTransparencyTechnique();

    /// Return the number of peels computed by Depth Peeling or x2 Dual Depth Peeling
    VIZ_SCENE3D_API int getTransparencyDepth();

    /// Set the OIT desired
    /// Deactivate OIT compositor
    VIZ_SCENE3D_API bool setTransparencyTechnique(transparencyTechnique technique);

    /// Set the number of peels computed by Depth Peeling or x2 Dual Depth Peeling
    /// Deactivate OIT compositor
    VIZ_SCENE3D_API void setTransparencyDepth(int depth);

    /// Set the stereo mode. Keep in mind that OIT techniques disable stereo for now.
    VIZ_SCENE3D_API void setStereoMode(StereoModeType stereoMode);

    /// Return the enabled stereo mode.
    VIZ_SCENE3D_API StereoModeType getStereoMode() const;

    /// Re/Activate OIT compositor
    VIZ_SCENE3D_API void update();

private:

    //virtual void notifyRenderSingleObject(::Ogre::Renderable* rend, const ::Ogre::Pass* pass,
    //                                      const ::Ogre::AutoParamDataSource* source,
    //                                      const ::Ogre::LightList* pLightList, bool suppressRenderStateChanges);

    /// Set number of ping pong peels for Depth Peeling compositor
    VIZ_SCENE3D_API void setTransparencyDepthOfDepthPeeling(int depth);

    /// Set number of ping pong peels for Dual Depth Peeling compositor
    VIZ_SCENE3D_API void setTransparencyDepthOfDualDepthPeeling(int depth);

    /// Set number of Depth Peeling ping pong peels for Hybrid Transparency compositor
    /// - other peels computed with Weighted Blended OIT
    VIZ_SCENE3D_API void setTransparencyDepthOfHybridTransparency(int depth);

    /// Setup Default compositor (without OIT)
    VIZ_SCENE3D_API void setupDefaultTransparency();

    /// Setup OIT current compositor
    VIZ_SCENE3D_API void setupTransparency();

    //VIZ_SCENE3D_API void setupQueries();

    //VIZ_SCENE3D_API virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

    //int m_transparencyTechniqueMaxDepth;

    /// OIT used
    transparencyTechnique m_transparencyTechnique;

    /// OIT used - string name
    ::Ogre::String m_coreCompositorName;

    /// OIT compositor instance used
    ::Ogre::CompositorInstance* m_compositorInstance;

    /// Cel shading activated
    ::Ogre::String m_celShadingName;

    StereoModeType m_stereoMode {StereoModeType::NONE};

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

} // namespace sight::viz::scene3d
