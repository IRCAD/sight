/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "fwRenderOgre/compositor/ChainManager.hpp"

#include "fwRenderOgre/compositor/listener/AutoStereo.hpp"
#include "fwRenderOgre/compositor/SaoListener.hpp"
#include "fwRenderOgre/helper/Shading.hpp"
#include "fwRenderOgre/IAdaptor.hpp"
#include "fwRenderOgre/Layer.hpp"
#include "fwRenderOgre/SRender.hpp"

#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreCompositionPass.h>
#include <OGRE/OgreCompositionTargetPass.h>
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorManager.h>

#include <regex>

namespace fwRenderOgre
{

namespace compositor
{

//-----------------------------------------------------------------------------

const ChainManager::CompositorIdType ChainManager::FINAL_CHAIN_COMPOSITOR = "FinalChainCompositor";

//-----------------------------------------------------------------------------

ChainManager::ChainManager(::Ogre::Viewport* _viewport) :
    m_ogreViewport(_viewport)
{
    m_adaptorsObjectsOwner = ::fwData::Composite::New();
}

//-----------------------------------------------------------------------------

ChainManager::~ChainManager()
{
    this->unregisterServices();
}

//-----------------------------------------------------------------------------

void ChainManager::addAvailableCompositor(CompositorIdType _compositorName)
{
    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();

    // Remove the final chain compositor if present
    if(compositorManager.getByName(FINAL_CHAIN_COMPOSITOR))
    {
        compositorManager.setCompositorEnabled(m_ogreViewport, FINAL_CHAIN_COMPOSITOR, false);
        compositorManager.removeCompositor(m_ogreViewport, FINAL_CHAIN_COMPOSITOR);
    }

    // Add the new compositor
    m_compositorChain.push_back(CompositorType(_compositorName, false));
    ::Ogre::CompositorInstance* compositor = compositorManager.addCompositor(m_ogreViewport, _compositorName);

    // TODO: Handle this with a proper registration of the listener so that future extensions do not need to modify
    // anything here
    if (_compositorName == "SAO" )
    {
        compositor->addListener(new SaoListener(m_ogreViewport));
    }

    this->addFinalCompositor();
}

//-----------------------------------------------------------------------------

void ChainManager::clearCompositorChain(const std::string& /*_layerId*/,
                                        ::fwRenderOgre::SRender::sptr /*_renderService*/)
{
    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    for(auto& chain : m_compositorChain)
    {
        compositorManager.setCompositorEnabled(m_ogreViewport, chain.first, false);
        compositorManager.removeCompositor(m_ogreViewport, chain.first);
    }

    m_compositorChain.clear();
}

//-----------------------------------------------------------------------------

void ChainManager::updateCompositorState(CompositorIdType _compositorName, bool _isEnabled,
                                         const std::string& _layerId, ::fwRenderOgre::SRender::sptr _renderService)
{
    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();

    // If there isn't any compositor available, the update operation can't be done
    if(!m_compositorChain.empty())
    {
        auto compositorToUpdate = std::find_if(m_compositorChain.begin(),
                                               m_compositorChain.end(),
                                               FindCompositorByName(_compositorName));

        if(compositorToUpdate != m_compositorChain.end())
        {
            compositorToUpdate->second = _isEnabled;
            compositorManager.setCompositorEnabled(m_ogreViewport, _compositorName, _isEnabled);

            this->updateCompositorAdaptors(_compositorName, _isEnabled, _layerId, _renderService);

            // Send a signal, i.e. to update editors in user interfaces
            auto sig = _renderService->signal<SRender::CompositorUpdatedSignalType>(SRender::s_COMPOSITOR_UPDATED_SIG);
            sig->asyncEmit(_compositorName, _isEnabled, _renderService->getLayer(_layerId));
        }
    }
}

//-----------------------------------------------------------------------------

void ChainManager::setCompositorChain(const std::vector<CompositorIdType>& _compositors,
                                      const std::string& _layerId, ::fwRenderOgre::SRender::sptr _renderService)
{
    this->clearCompositorChain(_layerId, _renderService);

    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();

    // Remove the final chain compositor if present
    if(compositorManager.getByName(FINAL_CHAIN_COMPOSITOR))
    {
        compositorManager.setCompositorEnabled(m_ogreViewport, FINAL_CHAIN_COMPOSITOR, false);
        compositorManager.removeCompositor(m_ogreViewport, FINAL_CHAIN_COMPOSITOR);
    }

    for(const CompositorIdType& compositorName : _compositors)
    {
        if(compositorManager.resourceExists(compositorName))
        {
            m_compositorChain.push_back(CompositorType(compositorName, true));
            compositorManager.addCompositor(m_ogreViewport, compositorName);
            compositorManager.setCompositorEnabled(m_ogreViewport, compositorName, true);

            this->updateCompositorAdaptors(compositorName, true, _layerId, _renderService);

            // Send a signal, i.e. to update editors in user interfaces
            auto sig = _renderService->signal<SRender::CompositorUpdatedSignalType>(SRender::s_COMPOSITOR_UPDATED_SIG);
            sig->asyncEmit(compositorName, true, _renderService->getLayer(_layerId));
        }
        else
        {
            SLM_ERROR("'" + compositorName + "' does not refer to an existing compositor");
        }
    }

    this->addFinalCompositor();
}

//-----------------------------------------------------------------------------

void ChainManager::addFinalCompositor()
{
    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    compositorManager.addCompositor(m_ogreViewport, FINAL_CHAIN_COMPOSITOR);
    compositorManager.setCompositorEnabled(m_ogreViewport, FINAL_CHAIN_COMPOSITOR, true);
}

//-----------------------------------------------------------------------------

void ChainManager::updateCompositorAdaptors(CompositorIdType _compositorName, bool _isEnabled,
                                            const std::string& _layerId, ::fwRenderOgre::SRender::sptr _renderService)
{
    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    ::Ogre::CompositorChain* compChain           = compositorManager.getCompositorChain(m_ogreViewport);
    ::Ogre::CompositorInstance* compositor       = compChain->getCompositor(_compositorName);
    SLM_ASSERT("The given compositor '" + _compositorName + "' doesn't exist in the compositor chain", compositor);

    ::Ogre::CompositionTechnique* tech = compositor->getTechnique();

    // Collect target passes
    std::vector< ::Ogre::CompositionTargetPass*> targetPasses = tech->getTargetPasses();
    targetPasses.push_back(tech->getOutputTargetPass());

    for(const auto targetPass : targetPasses)
    {
        const auto& passes = targetPass->getPasses();

        for(const auto targetPass : passes)
        {
            // We retrieve the parameters of the base material in a temporary material
            const ::Ogre::MaterialPtr material = targetPass->getMaterial();

            if(material)
            {
                const auto constants = ::fwRenderOgre::helper::Shading::findMaterialConstants(*material);
                for(const auto& constant : constants)
                {
                    const std::string& constantName = std::get<0>(constant);

                    // Skip constant that start with "eu_". They are not supposed to be set by the user.
                    const std::regex eu("eu.*");
                    if(std::regex_match(constantName, eu))
                    {
                        continue;
                    }

                    const auto shaderType = std::get<2>(constant);

                    const std::string shaderTypeStr = shaderType == ::Ogre::GPT_VERTEX_PROGRAM ? "vertex" :
                                                      shaderType == ::Ogre::GPT_FRAGMENT_PROGRAM ? "fragment" :
                                                      "geometry";

                    // Naming convention for shader parameters
                    const ::fwTools::fwID::IDType id = _renderService->getID() + _layerId + "_" + shaderTypeStr + "-" +
                                                       constantName;

                    if(_isEnabled && this->getRegisteredService(id) == nullptr)
                    {
                        const auto& constantType  = std::get<1>(constant);
                        const auto& constantValue = std::get<3>(constant);

                        auto obj = ::fwRenderOgre::helper::Shading::createObjectFromShaderParameter(constantType,
                                                                                                    constantValue);

                        if(obj != nullptr)
                        {
                            // Creates an Ogre adaptor and associates it with the Sight object
                            auto srv = this->registerService("::visuOgreAdaptor::SCompositorParameter", id);
                            srv->registerInOut(obj, "parameter", true);

                            auto shaderParamService = ::fwRenderOgre::IAdaptor::dynamicCast(srv);
                            shaderParamService->setRenderService(_renderService);

                            ::fwServices::IService::ConfigType config;
                            config.add("config.<xmlattr>.layer", _layerId);
                            config.add("config.<xmlattr>.compositorName", _compositorName);
                            config.add("config.<xmlattr>.parameter", constantName);
                            config.add("config.<xmlattr>.shaderType", shaderTypeStr);

                            shaderParamService->setConfiguration(config);
                            shaderParamService->configure();
                            shaderParamService->start();

                            (*m_adaptorsObjectsOwner)[constantName] = obj;
                        }
                    }
                    else
                    {
                        this->unregisterService(id);
                        if(m_adaptorsObjectsOwner->at< ::fwData::Object>(constantName) != nullptr)
                        {
                            m_adaptorsObjectsOwner->getContainer().erase(constantName);
                        }
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace compositor

} // namespace fwRenderOgre
