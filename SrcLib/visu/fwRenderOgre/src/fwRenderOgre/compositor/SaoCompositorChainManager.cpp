#include "fwRenderOgre/compositor/SaoCompositorChainManager.hpp"

#include <fwCore/spyLog.hpp>

#include <fwCore/spyLog.hpp>

#include "OGRE/OgreCamera.h"

namespace fwRenderOgre {

//-----------------------------------------------------------------------------


// Here we add the class derivated from the Ogre Listener
class FWRENDEROGRE_CLASS_API SaoListener : public ::Ogre::CompositorInstance::Listener
{
public:
    SaoListener():m_VP(nullptr),m_Parent(nullptr){}
    SaoListener(::Ogre::Viewport* vp,SaoCompositorChainManager* saoChainManager):m_VP(vp),m_Parent(saoChainManager){}
    ~SaoListener(){}

    SaoCompositorChainManager* getParent(){return m_Parent;}


    void notifyMaterialRender(::Ogre::uint32 pass_id, ::Ogre::MaterialPtr& mat)
        // method called before a render_target operation involving a material to set dynamically the material parameters
    {
        // Test with the two separated compositor

        /*

        //§ When entering in this method, the Mip Map compositor has finished its work

        if (mat.get()->getName() == ::Ogre::MaterialManager::getSingletonPtr()->getByName("AO_mat").get()->getName())
            std::cout << "Mat Ao_mat appelé" << std::endl;

//                if (pass_id == 1)
//                {
        // no way to do a if with the material name

        std::cout << "No? " << std::endl;
        std::cout << mat.get()->getName() << std::endl;


        // ---------------------------------------------------
        //  Try to change the content of the Second Compositor
        // ---------------------------------------------------


        // get a pointer on the precedent compositor


        ::Ogre::CompositorChain* compChain =
            ::Ogre::CompositorManager::getSingletonPtr()->getCompositorChain(m_VP);
        ::Ogre::CompositorChain::InstanceIterator compIter = compChain->getCompositors();


//                ::Ogre::Compositor *MipMap,*Test;
        ::Ogre::TexturePtr mip0,mip1,mip2,mip3,mip4,mip5,mip6,mip7,mip8, rt0;
        while( compIter.hasMoreElements())
        {
            ::Ogre::CompositorInstance* targetComp = compIter.getNext();
            if (targetComp->getCompositor()->getName() == "MipMap")
            {
                mip0 = targetComp->getTextureInstance("mip0",0);
                mip1 = targetComp->getTextureInstance("mip1",0);
                mip2 = targetComp->getTextureInstance("mip2",0);
                mip3 = targetComp->getTextureInstance("mip3",0);
                mip4 = targetComp->getTextureInstance("mip4",0);
                mip5 = targetComp->getTextureInstance("mip5",0);
                mip6 = targetComp->getTextureInstance("mip6",0);
                mip7 = targetComp->getTextureInstance("mip7",0);
                mip8 = targetComp->getTextureInstance("mip8",0);
            }
            if (targetComp->getCompositor()->getName() == "AO_Samples")
            {
                rt0 = targetComp->getTextureInstance("rt0",0);
            }
        }


        // ---------------------------------------------------
        //  Copy the content of mip0,mip1... in rt0
        // ---------------------------------------------------


        rt0.get()->freeInternalResources();

        rt0.get()->changeGroupOwnership(mip0.get()->getGroup());
        rt0.get()->setWidth(mip0.get()->getWidth());
        rt0.get()->setHeight(mip0.get()->getHeight());
        rt0.get()->setNumMipmaps(8);
        rt0.get()->setFormat(::Ogre::PixelFormat::PF_FLOAT32_R);
        rt0.get()->setUsage(::Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
        rt0.get()->setTextureType(::Ogre::TextureType::TEX_TYPE_2D);

        rt0.get()->createInternalResources();

        // copy the content of the mip textures in the mipmap (rt0)
        // essayer de le faire de manière automatique avec une boucle + fonction qui récupèrerai l'ensemble des textures de MipMap dans un conteneur

        rt0.get()->getBuffer(0,0)->blit(mip0.get()->getBuffer());
        rt0.get()->getBuffer(0,1)->blit(mip1.get()->getBuffer());
        rt0.get()->getBuffer(0,2)->blit(mip2.get()->getBuffer());
        rt0.get()->getBuffer(0,3)->blit(mip3.get()->getBuffer());
        rt0.get()->getBuffer(0,4)->blit(mip4.get()->getBuffer());
        rt0.get()->getBuffer(0,5)->blit(mip5.get()->getBuffer());
        rt0.get()->getBuffer(0,6)->blit(mip6.get()->getBuffer());
        rt0.get()->getBuffer(0,7)->blit(mip7.get()->getBuffer());
        rt0.get()->getBuffer(0,8)->blit(mip8.get()->getBuffer());

        if (pass_id == 1)
        {
            std::cout << "AO_mat material called" << std::endl;
//                    mat.get()->getTechnique(0)->getPass(0)->getFragmentProgram().get()->setParameter("r",::Ogre::StringConverter::toString(this->getParent()->getSaoRadius()));

            mat.get()->getTechnique(0)->getPass(0)->getFragmentProgramParameters().get()->setNamedConstant("r",static_cast<float>(this->getParent()->getSaoRadius()));

            mat.get()->getTechnique(0)->getPass(0)->getFragmentProgramParameters().get()->setNamedConstant("s",this->getParent()->getSaoSamples());

        }

        */


        // test with only one compositor

        // try to go here only when the AO_mat is called
        if (pass_id  == 1)
        {

            std::cout << "we are in the pass with the identifier 1" << std::endl;

            ::Ogre::CompositorChain* compChain =
                ::Ogre::CompositorManager::getSingletonPtr()->getCompositorChain(m_VP);


            ::Ogre::CompositorInstance * Sao_compositor = compChain->getCompositor("SAO_complet");
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
            rt0 = Sao_compositor->getTextureInstance("rt0",0);

            // ---------------------------------------------------
            //  Copy the content of mip0,mip1... in rt0
            // ---------------------------------------------------


            rt0.get()->freeInternalResources();

            rt0.get()->changeGroupOwnership(mip0.get()->getGroup());
            rt0.get()->setWidth(mip0.get()->getWidth());
            rt0.get()->setHeight(mip0.get()->getHeight());
            rt0.get()->setNumMipmaps(8);
            rt0.get()->setFormat(::Ogre::PixelFormat::PF_FLOAT32_R);
            rt0.get()->setUsage(::Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
            rt0.get()->setTextureType(::Ogre::TextureType::TEX_TYPE_2D);

            rt0.get()->createInternalResources();

            // copy the content of the mip textures in the mipmap (rt0)
            // essayer de le faire de manière automatique avec une boucle + fonction qui récupèrerai l'ensemble des textures de MipMap dans un conteneur

            rt0.get()->getBuffer(0,0)->blit(mip0.get()->getBuffer());
            rt0.get()->getBuffer(0,1)->blit(mip1.get()->getBuffer());
            rt0.get()->getBuffer(0,2)->blit(mip2.get()->getBuffer());
            rt0.get()->getBuffer(0,3)->blit(mip3.get()->getBuffer());
            rt0.get()->getBuffer(0,4)->blit(mip4.get()->getBuffer());
            rt0.get()->getBuffer(0,5)->blit(mip5.get()->getBuffer());
            rt0.get()->getBuffer(0,6)->blit(mip6.get()->getBuffer());
            rt0.get()->getBuffer(0,7)->blit(mip7.get()->getBuffer());
            rt0.get()->getBuffer(0,8)->blit(mip8.get()->getBuffer());

            // change the sao arguments

//            std::cout << "still here it's ok " << std::endl;

//            std::cout << "radius " << getParent()->getSaoRadius() << std::endl;

            mat.get()->getTechnique(0)->getPass(0)->getFragmentProgramParameters().get()->setNamedConstant("u_radius",static_cast<float>(this->getParent()->getSaoRadius()));

            mat.get()->getTechnique(0)->getPass(0)->getFragmentProgramParameters().get()->setNamedConstant("u_numSamples",this->getParent()->getSaoSamples());



//            mip0 = Sao_compositor->getTextureInstance("mip0",0);
            const ::Ogre::Matrix4& proj = m_Parent->getSceneCamera()->getProjectionMatrix();

            const ::Ogre::Vector4 projInfo( float(-2.0 / (static_cast<float>(mip0.get()->getWidth() * proj[0][0]))),
                                             float(-2.0 / (static_cast<float>(mip0.get()->getWidth() * proj[1][1]))),
                                             float((1.0 - (double)proj[0][2]) / proj[0][0]),
                                             float((1.0 + (double)proj[1][2]) / proj[1][1])
                                            );
            mat.get()->getTechnique(0)->getPass(0)->getFragmentProgramParameters().get()->setNamedConstant("u_projInfo", projInfo);


//            std::cout << "no problem here ! " << std::endl;
        }

        if (pass_id  >= 41)
        {
            ::Ogre::CompositorChain* compChain =
                ::Ogre::CompositorManager::getSingletonPtr()->getCompositorChain(m_VP);

            ::Ogre::TexturePtr prevMip;
            ::Ogre::CompositorInstance * Sao_compositor = compChain->getCompositor("SAO_complet");
            prevMip = Sao_compositor->getTextureInstance("mip" + std::to_string(pass_id - 41), 0);

            mat.get()->getTechnique(0)->getPass(0)->getFragmentProgramParameters().get()->setNamedConstant("u_vpWidth", static_cast<float>(prevMip.get()->getWidth()));
           mat.get()->getTechnique(0)->getPass(0)->getFragmentProgramParameters().get()->setNamedConstant("u_vpHeight", static_cast<float>(prevMip.get()->getHeight()));
        }

        if (pass_id == 4)
        {
            // change the blend state
            mat.get()->getTechnique(0)->getPass(0)->getFragmentProgramParameters().get()->setNamedConstant("u_blend", this->getParent()->getBlend());
        }
    }

private:
    ::Ogre::Viewport* m_VP;

    SaoCompositorChainManager* m_Parent;

};


//-----------------------------------------------------------------------------



SaoCompositorChainManager::SaoCompositorChainManager(): m_ogreViewport(0),m_SaoRadius(0.85),m_SaoSamples(11),m_blend(false)
{
//    m_saoChain.push_back("Test");
//    m_saoChain.push_back("MipMap");
//    m_saoChain.push_back("AO_Samples");
//    m_saoChain.push_back("FinalChainCompositor");

    //test part

//    m_saoChain.push_back("Radial Blur");
//    m_saoChain.push_back("Bloom");


    // test with the only compositor doing all the work
    m_saoChain.push_back("SAO_complet");
//    m_saoChain.push_back("FinalChainCompositor");

}

SaoCompositorChainManager::SaoCompositorChainManager(::Ogre::Viewport* viewport): m_ogreViewport(viewport),m_SaoRadius(0.85),m_SaoSamples(11),m_blend(false)
{
    // create the chain
//    m_saoChain.push_back("Test");
//    m_saoChain.push_back("MipMap");
//    m_saoChain.push_back("AO_Samples");
//    m_saoChain.push_back("FinalChainCompositor");


    //test part

//    m_saoChain.push_back("Radial Blur");
//    m_saoChain.push_back("Bloom");

    // test with only one compositor
    m_saoChain.push_back("SAO_complet");
//    m_saoChain.push_back("FinalChainCompositor");

}



// This method can let the manager change the content of the original Ogre's compositor chain
::Ogre::CompositorManager* SaoCompositorChainManager::getCompositorManager()
{
    return ::Ogre::CompositorManager::getSingletonPtr();
}


// fonction that set the Sao Compositor
void SaoCompositorChainManager::setSaoState(bool state)
{
    std::cout << "try to change the state of the sao manager" << std::endl;


    // get the Compositor Manager of Ogre (of the scene)
    ::Ogre::CompositorManager* compositorManager = this->getCompositorManager();


    // if here, we add the compositor present of the chain in the Ogre chain compositor
    if (state)
    {
        // check the content of the Ogre Compositor chain

        ::Ogre::CompositorChain* compChain =
            ::Ogre::CompositorManager::getSingletonPtr()->getCompositorChain(m_ogreViewport);
        ::Ogre::CompositorChain::InstanceIterator compIter = compChain->getCompositors();

//        compChain->removeAllCompositors();

/* Init test but Don't work
 *
 *

        if (compChain->getCompositor("MipMap") == nullptr)
            compositorManager->addCompositor(m_ogreViewport,"MipMap");

        if (compChain->getCompositor("AO_Samples") == nullptr)
            compositorManager->addCompositor(m_ogreViewport,"AO_Samples");
//         Attach à listener to the AO_samples compositor
        compChain->getCompositor("AO_Samples")->addListener(new SaoListener(m_ogreViewport,this));

        */

/*
 * Test with previous removing compositor in the chain
 */


        /*

        if (compChain->getCompositor("MipMap") != nullptr)
        {
            compositorManager->removeCompositor(m_ogreViewport,"MipMap"); // suppression pour ajout à la fin de la chaine
        }

        if (compChain->getCompositor("AO_Samples") != nullptr)
        {
            compositorManager->removeCompositor(m_ogreViewport,"AO_Samples");
        }

            compositorManager->addCompositor(m_ogreViewport,"MipMap");
            compositorManager->addCompositor(m_ogreViewport,"AO_Samples");
//         Attach à listener to the AO_samples compositor
        compChain->getCompositor("AO_Samples")->addListener(new SaoListener(m_ogreViewport,this));

        */


        // FINAL VERSION (for the moment)


        // add final compositor but disable

        const std::string FINAL_CHAIN_COMPOSITOR = "FinalChainCompositor";
        bool needFinalCompositorSwap(false);
        // if not present in the chain add it and disable it
        if(!(compositorManager->getByName(FINAL_CHAIN_COMPOSITOR)).isNull())
        {
            compositorManager->setCompositorEnabled(m_ogreViewport, FINAL_CHAIN_COMPOSITOR, false);
            compositorManager->removeCompositor(m_ogreViewport, FINAL_CHAIN_COMPOSITOR);
            needFinalCompositorSwap = true;
        }


        if (compChain->getCompositor("SAO_complet") != nullptr)
        {
            compositorManager->removeCompositor(m_ogreViewport,"SAO_complet"); // suppression pour ajout à la fin de la chaine
        }

        compositorManager->addCompositor(m_ogreViewport,"SAO_complet");
        compChain->getCompositor("SAO_complet")->addListener(new SaoListener(m_ogreViewport,this));

        if (needFinalCompositorSwap)
        {
            compositorManager->addCompositor(m_ogreViewport,FINAL_CHAIN_COMPOSITOR);
            compositorManager->setCompositorEnabled(m_ogreViewport,FINAL_CHAIN_COMPOSITOR,true);
        }



//        compositorManager->addCompositor(m_ogreViewport,"FinalChainCompositor");


        // two basic compositors in the chain and see the result

//        if (compChain->getCompositor("Radial Blur") == nullptr)
//                    compositorManager->addCompositor(m_ogreViewport,"Radial Blur");

//        if (compChain->getCompositor("Bloom") == nullptr)
//                    compositorManager->addCompositor(m_ogreViewport,"Bloom");


        // test to see the content of the Compositor chain

        compIter = compChain->getCompositors();
        int index = 0;
        // désactivation des autres compositors -> uniquement pour les tests, surement non nécessaire
        while( compIter.hasMoreElements())
        {
            ::Ogre::CompositorInstance* targetComp = compIter.getNext();
            std::cout << "Compositor place : " << ++index << " " << targetComp->getCompositor()->getName() << std::endl;
//            if (targetComp->getEnabled())
//                targetComp->setEnabled(false);
//  //              std::cout << "      Enable" << std::endl;
        }


        compIter = compChain->getCompositors();


        // add all the compositor of the chain
        std::cout << "try to enable the sao chain" << std::endl;
        for(SaoCompositorIdType compositorName : m_saoChain)
        {
            std::cout << "compositor found in the sao chain" << std::endl;
            if(this->getCompositorManager()->resourceExists(compositorName))
            {
                std::cout << "Le compositor " << static_cast<std::string>(compositorName) << " existe et a correctement été chargé " << std::endl;
                //compositorManager->addCompositor(m_ogreViewport, compositorName);
                compositorManager->setCompositorEnabled(m_ogreViewport, compositorName, true);
            }
            else
                OSLM_WARN("\"" << compositorName << "\" does not refer to an existing compositor");

        }

    }
    else // disable the sao Chain
    {
        for(SaoCompositorIdType compositorName : m_saoChain)
        {
            if(this->getCompositorManager()->resourceExists(compositorName))
            {
                compositorManager->setCompositorEnabled(m_ogreViewport, compositorName, false);
            }
        }
    }
}

//-----------------------------------------------------------------------------

double SaoCompositorChainManager::getSaoRadius()
{
    return m_SaoRadius;
}

//-----------------------------------------------------------------------------


// fonction to change the sampling radius
void SaoCompositorChainManager::setSaoRadius(double newRadius)
{
    std::cout << "try to change the r value " << newRadius << std::endl;

    m_SaoRadius = newRadius;
}

//-----------------------------------------------------------------------------

int SaoCompositorChainManager::getSaoSamples()
{
    return m_SaoSamples;
}

//-----------------------------------------------------------------------------

// fonction to change the sampling radius
void SaoCompositorChainManager::setSaoSamples(int newSamplesNumber)
{
    m_SaoSamples = newSamplesNumber;
}

//-----------------------------------------------------------------------------

bool SaoCompositorChainManager::getBlend()
{
    return m_blend;
}

//-----------------------------------------------------------------------------

void SaoCompositorChainManager::enableBlend(bool state)
{
    m_blend = state;
}



}// namespace fwRenderOgre
