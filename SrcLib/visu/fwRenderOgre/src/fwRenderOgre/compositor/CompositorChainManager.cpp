/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/compositor/CompositorChainManager.hpp"

#include <fwCore/spyLog.hpp>

#include <fwCore/spyLog.hpp>

#include <OGRE/OgreCompositorManager.h>

namespace fwRenderOgre
{

//-----------------------------------------------------------------------------

const CompositorChainManager::CompositorIdType CompositorChainManager::FINAL_CHAIN_COMPOSITOR = "FinalChainCompositor";

//-----------------------------------------------------------------------------

CompositorChainManager::CompositorChainManager()
    : m_ogreViewport(0)
{
}

//-----------------------------------------------------------------------------

CompositorChainManager::CompositorChainManager(::Ogre::Viewport* ogreViewport)
    : m_ogreViewport(ogreViewport)
{
}

//-----------------------------------------------------------------------------

void CompositorChainManager::addAvailableCompositor(CompositorIdType compositorName)
{
    ::Ogre::CompositorManager* compositorManager = this->getCompositorManager();
    bool needFinalCompositorSwap(false);

    // If the compositor chain already contains the final compositor, we have to remove it
    if(!m_compositorChain.empty() && !(compositorManager->getByName(FINAL_CHAIN_COMPOSITOR)).isNull())
    {
        m_compositorChain.pop_back();
        compositorManager->setCompositorEnabled(m_ogreViewport, FINAL_CHAIN_COMPOSITOR, false);
        compositorManager->removeCompositor(m_ogreViewport, FINAL_CHAIN_COMPOSITOR);
        needFinalCompositorSwap = true;
    }

    // Now, we can add the new compositor to the compositor chain
    m_compositorChain.push_back(CompositorType(compositorName, false));
    compositorManager->addCompositor(m_ogreViewport, compositorName);

    // If the final compositor has been removed, we need to add it to the compositor chain
    if(needFinalCompositorSwap)
    {
        this->addFinalCompositor();
    }
}

//-----------------------------------------------------------------------------

void CompositorChainManager::clearCompositorChain()
{
    m_compositorChain.clear();
    this->getCompositorManager()->removeCompositorChain(m_ogreViewport);
}

//-----------------------------------------------------------------------------

void CompositorChainManager::updateCompositorState(CompositorIdType compositorName, bool isEnabled)
{
    // If there isn't any compositor available, the update operation can't be done
    if(!m_compositorChain.empty())
    {
        auto compositorToUpdate = std::find_if(m_compositorChain.begin(),
                                               m_compositorChain.end(),
                                               FindCompositorByName(compositorName));

        if(compositorToUpdate != m_compositorChain.end())
        {
            compositorToUpdate->second = isEnabled;
            this->getCompositorManager()->setCompositorEnabled(m_ogreViewport, compositorName, isEnabled);
        }
    }

    // check the content of the Ogre Compositor chain

    ::Ogre::CompositorChain* compChain =
        ::Ogre::CompositorManager::getSingletonPtr()->getCompositorChain(m_ogreViewport);
    ::Ogre::CompositorChain::InstanceIterator compIter = compChain->getCompositors();

    std::cout << "Liste des compositors actuellement dans la chaine " << std::endl;

    while( compIter.hasMoreElements())
    {
        ::Ogre::CompositorInstance* targetComp = compIter.getNext();
        std::cout << targetComp->getCompositor()->getName() << std::endl;
        if (targetComp->getEnabled())
            std::cout << "      Enable" << std::endl;
    }

    compIter = compChain->getCompositors();

    compIter = compChain->getCompositors();
    int index = 0;

    while( compIter.hasMoreElements())
    {
        ::Ogre::CompositorInstance* targetComp = compIter.getNext();
        std::cout << "Compositor place : " << ++index << " " << targetComp->getCompositor()->getName() << std::endl;
//            if (targetComp->getEnabled())
//                targetComp->setEnabled(false);
//  //              std::cout << "      Enable" << std::endl;
    }


}

//-----------------------------------------------------------------------------

void CompositorChainManager::setCompositorChain(std::vector<CompositorIdType> compositors)
{
    ::Ogre::CompositorManager* compositorManager = this->getCompositorManager();

    for(CompositorIdType compositorName : compositors)
    {
        if(this->getCompositorManager()->resourceExists(compositorName))
        {
            std::cout << "Le compositor " << static_cast<std::string>(compositorName) << "existe et a correctement été chargé " << std::endl;
            m_compositorChain.push_back(CompositorType(compositorName, true));
            compositorManager->addCompositor(m_ogreViewport, compositorName);
            compositorManager->setCompositorEnabled(m_ogreViewport, compositorName, true);
        }
        else
        {
            OSLM_WARN("\"" << compositorName << "\" does not refer to an existing compositor");
        }
    }

    this->addFinalCompositor();
}

//-----------------------------------------------------------------------------

void CompositorChainManager::addFinalCompositor()
{
    m_compositorChain.push_back(CompositorType(FINAL_CHAIN_COMPOSITOR, true));
    this->getCompositorManager()->addCompositor(m_ogreViewport, FINAL_CHAIN_COMPOSITOR);
    this->getCompositorManager()->setCompositorEnabled(m_ogreViewport, FINAL_CHAIN_COMPOSITOR, true);
}

//-----------------------------------------------------------------------------

::Ogre::CompositorManager* CompositorChainManager::getCompositorManager()
{
    return ::Ogre::CompositorManager::getSingletonPtr();
}

//-----------------------------------------------------------------------------

} // namespace fwRenderOgre
