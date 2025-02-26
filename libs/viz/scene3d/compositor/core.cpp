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

#include <viz/scene3d/compositor/core.hpp>
#include <viz/scene3d/compositor/manager/oit.hpp>
#include <viz/scene3d/utils.hpp>

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

core::core(Ogre::Viewport* _viewport) :

    m_core_compositor_name("Default"),
    m_viewport(_viewport)
{
}

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

void core::set_transparency(transparency_technique _technique, int _depth)
{
    // Disable first the previous compositor
    try
    {
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(m_viewport, m_core_compositor_name, false);
    }
    catch(Ogre::InvalidParametersException&)
    {
        SIGHT_DEBUG("Can not find compositor: " + m_core_compositor_name);
    }

    // Enable then first the new compositor
    m_transparency_technique = _technique;
    m_num_pass               = _depth;

    m_cell_shading_name = "";

    SIGHT_ERROR_IF(
        "OIT isn't supported when stereo is enabled, falling back to mono rendering.",
        m_transparency_technique != DEFAULT && m_stereo_mode != stereo_mode_t::none
    );

    if(m_transparency_technique == DEFAULT)
    {
        m_core_compositor_name = STEREO_COMPOSITOR_MAP.at(m_stereo_mode);
        try
        {
            Ogre::CompositorManager::getSingleton().setCompositorEnabled(m_viewport, m_core_compositor_name, true);
        }
        catch(Ogre::InvalidParametersException&)
        {
            SIGHT_DEBUG("Can not find compositor: " + m_core_compositor_name);
        }
    }
    else
    {
        compositor::manager::oit::setup_transparency(m_viewport, m_transparency_technique, m_num_pass);
    }
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
