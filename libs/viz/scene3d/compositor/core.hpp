/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

namespace sight::viz::scene3d::compositor
{

/**
 * Manages principal compositor for a layer's 3D scene
 */
class VIZ_SCENE3D_CLASS_API core //TODO : Manage occlusion query
    //    The current commented lines in this class are standing for occlusion query
    // purposes
    //public Ogre::FrameListener,
    //public Ogre::RenderTargetListener,
    //public Ogre::RenderObjectListener
{
public:

    enum class stereo_mode_t : std::uint8_t
    {
        NONE,
        AUTOSTEREO_5,
        AUTOSTEREO_8,
        STEREO
    };

    typedef std::shared_ptr<core> sptr;

    /// Default Compositor, one per "default" layer
    VIZ_SCENE3D_API core(Ogre::Viewport* _viewport);

    /// Destructor of default compositor
    VIZ_SCENE3D_API ~core();

    /// Return the OIT selected
    VIZ_SCENE3D_API transparencyTechnique getTransparencyTechnique();

    /// Return the number of peels computed by Depth Peeling or x2 Dual Depth Peeling
    [[nodiscard]] VIZ_SCENE3D_API int getTransparencyDepth() const;

    /// Set the OIT desired
    /// Deactivate OIT compositor
    VIZ_SCENE3D_API bool setTransparencyTechnique(transparencyTechnique _technique);

    /// Set the number of peels computed by Depth Peeling or x2 Dual Depth Peeling
    /// Deactivate OIT compositor
    VIZ_SCENE3D_API void setTransparencyDepth(int _depth);

    /// Set the stereo mode. Keep in mind that OIT techniques disable stereo for now.
    VIZ_SCENE3D_API void setStereoMode(stereo_mode_t _stereo_mode);

    /// Return the enabled stereo mode.
    [[nodiscard]] VIZ_SCENE3D_API stereo_mode_t getStereoMode() const;

    /// Re/check OIT compositor
    VIZ_SCENE3D_API void update();

private:

    //virtual void notifyRenderSingleObject(Ogre::Renderable* rend, const Ogre::Pass* pass,
    //                                      const Ogre::AutoParamDataSource* source,
    //                                      const Ogre::LightList* pLightList, bool suppressRenderStateChanges);

    /// Set number of ping pong peels for Depth Peeling compositor
    VIZ_SCENE3D_API void setTransparencyDepthOfDepthPeeling(int _depth);

    /// Set number of ping pong peels for Dual Depth Peeling compositor
    VIZ_SCENE3D_API void setTransparencyDepthOfDualDepthPeeling(int _depth);

    /// Set number of Depth Peeling ping pong peels for Hybrid Transparency compositor
    /// - other peels computed with Weighted Blended OIT
    VIZ_SCENE3D_API void setTransparencyDepthOfHybridTransparency(int _depth);

    /// Setup Default compositor (without OIT)
    VIZ_SCENE3D_API void setupDefaultTransparency();

    /// Setup OIT current compositor
    VIZ_SCENE3D_API void setupTransparency();

    //VIZ_SCENE3D_API void setupQueries();

    //VIZ_SCENE3D_API virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

    //int m_transparencyTechniqueMaxDepth;

    /// OIT used
    transparencyTechnique m_transparencyTechnique {DEFAULT};

    /// OIT used - string name
    Ogre::String m_coreCompositorName;

    /// OIT compositor instance used
    Ogre::CompositorInstance* m_compositorInstance {nullptr};

    /// Cel shading activated
    Ogre::String m_cellShadingName;

    stereo_mode_t m_stereoMode {stereo_mode_t::NONE};

    //bool m_useOcclusionQuery;

    //bool m_doOcclusionQuery;

    //Ogre::HardwareOcclusionQuery* m_OITQuery;

    //Ogre::HardwareOcclusionQuery* m_activeQuery;

    /// Number of peels computed for Depth Peeling or 2x Dual Depth Peeling
    int m_numPass {8};

    //int m_currNumPass;

    //bool m_isPing;

    //bool m_isPong;

    /// Viewport linked to compositor
    Ogre::Viewport* m_viewport;

    /// Name of the last compositor put in the compositor chain.
    /// This compositor is used to have a blend in order to get a correct final render
    static const std::string FINAL_CHAIN_COMPOSITOR;
};

} // namespace sight::viz::scene3d::compositor
