/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

#include <sight/viz/scene3d/config.hpp>

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
class SIGHT_VIZ_SCENE3D_CLASS_API core //TODO : Manage occlusion query
//    The current commented lines in this class are standing for occlusion query
// purposes
//public Ogre::FrameListener,
//public Ogre::RenderTargetListener,
//public Ogre::RenderObjectListener
{
public:

    enum class stereo_mode_t : std::uint8_t
    {
        none,
        autostereo_5,
        autostereo_8,
        stereo
    };

    using sptr = std::shared_ptr<core>;

    /// Default Compositor, one per "default" layer
    SIGHT_VIZ_SCENE3D_API core(Ogre::Viewport* _viewport);

    /// Destructor of default compositor
    SIGHT_VIZ_SCENE3D_API ~core() = default;

    /// Return the OIT selected
    SIGHT_VIZ_SCENE3D_API transparency_technique get_transparency_technique();

    /// Return the number of peels computed by Depth Peeling or x2 Dual Depth Peeling
    [[nodiscard]] SIGHT_VIZ_SCENE3D_API int get_transparency_depth() const;

    /// Set the stereo mode. Keep in mind that OIT techniques disable stereo for now.
    SIGHT_VIZ_SCENE3D_API void set_stereo_mode(stereo_mode_t _stereo_mode);

    /// Return the enabled stereo mode.
    [[nodiscard]] SIGHT_VIZ_SCENE3D_API stereo_mode_t get_stereo_mode() const;

    /// Set the OIT desired and the number of peels computed by Depth Peeling or x2 Dual Depth Peeling
    SIGHT_VIZ_SCENE3D_API void set_transparency(transparency_technique _technique, int _depth = 0);

private:

    //virtual void notifyRenderSingleObject(Ogre::Renderable* rend, const Ogre::Pass* pass,
    //                                      const Ogre::AutoParamDataSource* source,
    //                                      const Ogre::LightList* pLightList, bool suppressRenderStateChanges);

    //SIGHT_VIZ_SCENE3D_API void setupQueries();

    //SIGHT_VIZ_SCENE3D_API virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

    //int m_transparencyTechniqueMaxDepth;

    /// OIT used
    transparency_technique m_transparency_technique {DEFAULT};

    /// OIT used - string name
    Ogre::String m_core_compositor_name;

    /// Cel shading activated
    Ogre::String m_cell_shading_name;

    stereo_mode_t m_stereo_mode {stereo_mode_t::none};

    //bool m_useOcclusionQuery;

    //bool m_doOcclusionQuery;

    //Ogre::HardwareOcclusionQuery* m_OITQuery;

    //Ogre::HardwareOcclusionQuery* m_activeQuery;

    /// Number of peels computed for Depth Peeling or 2x Dual Depth Peeling
    int m_num_pass {8};

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
