/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "viz/scene3d/vr/ray_entry_compositor.hpp"

#include "viz/scene3d/compositor/core.hpp"
#include "viz/scene3d/ogre.hpp"

#include <OgreCompositionPass.h>
#include <OgreCompositionTargetPass.h>
#include <OgreCompositionTechnique.h>
#include <OgreCompositorManager.h>

#include <mutex>
#include <utility>

namespace sight::viz::scene3d::vr
{

// Mutex to avoid concurrent compositor manager calls.
static std::mutex s_compositor_manager_lock;

//------------------------------------------------------------------------------

ray_entry_compositor::ray_entry_compositor(
    std::string _compositor_name,
    std::uint8_t _rq_group,
    compositor::core::stereo_mode_t _stereo_mode,
    bool _enable_mixed_rendering
) :
    m_compositor_name(std::move(_compositor_name))
{
    auto& cm = Ogre::CompositorManager::getSingleton();
    std::lock_guard<std::mutex> guard(s_compositor_manager_lock);

    m_compositor = cm.getByName(m_compositor_name, RESOURCE_GROUP);

    if(!m_compositor)
    {
        m_compositor = cm.create(m_compositor_name, viz::scene3d::RESOURCE_GROUP);

        auto* comp_tech = m_compositor->createTechnique();

        std::uint8_t nb_viewpoints = 1;
        float height_factor        = 1.F;
        float width_factor         = 1.F;

        switch(_stereo_mode)
        {
            case compositor::core::stereo_mode_t::none:
                break;

            case compositor::core::stereo_mode_t::stereo:
                nb_viewpoints = 2;
                height_factor = 0.5F;
                break;

            case compositor::core::stereo_mode_t::autostereo_5:
                nb_viewpoints = 5;
                height_factor = 0.5F;
                width_factor  = 0.6F;
                break;

            case compositor::core::stereo_mode_t::autostereo_8:
                nb_viewpoints = 8;
                height_factor = 0.5F;
                width_factor  = 0.375F;
                break;
        }

        const auto scheme_prefix = std::string("VolumeEntries") + (nb_viewpoints > 1 ? "auto_stereo" : "");

        for(std::uint8_t i = 0 ; i < nb_viewpoints ; ++i)
        {
            const auto scheme_suffix   = nb_viewpoints > 1 ? std::to_string(i) : "";
            const auto tex_target_name = m_compositor_name + "texture" + scheme_suffix;

            auto* tex_def = comp_tech->createTextureDefinition(tex_target_name);
            tex_def->scope = Ogre::CompositionTechnique::TextureScope::TS_CHAIN;
            tex_def->formatList.push_back(Ogre::PixelFormat::PF_FLOAT32_GR);
            tex_def->heightFactor = height_factor;
            tex_def->widthFactor  = width_factor;

            auto* back_faces_target_pass = comp_tech->createTargetPass();
            back_faces_target_pass->setInputMode(Ogre::CompositionTargetPass::InputMode::IM_NONE);
            back_faces_target_pass->setOutputName(tex_target_name);

            auto* clear_pass = back_faces_target_pass->createPass();
            clear_pass->setType(Ogre::CompositionPass::PT_CLEAR);
            clear_pass->setClearBuffers(Ogre::FBT_COLOUR | Ogre::FBT_DEPTH);
            clear_pass->setClearColour(Ogre::ColourValue(0.F, 1.F, 1.F, 1.F));

            auto* back_faces_pass = back_faces_target_pass->createPass();
            back_faces_pass->setMaterialScheme(std::string(scheme_prefix) + "_BackFaces" + scheme_suffix);
            back_faces_pass->setType(Ogre::CompositionPass::PT_RENDERSCENE);
            back_faces_pass->setFirstRenderQueue(_rq_group);
            back_faces_pass->setLastRenderQueue(_rq_group);

            auto* front_faces_target_pass = comp_tech->createTargetPass();
            front_faces_target_pass->setInputMode(Ogre::CompositionTargetPass::InputMode::IM_NONE);
            front_faces_target_pass->setOutputName(tex_target_name);

            auto* front_faces_pass = front_faces_target_pass->createPass();
            front_faces_pass->setMaterialScheme(std::string(scheme_prefix) + "_FrontFaces" + scheme_suffix);
            front_faces_pass->setType(Ogre::CompositionPass::PT_RENDERSCENE);
            front_faces_pass->setFirstRenderQueue(_rq_group);
            front_faces_pass->setLastRenderQueue(_rq_group);

            auto* back_faces_max_target_pass = comp_tech->createTargetPass();
            back_faces_max_target_pass->setInputMode(Ogre::CompositionTargetPass::InputMode::IM_NONE);
            back_faces_max_target_pass->setOutputName(tex_target_name);

            auto* back_faces_max_pass = back_faces_max_target_pass->createPass();
            back_faces_max_pass->setMaterialScheme(std::string(scheme_prefix) + "_BackFacesMax" + scheme_suffix);
            back_faces_max_pass->setType(Ogre::CompositionPass::PT_RENDERSCENE);
            back_faces_max_pass->setFirstRenderQueue(_rq_group);
            back_faces_max_pass->setLastRenderQueue(_rq_group);

            if(_enable_mixed_rendering)
            {
                auto* front_faces_min_target_pass = comp_tech->createTargetPass();
                front_faces_min_target_pass->setInputMode(Ogre::CompositionTargetPass::InputMode::IM_NONE);
                front_faces_min_target_pass->setOutputName(tex_target_name);

                auto* front_faces_min_pass = front_faces_min_target_pass->createPass();
                front_faces_min_pass->setMaterialScheme(
                    std::string(scheme_prefix) + "_FrontFacesMin" + scheme_suffix
                );
                front_faces_min_pass->setType(Ogre::CompositionPass::PT_RENDERSCENE);
                front_faces_min_pass->setLastRenderQueue(viz::scene3d::rq::SURFACE_ID);
            }
        }

        auto* output_target_pass = comp_tech->getOutputTargetPass();
        output_target_pass->setInputMode(Ogre::CompositionTargetPass::InputMode::IM_PREVIOUS);
    }
}

//------------------------------------------------------------------------------

ray_entry_compositor::~ray_entry_compositor()
{
    auto& cm = Ogre::CompositorManager::getSingleton();

    std::lock_guard<std::mutex> guard(s_compositor_manager_lock);
    // If this is the last reference. (Plus the one kept by the manager)
    if(m_compositor.use_count() == 2)
    {
        cm.remove(m_compositor);
    }
}

//------------------------------------------------------------------------------

const std::string& ray_entry_compositor::get_name() const
{
    return m_compositor_name;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr
