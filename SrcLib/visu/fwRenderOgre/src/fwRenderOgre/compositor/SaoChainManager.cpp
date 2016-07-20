/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/compositor/ChainManager.hpp"
#include "fwRenderOgre/compositor/SaoChainManager.hpp"

#include <fwCore/spyLog.hpp>

#include "OGRE/OgreCamera.h"

namespace fwRenderOgre
{

namespace compositor
{

//-----------------------------------------------------------------------------

// Here we add the class derivated from the Ogre Listener
class FWRENDEROGRE_CLASS_API SaoListener : public ::Ogre::CompositorInstance::Listener
{
public:
    SaoListener() = delete;
    SaoListener(::Ogre::Viewport* vp, SaoChainManager* saoChainManager) :
        m_VP(vp),
        m_parent(saoChainManager)
    {
    }

    // method called before a render_target operation involving a material to set dynamically the material parameters
    void notifyMaterialRender(::Ogre::uint32 pass_id, ::Ogre::MaterialPtr& mat)
    {
        // change the sao arguments
        auto fragmentParams = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

        // try to go here only when the AO_mat is called
        if (pass_id  == 1)
        {
            ::Ogre::CompositorChain* compChain = ::Ogre::CompositorManager::getSingletonPtr()->getCompositorChain(m_VP);

            ::Ogre::CompositorInstance* Sao_compositor = compChain->getCompositor("SAO");
            ::Ogre::TexturePtr mip0,mip1,mip2,mip3,mip4,mip5,mip6,mip7,mip8, rt0;

            mip0 = Sao_compositor->getTextureInstance("mip0",0);
            mip1 = Sao_compositor->getTextureInstance("mip1",0);
            mip2 = Sao_compositor->getTextureInstance("mip2",0);
            mip3 = Sao_compositor->getTextureInstance("mip3",0);
            mip4 = Sao_compositor->getTextureInstance("mip4",0);
            mip5 = Sao_compositor->getTextureInstance("mip5",0);
            mip6 = Sao_compositor->getTextureInstance("mip6",0);
            mip7 = Sao_compositor->getTextureInstance("mip7",0);
            mip8 = Sao_compositor->getTextureInstance("mip8",0);
            rt0  = Sao_compositor->getTextureInstance("rt0",0);

            // ---------------------------------------------------
            //  Copy the content of mip0,mip1... in rt0
            // ---------------------------------------------------
            rt0->freeInternalResources();

            rt0->changeGroupOwnership(mip0.get()->getGroup());
            rt0->setWidth(mip0.get()->getWidth());
            rt0->setHeight(mip0.get()->getHeight());
            rt0->setNumMipmaps(8);
            rt0->setFormat(::Ogre::PixelFormat::PF_FLOAT32_R);
            rt0->setUsage(::Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
            rt0->setTextureType(::Ogre::TextureType::TEX_TYPE_2D);

            rt0->createInternalResources();

            // copy the content of the mip textures in the mipmap (rt0)
            // TODO: try to automate with a loop
            rt0->getBuffer(0,0)->blit(mip0.get()->getBuffer());
            rt0->getBuffer(0,1)->blit(mip1.get()->getBuffer());
            rt0->getBuffer(0,2)->blit(mip2.get()->getBuffer());
            rt0->getBuffer(0,3)->blit(mip3.get()->getBuffer());
            rt0->getBuffer(0,4)->blit(mip4.get()->getBuffer());
            rt0->getBuffer(0,5)->blit(mip5.get()->getBuffer());
            rt0->getBuffer(0,6)->blit(mip6.get()->getBuffer());
            rt0->getBuffer(0,7)->blit(mip7.get()->getBuffer());
            rt0->getBuffer(0,8)->blit(mip8.get()->getBuffer());


            fragmentParams->setNamedConstant("u_radius",static_cast<float>(m_parent->getSaoRadius()));

            fragmentParams->setNamedConstant("u_numSamples",m_parent->getSaoSamples());

            const ::Ogre::Matrix4& proj = m_parent->getSceneCamera()->getProjectionMatrix();

            const ::Ogre::Vector4 projInfo( -2.f / (static_cast<float>(mip0.get()->getWidth() * proj[0][0])),
                                            -2.f / (static_cast<float>(mip0.get()->getHeight() * proj[1][1])),
                                            (1.f - proj[0][2]) / proj[0][0],
                                            (1.f + proj[1][2]) / proj[1][1]);
            fragmentParams->setNamedConstant("u_projInfo",projInfo);
        }

        if (pass_id  >= 41)
        {
            ::Ogre::CompositorChain* compChain = ::Ogre::CompositorManager::getSingletonPtr()->getCompositorChain(m_VP);

            ::Ogre::CompositorInstance* Sao_compositor = compChain->getCompositor("SAO");

            ::Ogre::TexturePtr prevMip = Sao_compositor->getTextureInstance("mip" + std::to_string(pass_id - 41), 0);

            fragmentParams->setNamedConstant("u_vpWidth",static_cast<float>(prevMip.get()->getWidth()));
            fragmentParams->setNamedConstant("u_vpHeight",static_cast<float>(prevMip.get()->getHeight()));
        }

        if (pass_id == 4)
        {
            // Change the Blend State

            // setNamedConstant doesn't work with bool value
            int state = m_parent->getBlend() ? 0 : 1;

            // change the blend state
            fragmentParams->setNamedConstant("u_blend", state);

            // Change the AO Intensity value
            fragmentParams->setNamedConstant("aoIntensity",static_cast<float>(m_parent->getAoIntensity()));
        }
    }

private:
    ::Ogre::Viewport* m_VP;

    SaoChainManager* m_parent;
};


//-----------------------------------------------------------------------------

SaoChainManager::SaoChainManager(::Ogre::Viewport* viewport, ::Ogre::Camera* camera) :
    m_ogreViewport(viewport),
    m_saoRadius(0.85),
    m_saoSamples(11),
    m_blend(false),
    m_aoIntensity(1.0),
    m_camera(camera)
{
    // test with only one compositor
    m_saoChain.push_back("SAO");
//    m_saoChain.push_back("FinalChainCompositor");

}

//-----------------------------------------------------------------------------

::Ogre::CompositorManager& SaoChainManager::getCompositorManager()
{
    return ::Ogre::CompositorManager::getSingleton();
}

//-----------------------------------------------------------------------------

void SaoChainManager::setSaoState(bool state)
{
    // get the Compositor Manager of Ogre (of the scene)
    ::Ogre::CompositorManager& compositorManager = this->getCompositorManager();

    // if here, we add the compositor present of the chain in the Ogre chain compositor
    if (state)
    {
        const std::string FINAL_CHAIN_COMPOSITOR = ::fwRenderOgre::compositor::ChainManager::FINAL_CHAIN_COMPOSITOR;

        // check the content of the Ogre Compositor chain
        ::Ogre::CompositorChain* compChain = compositorManager.getCompositorChain(m_ogreViewport);

        // add final compositor but disable
        bool needFinalCompositorSwap(false);
        // if not present in the chain add it and disable it
        if(!(compositorManager.getByName(FINAL_CHAIN_COMPOSITOR)).isNull())
        {
            compositorManager.setCompositorEnabled(m_ogreViewport, FINAL_CHAIN_COMPOSITOR, false);
            compositorManager.removeCompositor(m_ogreViewport, FINAL_CHAIN_COMPOSITOR);
            needFinalCompositorSwap = true;
        }


        if (compChain->getCompositor("SAO") != nullptr)
        {
            compositorManager.removeCompositor(m_ogreViewport,"SAO"); // suppression pour ajout à la fin de la chaine
        }

        compositorManager.addCompositor(m_ogreViewport,"SAO");
        compChain->getCompositor("SAO")->addListener(new SaoListener(m_ogreViewport,this));

        if (needFinalCompositorSwap)
        {
            compositorManager.addCompositor(m_ogreViewport,FINAL_CHAIN_COMPOSITOR);
            compositorManager.setCompositorEnabled(m_ogreViewport,FINAL_CHAIN_COMPOSITOR,true);
        }

        // add all the compositor of the chain
        for(SaoCompositorIdType compositorName : m_saoChain)
        {
            if(compositorManager.resourceExists(compositorName))
            {
                //compositorManager.addCompositor(m_ogreViewport, compositorName);
                compositorManager.setCompositorEnabled(m_ogreViewport, compositorName, true);
            }
            else
            {
                OSLM_WARN("\"" << compositorName << "\" does not refer to an existing compositor");
            }
        }
    }
    else // disable the sao Chain
    {
        for(SaoCompositorIdType compositorName : m_saoChain)
        {
            if(compositorManager.resourceExists(compositorName))
            {
                compositorManager.setCompositorEnabled(m_ogreViewport, compositorName, false);
            }
        }
    }
}

//-----------------------------------------------------------------------------

double SaoChainManager::getSaoRadius()
{
    return m_saoRadius;
}

//-----------------------------------------------------------------------------

void SaoChainManager::setSaoRadius(double newRadius)
{
    m_saoRadius = newRadius;
}

//-----------------------------------------------------------------------------

int SaoChainManager::getSaoSamples()
{
    return m_saoSamples;
}

//-----------------------------------------------------------------------------

void SaoChainManager::setSaoSamples(int newSamplesNumber)
{
    m_saoSamples = newSamplesNumber;
}

//-----------------------------------------------------------------------------

bool SaoChainManager::getBlend()
{
    return m_blend;
}

//-----------------------------------------------------------------------------

void SaoChainManager::enableBlend(bool state)
{
    m_blend = state;
}

//-----------------------------------------------------------------------------

double SaoChainManager::getAoIntensity()
{
    return m_aoIntensity;
}

//-----------------------------------------------------------------------------

void SaoChainManager::setAoIntensity(double new_intensity)
{
    m_aoIntensity = new_intensity;
}

//-----------------------------------------------------------------------------

} // namespace compositor

}// namespace fwRenderOgre
