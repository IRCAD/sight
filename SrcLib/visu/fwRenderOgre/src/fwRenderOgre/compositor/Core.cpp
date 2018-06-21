/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRenderOgre/compositor/Core.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <OGRE/OgreCompositionPass.h>
#include <OGRE/OgreCompositionTargetPass.h>
#include <OGRE/OgreCompositor.h>
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreHardwareOcclusionQuery.h>

namespace fwRenderOgre
{

namespace compositor
{

// ----------------------------------------------------------------------------

const std::string Core::FINAL_CHAIN_COMPOSITOR = "FinalChainCompositor";

// ----------------------------------------------------------------------------

Core::Core(::Ogre::Viewport* viewport) :
    //m_transparencyTechniqueMaxDepth(8),
    m_transparencyTechnique(DEFAULT),
    m_transparencyTechniqueName("Default"),
    m_compositorInstance(nullptr),
    //m_useOcclusionQuery(false),
    //m_doOcclusionQuery(false),
    //m_OITQuery(nullptr),
    //m_activeQuery(nullptr),
    //m_currNumPass(0),
    //m_isPing(false),
    //m_isPong(false),
    m_celShadingName(""),
    m_numPass(8),
    m_viewport(viewport)
{
}

//-----------------------------------------------------------------------------

Core::~Core()
{
}

//-----------------------------------------------------------------------------

transparencyTechnique Core::getTransparencyTechnique()
{
    return m_transparencyTechnique;
}

//-----------------------------------------------------------------------------

int Core::getTransparencyDepth()
{
    return m_numPass;
}

//-----------------------------------------------------------------------------

bool Core::setTransparencyTechnique(transparencyTechnique technique)
{
    ::Ogre::CompositorManager::getSingleton().setCompositorEnabled( m_viewport, m_transparencyTechniqueName, false );
    m_transparencyTechnique = technique;

    return true;
}

//-----------------------------------------------------------------------------

void Core::update()
{
    m_celShadingName = "";

    switch (m_transparencyTechnique)
    {
        case DEFAULT:
            m_transparencyTechniqueName = "Default";
            this->setupTransparency();
            this->setupDefaultTransparency();
            break;
        case CELSHADING_DEPTHPEELING:
            m_celShadingName = "CelShading";
            BOOST_FALLTHROUGH;
        case DEPTHPEELING:
            m_transparencyTechniqueName = m_celShadingName+"DepthPeeling";
            this->setupTransparency();
            this->setTransparencyDepthOfDepthPeeling(m_numPass);
            break;
        case DUALDEPTHPEELING:
            m_transparencyTechniqueName = "DualDepthPeeling";
            this->setupTransparency();
            this->setTransparencyDepthOfDualDepthPeeling(m_numPass);
            break;
        case WEIGHTEDBLENDEDOIT:
            m_transparencyTechniqueName = "WeightedBlended";
            this->setupTransparency();
            ::Ogre::CompositorManager::getSingleton().setCompositorEnabled( m_viewport, "WeightedBlended", true );
            break;
        case HYBRIDTRANSPARENCY:
            m_transparencyTechniqueName = "HybridTransparency";
            this->setupTransparency();
            this->setTransparencyDepthOfHybridTransparency(m_numPass);
            break;
    }

}

//-----------------------------------------------------------------------------

void Core::setTransparencyDepth(int depth)
{
    ::Ogre::CompositorManager::getSingleton().setCompositorEnabled( m_viewport, m_transparencyTechniqueName, false );
    m_numPass = depth;
}

//-----------------------------------------------------------------------------

void Core::setupDefaultTransparency()
{
    ::Ogre::CompositorManager::getSingleton().setCompositorEnabled( m_viewport, "Default", true );
}

//-----------------------------------------------------------------------------

void Core::setupTransparency()
{
    // Check if compositor is already existing
    ::Ogre::CompositorChain* compChain = ::Ogre::CompositorManager::getSingleton().getCompositorChain(
        m_viewport);
    ::Ogre::CompositorChain::InstanceIterator compIter = compChain->getCompositors();

    m_compositorInstance = nullptr;
    while( compIter.hasMoreElements())
    {
        ::Ogre::CompositorInstance* targetComp = compIter.getNext();
        if(targetComp->getCompositor()->getName() == m_transparencyTechniqueName)
        {
            m_compositorInstance = targetComp;
            break;
        }
    }

    // If we didn't retrieve the good compositor
    if(m_compositorInstance == nullptr)
    {
        ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
        bool needFinalCompositorSwap(false);

        // If the compositor chain already contains the final compositor, we have to remove it
        if(compositorManager.getByName(FINAL_CHAIN_COMPOSITOR))
        {
            compositorManager.setCompositorEnabled(m_viewport, FINAL_CHAIN_COMPOSITOR, false);
            compositorManager.removeCompositor(m_viewport, FINAL_CHAIN_COMPOSITOR);
            needFinalCompositorSwap = true;
        }

        // Now, we can add the new compositor to the compositor chain
        m_compositorInstance = compositorManager.addCompositor( m_viewport,
                                                                m_transparencyTechniqueName,
                                                                0);
        compositorManager.setCompositorEnabled( m_viewport, m_transparencyTechniqueName, true );

        // If the final compositor has been removed, we need to add it to the compositor chain
        if(needFinalCompositorSwap)
        {
            compositorManager.addCompositor(m_viewport, FINAL_CHAIN_COMPOSITOR);
            compositorManager.setCompositorEnabled(m_viewport, FINAL_CHAIN_COMPOSITOR, true);
        }

        if(m_compositorInstance == nullptr)
        {
            SLM_ERROR( "Compositor " + m_transparencyTechniqueName +
                       " script is missing in resources (check your resources' paths)");
        }
    }
}

//-----------------------------------------------------------------------------

void Core::setTransparencyDepthOfDepthPeeling(int depth)
{
    ::Ogre::CompositionTechnique* dpCompTech = m_compositorInstance->getTechnique();

    // Check if depthpeeling technique is already existing
    const int numOfTargetPass = static_cast<int>(dpCompTech->getTargetPasses().size());

    // 3 is the first ping pong target
    const int firstPingPongTarget = 4;
    for (int i = firstPingPongTarget; i < numOfTargetPass; i++)
    {
        // When a target is discarded, the next one becomes the current one (eg : if I remove the 2nd target,
        // the 3rd target becomes the new 2nd target)
        dpCompTech->removeTargetPass(firstPingPongTarget);
    }

    // Ping pong peel and blend
    for(int i = 0; i < depth; i++)
    {
        std::string pingPong = (i%2) ? "ing" : "ong";

        // Peel buffer
        {
            ::Ogre::CompositionTargetPass* dpCompTargetPeel = dpCompTech->createTargetPass();
            dpCompTargetPeel->setOutputName("p" + pingPong + "Buffer");

            // No previous input
            dpCompTargetPeel->setInputMode(::Ogre::CompositionTargetPass::IM_NONE);

            // Clear pass
            {
                ::Ogre::CompositionPass* dpCompPassClear = dpCompTargetPeel->createPass();
                dpCompPassClear->setType(::Ogre::CompositionPass::PT_CLEAR);
            }

            // Material scheme
            dpCompTargetPeel->setMaterialScheme(m_celShadingName+"DepthPeeling/peelP"+pingPong);

            // No shadow
            dpCompTargetPeel->setShadowsEnabled(false);

            // Render scene pass
            {
                ::Ogre::CompositionPass* dpCompPassRenderScene = dpCompTargetPeel->createPass();
                dpCompPassRenderScene->setType(::Ogre::CompositionPass::PT_RENDERSCENE);
                dpCompPassRenderScene->setLastRenderQueue(s_SURFACE_RQ_GROUP_ID);
            }
        }

        // Blend buffer
        {
            ::Ogre::CompositionTargetPass* dpCompTargetBlend = dpCompTech->createTargetPass();
            dpCompTargetBlend->setOutputName("gbuffer");

            // No previous input
            dpCompTargetBlend->setInputMode(::Ogre::CompositionTargetPass::IM_NONE);

            // Render quad
            {
                ::Ogre::CompositionPass* dpCompPassRenderQuad = dpCompTargetBlend->createPass();
                dpCompPassRenderQuad->setType(::Ogre::CompositionPass::PT_RENDERQUAD);
                dpCompPassRenderQuad->setMaterialName(m_celShadingName+"DepthPeeling/Blend");
                dpCompPassRenderQuad->setInput(0, "p" + pingPong + "Buffer", 0);
                if(!m_celShadingName.empty())
                {
                    dpCompPassRenderQuad->setInput(1, "p" + pingPong + "Buffer", 1);
                    dpCompPassRenderQuad->setInput(2, "p" + pingPong + "Buffer", 2);
                }

            }
        }
    }

    ::Ogre::CompositorManager::getSingleton().setCompositorEnabled( m_viewport, m_celShadingName+"DepthPeeling", true );
}

//-----------------------------------------------------------------------------

void Core::setTransparencyDepthOfDualDepthPeeling(int depth)
{
    ::Ogre::CompositionTechnique* dpCompTech = m_compositorInstance->getTechnique();

    // Check if depthpeeling technique is already existing
    const int numOfTargetPass = static_cast<int>(dpCompTech->getTargetPasses().size());

    // 3 is the first ping pong target
    const int firstPingPongTarget = 2;
    for (int i = firstPingPongTarget; i < numOfTargetPass; i++)
    {
        // When a target is discarded, the next one becomes the current one (eg : if I remove the 2nd target,
        // the 3rd target becomes the new 2nd target)
        dpCompTech->removeTargetPass(firstPingPongTarget);
    }

    // Ping pong peel and blend
    for(int i = 0; i < depth; i++)
    {
        std::string pingPong = (i%2) ? "ing" : "ong";

        // Peel buffer
        {
            ::Ogre::CompositionTargetPass* dpCompTargetPeel = dpCompTech->createTargetPass();
            dpCompTargetPeel->setOutputName("p" + pingPong + "Buffer");

            // No previous input
            dpCompTargetPeel->setInputMode(::Ogre::CompositionTargetPass::IM_NONE);

            // Clear pass
            {
                ::Ogre::CompositionPass* dpCompPassClear = dpCompTargetPeel->createPass();
                dpCompPassClear->setType(::Ogre::CompositionPass::PT_CLEAR);
                dpCompPassClear->setClearColour(::Ogre::ColourValue(-1.f, 0.f, 0.f, 0.f));
            }

            // Material scheme
            dpCompTargetPeel->setMaterialScheme("DualDepthPeeling/peelP"+pingPong);

            // No shadow
            dpCompTargetPeel->setShadowsEnabled(false);

            // Render scene pass
            {
                ::Ogre::CompositionPass* dpCompPassRenderScene = dpCompTargetPeel->createPass();
                dpCompPassRenderScene->setType(::Ogre::CompositionPass::PT_RENDERSCENE);
                dpCompPassRenderScene->setLastRenderQueue(s_SURFACE_RQ_GROUP_ID);
            }
        }

        // Blend buffer
        {
            ::Ogre::CompositionTargetPass* dpCompTargetBlend = dpCompTech->createTargetPass();
            dpCompTargetBlend->setOutputName("gbuffer");

            // No previous input
            dpCompTargetBlend->setInputMode(::Ogre::CompositionTargetPass::IM_NONE);

            // Render quad
            {
                ::Ogre::CompositionPass* dpCompPassRenderQuad = dpCompTargetBlend->createPass();
                dpCompPassRenderQuad->setType(::Ogre::CompositionPass::PT_RENDERQUAD);
                dpCompPassRenderQuad->setMaterialName("DualDepthPeeling/Blend");
                dpCompPassRenderQuad->setInput(0, "p" + pingPong + "Buffer", 3);
                dpCompPassRenderQuad->setInput(1, "p" + pingPong + "Buffer", 5);
            }
        }
    }

    ::Ogre::CompositorManager::getSingleton().setCompositorEnabled( m_viewport, "DualDepthPeeling", true );
}

//-----------------------------------------------------------------------------

void Core::setTransparencyDepthOfHybridTransparency(int depth)
{
    ::Ogre::CompositionTechnique* dpCompTech = m_compositorInstance->getTechnique();

    // Check if hybrid transparency technique is already existing
    const int numOfTargetPass = static_cast<int>(dpCompTech->getTargetPasses().size());

    // 3 is the first ping pong target
    const int firstPingPongTarget = 4;

    for (int i = firstPingPongTarget; i < numOfTargetPass; i++)
    {
        // When a target is discarded, the next one becomes the current one (eg : if I remove the 2nd target,
        // the 3rd target becomes the new 2nd target)
        dpCompTech->removeTargetPass(firstPingPongTarget);
    }

    // Ping pong peel and blend
    for(int i = 0; i < (depth/2)*2; i++)
    {
        std::string pingPong = (i%2) ? "ing" : "ong";

        // Peel buffer
        {
            ::Ogre::CompositionTargetPass* dpCompTargetPeel = dpCompTech->createTargetPass();
            dpCompTargetPeel->setOutputName("p" + pingPong + "Buffer");

            // No previous input
            dpCompTargetPeel->setInputMode(::Ogre::CompositionTargetPass::IM_NONE);

            // Clear pass
            {
                ::Ogre::CompositionPass* dpCompPassClear = dpCompTargetPeel->createPass();
                dpCompPassClear->setType(::Ogre::CompositionPass::PT_CLEAR);
            }

            // Material scheme
            dpCompTargetPeel->setMaterialScheme("HybridTransparency/peelP"+pingPong);

            // No shadow
            dpCompTargetPeel->setShadowsEnabled(false);

            // Render scene pass
            {
                ::Ogre::CompositionPass* dpCompPassRenderScene = dpCompTargetPeel->createPass();
                dpCompPassRenderScene->setType(::Ogre::CompositionPass::PT_RENDERSCENE);
                dpCompPassRenderScene->setLastRenderQueue(s_SURFACE_RQ_GROUP_ID);
            }
        }

        // Blend buffer
        {
            ::Ogre::CompositionTargetPass* dpCompTargetBlend = dpCompTech->createTargetPass();
            dpCompTargetBlend->setOutputName("gbuffer");

            // No previous input
            dpCompTargetBlend->setInputMode(::Ogre::CompositionTargetPass::IM_NONE);

            // Render quad
            {
                ::Ogre::CompositionPass* dpCompPassRenderQuad = dpCompTargetBlend->createPass();
                dpCompPassRenderQuad->setType(::Ogre::CompositionPass::PT_RENDERQUAD);
                dpCompPassRenderQuad->setMaterialName("DepthPeeling/Blend");
                dpCompPassRenderQuad->setInput(0, "p" + pingPong + "Buffer", 0);
            }
        }
    }

    // Occlusion buffer
    {
        ::Ogre::CompositionTargetPass* dpCompTargetOcclusion = dpCompTech->createTargetPass();
        dpCompTargetOcclusion->setOutputName("occlusion");

        // No previous input
        dpCompTargetOcclusion->setInputMode(::Ogre::CompositionTargetPass::IM_NONE);

        // Clear pass
        {
            ::Ogre::CompositionPass* dpCompPassClear = dpCompTargetOcclusion->createPass();
            dpCompPassClear->setType(::Ogre::CompositionPass::PT_CLEAR);
            dpCompPassClear->setClearColour(::Ogre::ColourValue(1.f, 1.f, 1.f, 1.f));
        }

        // Material scheme
        dpCompTargetOcclusion->setMaterialScheme("HybridTransparency/occlusionMap");

        // No shadow
        dpCompTargetOcclusion->setShadowsEnabled(false);

        // Render scene pass
        {
            ::Ogre::CompositionPass* dpCompPassRenderScene = dpCompTargetOcclusion->createPass();
            dpCompPassRenderScene->setType(::Ogre::CompositionPass::PT_RENDERSCENE);
            dpCompPassRenderScene->setLastRenderQueue(s_SURFACE_RQ_GROUP_ID);
        }
    }

    // Weight blend buffer
    {
        ::Ogre::CompositionTargetPass* dpCompTargetWeightBlend = dpCompTech->createTargetPass();
        dpCompTargetWeightBlend->setOutputName("weightedColor");

        // No previous input
        dpCompTargetWeightBlend->setInputMode(::Ogre::CompositionTargetPass::IM_NONE);

        // Clear pass
        {
            ::Ogre::CompositionPass* dpCompPassClear = dpCompTargetWeightBlend->createPass();
            dpCompPassClear->setType(::Ogre::CompositionPass::PT_CLEAR);
            dpCompPassClear->setClearBuffers(::Ogre::FBT_COLOUR);
        }

        // Material scheme
        dpCompTargetWeightBlend->setMaterialScheme("HybridTransparency/weightBlend");

        // No shadow
        dpCompTargetWeightBlend->setShadowsEnabled(false);

        // Render scene pass
        {
            ::Ogre::CompositionPass* dpCompPassRenderScene = dpCompTargetWeightBlend->createPass();
            dpCompPassRenderScene->setType(::Ogre::CompositionPass::PT_RENDERSCENE);
            dpCompPassRenderScene->setLastRenderQueue(s_SURFACE_RQ_GROUP_ID);
        }
    }

    // Transmittance blend buffer
    {
        ::Ogre::CompositionTargetPass* dpCompTargetTransmittance = dpCompTech->createTargetPass();
        dpCompTargetTransmittance->setOutputName("transmittance");

        // No previous input
        dpCompTargetTransmittance->setInputMode(::Ogre::CompositionTargetPass::IM_NONE);

        // Clear pass
        {
            ::Ogre::CompositionPass* dpCompPassClear = dpCompTargetTransmittance->createPass();
            dpCompPassClear->setType(::Ogre::CompositionPass::PT_CLEAR);
            dpCompPassClear->setClearColour(::Ogre::ColourValue(1.f, 1.f, 1.f, 1.f));
        }

        // Material scheme
        dpCompTargetTransmittance->setMaterialScheme("HybridTransparency/transmittanceBlend");

        // No shadow
        dpCompTargetTransmittance->setShadowsEnabled(false);

        // Render scene pass
        {
            ::Ogre::CompositionPass* dpCompPassRenderScene = dpCompTargetTransmittance->createPass();
            dpCompPassRenderScene->setType(::Ogre::CompositionPass::PT_RENDERSCENE);
            dpCompPassRenderScene->setLastRenderQueue(s_SURFACE_RQ_GROUP_ID);
        }
    }

    // WBOIT blend buffer
    {
        ::Ogre::CompositionTargetPass* dpCompTargetWBOITBlend = dpCompTech->createTargetPass();
        dpCompTargetWBOITBlend->setOutputName("WBOIT_output");

        // No previous input
        dpCompTargetWBOITBlend->setInputMode(::Ogre::CompositionTargetPass::IM_NONE);

        // Render quad
        {
            ::Ogre::CompositionPass* dpCompPassRenderQuad = dpCompTargetWBOITBlend->createPass();
            dpCompPassRenderQuad->setType(::Ogre::CompositionPass::PT_RENDERQUAD);
            dpCompPassRenderQuad->setMaterialName("HybridTransparency/WBOITBlend");
            dpCompPassRenderQuad->setInput(0, "weightedColor");
            dpCompPassRenderQuad->setInput(1, "transmittance");
        }
    }

    // Blend final (Depth Peeling + WBOIT) buffer
    {
        ::Ogre::CompositionTargetPass* dpCompTargetFinalBlend = dpCompTech->createTargetPass();
        dpCompTargetFinalBlend->setOutputName("gbuffer");

        // No previous input
        dpCompTargetFinalBlend->setInputMode(::Ogre::CompositionTargetPass::IM_NONE);

        // Render quad
        {
            ::Ogre::CompositionPass* dpCompPassRenderQuad = dpCompTargetFinalBlend->createPass();
            dpCompPassRenderQuad->setType(::Ogre::CompositionPass::PT_RENDERQUAD);
            dpCompPassRenderQuad->setMaterialName("HybridTransparency/BlendFinal");
            dpCompPassRenderQuad->setInput(0, "WBOIT_output");
        }
    }

    ::Ogre::CompositorManager::getSingleton().setCompositorEnabled( m_viewport, "HybridTransparency", true );

}

//-------------------------------------------------------------------------------------

/*void Core::setupQueries()
   {
    // Create the occlusion queries to be used in this sample
    try
       {
        ::Ogre::RenderSystem* renderSystem = Ogre::Root::getSingleton().getRenderSystem();
        m_OITQuery                         = renderSystem->createHardwareOcclusionQuery();

        m_useOcclusionQuery = (m_OITQuery != nullptr);
       }
       catch (::Ogre::Exception e)
       {
        m_useOcclusionQuery = false;
       }

       if (m_useOcclusionQuery == false)
       {
        OSLM_ERROR("Error: failed to create hardware occlusion query");
       }
       else
       {
        m_sceneManager->addRenderObjectListener(this);
        m_doOcclusionQuery = true;
       }
   }

   //-------------------------------------------------------------------------------------

   void Core::notifyRenderSingleObject(::Ogre::Renderable* rend, const ::Ogre::Pass* pass,
                                                 const ::Ogre::AutoParamDataSource* source,
                                                 const ::Ogre::LightList* pLightList,
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
            ::Ogre::SceneManager::MovableObjectIterator iterator = m_sceneManager->getMovableObjectIterator("Entity");
            while(iterator.hasMoreElements())
            {
                Ogre::Entity* e = static_cast<Ogre::Entity*>(iterator.getNext());
                if(rend == (::Ogre::Renderable *)e->getSubEntity(0))
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
            m_currNumPass = std::min(m_currNumPass, m_transparencyTechniqueMaxDepth);
            // Compared to the previous number of peel
            if(m_numPass != m_currNumPass)
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

   bool Core::frameRenderingQueued(const Ogre::FrameEvent& evt)
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

} // namespace compositor

} // namespace fwRenderOgre
