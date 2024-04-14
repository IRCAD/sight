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

#include <viz/scene3d/compositor/core.hpp>
#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/utils.hpp>

#include <OGRE/OgreCompositionPass.h>
#include <OGRE/OgreCompositionTargetPass.h>
#include <OGRE/OgreCompositor.h>
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreHardwareOcclusionQuery.h>

namespace sight::viz::scene3d::compositor
{

//----------------------------------------------------------------------------

static const std::map<core::stereo_mode_t, std::string> STEREO_COMPOSITOR_MAP = {
    {core::stereo_mode_t::autostereo_5, "AutoStereo5"},
    {core::stereo_mode_t::autostereo_8, "AutoStereo8"},
    {core::stereo_mode_t::stereo, "Stereo"},
    {core::stereo_mode_t::none, "Default"}
};

// ----------------------------------------------------------------------------

const std::string core::FINAL_CHAIN_COMPOSITOR = "FinalChainCompositor";

// ----------------------------------------------------------------------------

core::core(Ogre::Viewport* _viewport) :

    m_core_compositor_name("Default"),
    m_viewport(_viewport)
{
}

//-----------------------------------------------------------------------------

core::~core()
= default;

//-----------------------------------------------------------------------------

transparency_technique core::get_transparency_technique()
{
    return m_transparency_technique;
}

//-----------------------------------------------------------------------------

int core::get_transparency_depth() const
{
    return m_num_pass;
}

//-----------------------------------------------------------------------------

bool core::set_transparency_technique(transparency_technique _technique)
{
    try
    {
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(m_viewport, m_core_compositor_name, false);
    }
    catch(Ogre::InvalidParametersException&)
    {
        SIGHT_DEBUG("Can not find compositor: " + m_core_compositor_name);
    }
    m_transparency_technique = _technique;

    return true;
}

//-----------------------------------------------------------------------------

void core::update()
{
    m_cell_shading_name = "";

    SIGHT_ERROR_IF(
        "OIT isn't supported when stereo is enabled, falling back to mono rendering.",
        m_transparency_technique != DEFAULT && m_stereo_mode != stereo_mode_t::none
    );

    switch(m_transparency_technique)
    {
        case DEFAULT:
            m_core_compositor_name = STEREO_COMPOSITOR_MAP.at(m_stereo_mode);
            this->setup_transparency();
            this->setup_default_transparency();
            break;

        case cellshading_depthpeeling:
            m_cell_shading_name = "CellShading";
            BOOST_FALLTHROUGH;

        case depthpeeling:
            m_core_compositor_name = m_cell_shading_name + "DepthPeeling";
            this->setup_transparency();
            this->set_transparency_depth_of_depth_peeling(m_num_pass);
            break;

        case dualdepthpeeling:
            m_core_compositor_name = "DualDepthPeeling";
            this->setup_transparency();
            this->set_transparency_depth_of_dual_depth_peeling(m_num_pass);
            break;

        case weightedblendedoit:
            m_core_compositor_name = "WeightedBlended";
            this->setup_transparency();
            Ogre::CompositorManager::getSingleton().setCompositorEnabled(m_viewport, "WeightedBlended", true);
            break;

        case hybridtransparency:
            m_core_compositor_name = "HybridTransparency";
            this->setup_transparency();
            this->set_transparency_depth_of_hybrid_transparency(m_num_pass);
            break;
    }
}

//-----------------------------------------------------------------------------

void core::set_transparency_depth(int _depth)
{
    try
    {
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(m_viewport, m_core_compositor_name, false);
    }
    catch(Ogre::InvalidParametersException&)
    {
        SIGHT_DEBUG("Can not find compositor: " + m_core_compositor_name);
    }
    m_num_pass = _depth;
}

//-----------------------------------------------------------------------------

void core::set_stereo_mode(core::stereo_mode_t _stereo_mode)
{
    try
    {
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(m_viewport, m_core_compositor_name, false);
    }
    catch(Ogre::InvalidParametersException&)
    {
        SIGHT_DEBUG("Can not find compositor: " + m_core_compositor_name);
    }
    m_stereo_mode = _stereo_mode;
}

//-----------------------------------------------------------------------------

core::stereo_mode_t core::get_stereo_mode() const
{
    return m_stereo_mode;
}

//-----------------------------------------------------------------------------

void core::setup_default_transparency()
{
    try
    {
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(m_viewport, m_core_compositor_name, true);
    }
    catch(Ogre::InvalidParametersException&)
    {
        SIGHT_DEBUG("Can not find compositor: " + m_core_compositor_name);
    }
}

//-----------------------------------------------------------------------------

void core::setup_transparency()
{
    // Check if compositor is already existing
    Ogre::CompositorChain* comp_chain = Ogre::CompositorManager::getSingleton().getCompositorChain(m_viewport);

    const auto& comp_instances = comp_chain->getCompositorInstances();

    m_compositor_instance = nullptr;

    for(auto* target_comp : comp_instances)
    {
        if(target_comp->getCompositor()->getName() == m_core_compositor_name)
        {
            m_compositor_instance = target_comp;
            break;
        }
    }

    // If we didn't retrieve the good compositor
    if(m_compositor_instance == nullptr)
    {
        Ogre::CompositorManager& compositor_manager = Ogre::CompositorManager::getSingleton();
        bool need_final_compositor_swap(false);

        // If the compositor chain already contains the final compositor, we have to remove it
        Ogre::CompositorChain* chain = compositor_manager.getCompositorChain(m_viewport);
        if(const size_t pos = chain->getCompositorPosition(FINAL_CHAIN_COMPOSITOR); pos != Ogre::CompositorChain::NPOS)
        {
            chain->setCompositorEnabled(pos, false);
            chain->removeCompositor(pos);
            need_final_compositor_swap = true;
        }

        // Now, we can add the new compositor to the compositor chain
        m_compositor_instance = compositor_manager.addCompositor(
            m_viewport,
            m_core_compositor_name,
            0
        );
        compositor_manager.setCompositorEnabled(m_viewport, m_core_compositor_name, true);

        // If the final compositor has been removed, we need to add it to the compositor chain
        if(need_final_compositor_swap)
        {
            compositor_manager.addCompositor(m_viewport, FINAL_CHAIN_COMPOSITOR);
            compositor_manager.setCompositorEnabled(m_viewport, FINAL_CHAIN_COMPOSITOR, true);
        }

        if(m_compositor_instance == nullptr)
        {
            SIGHT_ERROR(
                "Compositor " + m_core_compositor_name
                + " script is missing in resources (check your resources' paths)"
            );
        }
    }
}

//-----------------------------------------------------------------------------

void core::set_transparency_depth_of_depth_peeling(int _depth)
{
    Ogre::CompositionTechnique* dp_comp_tech = m_compositor_instance->getTechnique();

    // Check if depthpeeling technique is already existing
    const int num_of_target_pass = static_cast<int>(dp_comp_tech->getTargetPasses().size());

    // 3 is the first ping pong target
    const int first_ping_pong_target = 4;
    for(int i = first_ping_pong_target ; i < num_of_target_pass ; i++)
    {
        // When a target is discarded, the next one becomes the current one (eg : if I remove the 2nd target,
        // the 3rd target becomes the new 2nd target)
        dp_comp_tech->removeTargetPass(first_ping_pong_target);
    }

    // Ping pong peel and blend
    for(int i = 0 ; i < _depth ; i++)
    {
        std::string ping_pong = (i % 2) != 0 ? "ing" : "ong";

        // Peel buffer
        {
            Ogre::CompositionTargetPass* dp_comp_target_peel = dp_comp_tech->createTargetPass();
            dp_comp_target_peel->setOutputName("p" + ping_pong + "Buffer");

            // No previous input
            dp_comp_target_peel->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

            // Clear pass
            {
                Ogre::CompositionPass* dp_comp_pass_clear = dp_comp_target_peel->createPass();
                dp_comp_pass_clear->setType(Ogre::CompositionPass::PT_CLEAR);
            }

            // Material scheme
            dp_comp_target_peel->setMaterialScheme(m_cell_shading_name + "DepthPeeling/peelP" + ping_pong);

            // No shadow
            dp_comp_target_peel->setShadowsEnabled(false);

            // Render scene pass
            {
                Ogre::CompositionPass* dp_comp_pass_render_scene = dp_comp_target_peel->createPass();
                dp_comp_pass_render_scene->setType(Ogre::CompositionPass::PT_RENDERSCENE);
                dp_comp_pass_render_scene->setLastRenderQueue(rq::SURFACE_ID);
            }
        }

        // Blend buffer
        {
            Ogre::CompositionTargetPass* dp_comp_target_blend = dp_comp_tech->createTargetPass();
            dp_comp_target_blend->setOutputName("gbuffer");

            // No previous input
            dp_comp_target_blend->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

            // Render quad
            {
                Ogre::CompositionPass* dp_comp_pass_render_quad = dp_comp_target_blend->createPass();
                dp_comp_pass_render_quad->setType(Ogre::CompositionPass::PT_RENDERQUAD);
                dp_comp_pass_render_quad->setMaterialName(m_cell_shading_name + "DepthPeeling/Blend");
                dp_comp_pass_render_quad->setInput(0, "p" + ping_pong + "Buffer", 0);
                if(!m_cell_shading_name.empty())
                {
                    dp_comp_pass_render_quad->setInput(1, "p" + ping_pong + "Buffer", 1);
                    dp_comp_pass_render_quad->setInput(2, "p" + ping_pong + "Buffer", 2);
                }
            }
        }
    }

    Ogre::CompositorManager::getSingleton().setCompositorEnabled(
        m_viewport,
        m_cell_shading_name + "DepthPeeling",
        true
    );
}

//-----------------------------------------------------------------------------

void core::set_transparency_depth_of_dual_depth_peeling(int _depth)
{
    Ogre::CompositionTechnique* dp_comp_tech = m_compositor_instance->getTechnique();

    // Check if depthpeeling technique is already existing
    const int num_of_target_pass = static_cast<int>(dp_comp_tech->getTargetPasses().size());

    // 3 is the first ping pong target
    const int first_ping_pong_target = 2;
    for(int i = first_ping_pong_target ; i < num_of_target_pass ; i++)
    {
        // When a target is discarded, the next one becomes the current one (eg : if I remove the 2nd target,
        // the 3rd target becomes the new 2nd target)
        dp_comp_tech->removeTargetPass(first_ping_pong_target);
    }

    // Ping pong peel and blend
    for(int i = 0 ; i < _depth ; i++)
    {
        std::string ping_pong = (i % 2) != 0 ? "ing" : "ong";

        // Peel buffer
        {
            Ogre::CompositionTargetPass* dp_comp_target_peel = dp_comp_tech->createTargetPass();
            dp_comp_target_peel->setOutputName("p" + ping_pong + "Buffer");

            // No previous input
            dp_comp_target_peel->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

            // Clear pass
            {
                Ogre::CompositionPass* dp_comp_pass_clear = dp_comp_target_peel->createPass();
                dp_comp_pass_clear->setType(Ogre::CompositionPass::PT_CLEAR);
                dp_comp_pass_clear->setClearColour(Ogre::ColourValue(-1.F, 0.F, 0.F, 0.F));
            }

            // Material scheme
            dp_comp_target_peel->setMaterialScheme("DualDepthPeeling/peelP" + ping_pong);

            // No shadow
            dp_comp_target_peel->setShadowsEnabled(false);

            // Render scene pass
            {
                Ogre::CompositionPass* dp_comp_pass_render_scene = dp_comp_target_peel->createPass();
                dp_comp_pass_render_scene->setType(Ogre::CompositionPass::PT_RENDERSCENE);
                dp_comp_pass_render_scene->setLastRenderQueue(rq::SURFACE_ID);
            }
        }

        // Blend buffer
        {
            Ogre::CompositionTargetPass* dp_comp_target_blend = dp_comp_tech->createTargetPass();
            dp_comp_target_blend->setOutputName("gbuffer");

            // No previous input
            dp_comp_target_blend->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

            // Render quad
            {
                Ogre::CompositionPass* dp_comp_pass_render_quad = dp_comp_target_blend->createPass();
                dp_comp_pass_render_quad->setType(Ogre::CompositionPass::PT_RENDERQUAD);
                dp_comp_pass_render_quad->setMaterialName("DualDepthPeeling/Blend");
                dp_comp_pass_render_quad->setInput(0, "p" + ping_pong + "Buffer", 3);
                dp_comp_pass_render_quad->setInput(1, "p" + ping_pong + "Buffer", 5);
            }
        }
    }

    Ogre::CompositorManager::getSingleton().setCompositorEnabled(m_viewport, "DualDepthPeeling", true);
}

//-----------------------------------------------------------------------------

void core::set_transparency_depth_of_hybrid_transparency(int _depth)
{
    Ogre::CompositionTechnique* dp_comp_tech = m_compositor_instance->getTechnique();

    // Check if hybrid transparency technique is already existing
    const int num_of_target_pass = static_cast<int>(dp_comp_tech->getTargetPasses().size());

    // 3 is the first ping pong target
    const int first_ping_pong_target = 4;

    for(int i = first_ping_pong_target ; i < num_of_target_pass ; i++)
    {
        // When a target is discarded, the next one becomes the current one (eg : if I remove the 2nd target,
        // the 3rd target becomes the new 2nd target)
        dp_comp_tech->removeTargetPass(first_ping_pong_target);
    }

    // Ping pong peel and blend
    for(int i = 0 ; i < (_depth / 2) * 2 ; i++)
    {
        std::string ping_pong = (i % 2) != 0 ? "ing" : "ong";

        // Peel buffer
        {
            Ogre::CompositionTargetPass* dp_comp_target_peel = dp_comp_tech->createTargetPass();
            dp_comp_target_peel->setOutputName("p" + ping_pong + "Buffer");

            // No previous input
            dp_comp_target_peel->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

            // Clear pass
            {
                Ogre::CompositionPass* dp_comp_pass_clear = dp_comp_target_peel->createPass();
                dp_comp_pass_clear->setType(Ogre::CompositionPass::PT_CLEAR);
            }

            // Material scheme
            dp_comp_target_peel->setMaterialScheme("HybridTransparency/peelP" + ping_pong);

            // No shadow
            dp_comp_target_peel->setShadowsEnabled(false);

            // Render scene pass
            {
                Ogre::CompositionPass* dp_comp_pass_render_scene = dp_comp_target_peel->createPass();
                dp_comp_pass_render_scene->setType(Ogre::CompositionPass::PT_RENDERSCENE);
                dp_comp_pass_render_scene->setLastRenderQueue(rq::SURFACE_ID);
            }
        }

        // Blend buffer
        {
            Ogre::CompositionTargetPass* dp_comp_target_blend = dp_comp_tech->createTargetPass();
            dp_comp_target_blend->setOutputName("gbuffer");

            // No previous input
            dp_comp_target_blend->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

            // Render quad
            {
                Ogre::CompositionPass* dp_comp_pass_render_quad = dp_comp_target_blend->createPass();
                dp_comp_pass_render_quad->setType(Ogre::CompositionPass::PT_RENDERQUAD);
                dp_comp_pass_render_quad->setMaterialName("DepthPeeling/Blend");
                dp_comp_pass_render_quad->setInput(0, "p" + ping_pong + "Buffer", 0);
            }
        }
    }

    // Occlusion buffer
    {
        Ogre::CompositionTargetPass* dp_comp_target_occlusion = dp_comp_tech->createTargetPass();
        dp_comp_target_occlusion->setOutputName("occlusion");

        // No previous input
        dp_comp_target_occlusion->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

        // Clear pass
        {
            Ogre::CompositionPass* dp_comp_pass_clear = dp_comp_target_occlusion->createPass();
            dp_comp_pass_clear->setType(Ogre::CompositionPass::PT_CLEAR);
            dp_comp_pass_clear->setClearColour(Ogre::ColourValue(1.F, 1.F, 1.F, 1.F));
        }

        // Material scheme
        dp_comp_target_occlusion->setMaterialScheme("HybridTransparency/occlusionMap");

        // No shadow
        dp_comp_target_occlusion->setShadowsEnabled(false);

        // Render scene pass
        {
            Ogre::CompositionPass* dp_comp_pass_render_scene = dp_comp_target_occlusion->createPass();
            dp_comp_pass_render_scene->setType(Ogre::CompositionPass::PT_RENDERSCENE);
            dp_comp_pass_render_scene->setLastRenderQueue(rq::SURFACE_ID);
        }
    }

    // Weight blend buffer
    {
        Ogre::CompositionTargetPass* dp_comp_target_weight_blend = dp_comp_tech->createTargetPass();
        dp_comp_target_weight_blend->setOutputName("weightedColor");

        // No previous input
        dp_comp_target_weight_blend->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

        // Clear pass
        {
            Ogre::CompositionPass* dp_comp_pass_clear = dp_comp_target_weight_blend->createPass();
            dp_comp_pass_clear->setType(Ogre::CompositionPass::PT_CLEAR);
            dp_comp_pass_clear->setClearBuffers(Ogre::FBT_COLOUR);
        }

        // Material scheme
        dp_comp_target_weight_blend->setMaterialScheme("HybridTransparency/weightBlend");

        // No shadow
        dp_comp_target_weight_blend->setShadowsEnabled(false);

        // Render scene pass
        {
            Ogre::CompositionPass* dp_comp_pass_render_scene = dp_comp_target_weight_blend->createPass();
            dp_comp_pass_render_scene->setType(Ogre::CompositionPass::PT_RENDERSCENE);
            dp_comp_pass_render_scene->setLastRenderQueue(rq::SURFACE_ID);
        }
    }

    // Transmittance blend buffer
    {
        Ogre::CompositionTargetPass* dp_comp_target_transmittance = dp_comp_tech->createTargetPass();
        dp_comp_target_transmittance->setOutputName("transmittance");

        // No previous input
        dp_comp_target_transmittance->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

        // Clear pass
        {
            Ogre::CompositionPass* dp_comp_pass_clear = dp_comp_target_transmittance->createPass();
            dp_comp_pass_clear->setType(Ogre::CompositionPass::PT_CLEAR);
            dp_comp_pass_clear->setClearColour(Ogre::ColourValue(1.F, 1.F, 1.F, 1.F));
        }

        // Material scheme
        dp_comp_target_transmittance->setMaterialScheme("HybridTransparency/transmittanceBlend");

        // No shadow
        dp_comp_target_transmittance->setShadowsEnabled(false);

        // Render scene pass
        {
            Ogre::CompositionPass* dp_comp_pass_render_scene = dp_comp_target_transmittance->createPass();
            dp_comp_pass_render_scene->setType(Ogre::CompositionPass::PT_RENDERSCENE);
            dp_comp_pass_render_scene->setLastRenderQueue(rq::SURFACE_ID);
        }
    }

    // WBOIT blend buffer
    {
        Ogre::CompositionTargetPass* dp_comp_target_wboit_blend = dp_comp_tech->createTargetPass();
        dp_comp_target_wboit_blend->setOutputName("WBOIT_output");

        // No previous input
        dp_comp_target_wboit_blend->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

        // Render quad
        {
            Ogre::CompositionPass* dp_comp_pass_render_quad = dp_comp_target_wboit_blend->createPass();
            dp_comp_pass_render_quad->setType(Ogre::CompositionPass::PT_RENDERQUAD);
            dp_comp_pass_render_quad->setMaterialName("HybridTransparency/WBOITBlend");
            dp_comp_pass_render_quad->setInput(0, "weightedColor");
            dp_comp_pass_render_quad->setInput(1, "transmittance");
        }
    }

    // Blend final (Depth Peeling + WBOIT) buffer
    {
        Ogre::CompositionTargetPass* dp_comp_target_final_blend = dp_comp_tech->createTargetPass();
        dp_comp_target_final_blend->setOutputName("gbuffer");

        // No previous input
        dp_comp_target_final_blend->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

        // Render quad
        {
            Ogre::CompositionPass* dp_comp_pass_render_quad = dp_comp_target_final_blend->createPass();
            dp_comp_pass_render_quad->setType(Ogre::CompositionPass::PT_RENDERQUAD);
            dp_comp_pass_render_quad->setMaterialName("HybridTransparency/BlendFinal");
            dp_comp_pass_render_quad->setInput(0, "WBOIT_output");
        }
    }

    Ogre::CompositorManager::getSingleton().setCompositorEnabled(m_viewport, "HybridTransparency", true);
}

//-------------------------------------------------------------------------------------

/*void core::setupQueries()
   {
    // Create the occlusion queries to be used in this sample
    try
       {
        Ogre::RenderSystem* renderSystem = Ogre::Root::getSingleton().getRenderSystem();
        m_OITQuery                         = renderSystem->createHardwareOcclusionQuery();

        m_useOcclusionQuery = (m_OITQuery != nullptr);
       }
       catch (Ogre::Exception e)
       {
        m_useOcclusionQuery = false;
       }

       if (m_useOcclusionQuery == false)
       {
        SIGHT_ERROR("Error: failed to create hardware occlusion query");
       }
       else
       {
        m_sceneManager->addRenderObjectListener(this);
        m_doOcclusionQuery = true;
       }
   }

   //-------------------------------------------------------------------------------------

   void core::notifyRenderSingleObject(Ogre::Renderable* rend, const Ogre::Pass* pass,
                                                 const Ogre::AutoParamDataSource* source,
                                                 const Ogre::LightList* pLightList,
                                                 bool suppressRenderStateChanges)
   {

       //
       // The following code activates and deactivates the occlusion queries
       // so that the queries only include the rendering of their intended targets
       //

       // Close the last occlusion query
       // Each occlusion query should only last a single rendering
       if (m_activeQuery != nullptr)
       {
        m_activeQuery->endOcclusionQuery();
        m_activeQuery = nullptr;
       }

       // Open a new occlusion query
       if (m_doOcclusionQuery == true)
       {
        // Check if a the object being rendered needs
        // to be occlusion queried
        for(int i = 0; i<3; i++)
           {
            Ogre::SceneManager::MovableObjectIterator iterator = m_sceneManager->getMovableObjectIterator("Entity");
            while(iterator.hasMoreElements())
            {
                Ogre::Entity* e = static_cast<Ogre::Entity*>(iterator.getNext());
                if(rend == (Ogre::Renderable *)e->getSubEntity(0))
                {
                    m_activeQuery = m_OITQuery;
                }
            }
           }
        m_activeQuery = m_OITQuery;

        // Number of fragments rendered in the last render pass
        unsigned int fragCount;
        bool test = m_OITQuery->pullOcclusionQuery(&fragCount);

        // Uncomment following for fragments counting display

        if(test)
        {
            char buffer[250] = {0};
            buffer[250] = 0;
            std::sprintf(buffer, "Comptage : [%d]", fragCount);
            std::cout<<buffer<<" frag for pass "<<pass->getName()<<" of material : "<<rend->getMaterial()->getName()<<
                std::endl;
        }

        int nbFragMax = m_parentLayer->getViewport()->getActualHeight()*m_parentLayer->getViewport()->getActualWidth();

        // Count the number of peels
        // To manage meshes, m_isPong and m_isPing allow an atomic incrementation
        // of m_currNumPass
        if(pass->getName() == "peel_pong" && !m_isPong  && fragCount != 0 && fragCount != nbFragMax)
        {
            m_currNumPass++;
            m_isPong = true;
            m_isPing = false;
        }
        else if(pass->getName() == "peel_ping" && !m_isPing  && fragCount != 0 && fragCount != nbFragMax)
        {
            m_currNumPass++;
            m_isPong = false;
            m_isPing = true;
        }

        // The final pass is the end of the depth peeling technique
        if(pass->getName() == "final")
        {
            // following incrementation renders another pass with 0 frag
            // (to reduce number of peels if necessary)
            m_currNumPass++;
            m_currNumPass = std::min(m_currNumPass, m_transparency_techniqueMaxDepth);
            // Compared to the previous number of peel
            if(m_num_pass != m_currNumPass)
            {
                // Uncomment following for number of passes info
                // std::cout<<"Depth of OIT updated : "<<m_currNumPass<<std::endl;
                this->setTransparencyDepth(m_currNumPass);
            }

            m_currNumPass = 0;
            m_isPong      = false;
            m_isPing      = false;
        }

        if (m_activeQuery != nullptr)
        {
            m_activeQuery->beginOcclusionQuery();
        }
       }
   }

   //-------------------------------------------------------------------------------------

   bool core::frameRenderingQueued(const Ogre::FrameEvent& evt)
   {
    if(m_renderWindow->isClosed())
       {
        return false;
       }

       // Modulate the light flare according to performed occlusion queries
       if (m_useOcclusionQuery)
       {
        // Stop occlusion queries until we get their information
        // (may not happen on the same frame they are requested in)
        m_doOcclusionQuery = false;

        // Check if all query information available
        if(m_OITQuery->isStillOutstanding() == false)
        {
            m_doOcclusionQuery = true;
        }
       }
    return true;
   }*/

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::compositor
