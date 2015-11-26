#include "fwRenderOgre/compositor/SaoCompositorChainManager.hpp"

#include <fwCore/spyLog.hpp>

#include <fwCore/spyLog.hpp>

namespace fwRenderOgre {

//-----------------------------------------------------------------------------

SaoCompositorChainManager::SaoCompositorChainManager(): m_ogreViewport(0),m_SaoRadius(0.85),m_SaoSamples(11)
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
    m_saoChain.push_back("FinalChainCompositor");

}

SaoCompositorChainManager::SaoCompositorChainManager(::Ogre::Viewport* viewport): m_ogreViewport(viewport),m_SaoRadius(0.85),m_SaoSamples(11)
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
    m_saoChain.push_back("FinalChainCompositor");

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

        // one compositor


        if (compChain->getCompositor("SAO_complet") != nullptr)
        {
            compositorManager->removeCompositor(m_ogreViewport,"SAO_complet"); // suppression pour ajout à la fin de la chaine
        }

        compositorManager->addCompositor(m_ogreViewport,"SAO_complet");
        compChain->getCompositor("SAO_complet")->addListener(new SaoListener(m_ogreViewport,this));

        // add the Final in the chain if not present
        if (compChain->getCompositor("FinalChainCompositor") == nullptr)
        {
            compositorManager->addCompositor(m_ogreViewport,"FinalChainCompositor"); // suppression pour ajout à la fin de la chaine
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


double SaoCompositorChainManager::getSaoRadius()
{
    return m_SaoRadius;
}

// fonction to change the sampling radius
void SaoCompositorChainManager::setSaoRadius(double newRadius)
{
    std::cout << "try to change the r value " << newRadius << std::endl;

    m_SaoRadius = newRadius;
}

int SaoCompositorChainManager::getSaoSamples()
{
    return m_SaoSamples;
}

// fonction to change the sampling radius
void SaoCompositorChainManager::setSaoSamples(int newSamplesNumber)
{
    m_SaoSamples = newSamplesNumber;
}



}// namespace fwRenderOgre
