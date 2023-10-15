/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "viz/scene3d/compositor/chain_manager.hpp"

#include "viz/scene3d/adaptor.hpp"
#include "viz/scene3d/compositor/listener/auto_stereo.hpp"
#include "viz/scene3d/compositor/sao_listener.hpp"
#include "viz/scene3d/helper/Shading.hpp"
#include "viz/scene3d/Layer.hpp"
#include "viz/scene3d/render.hpp"

#include <service/op.hpp>
#include <service/registry.hpp>

#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreCompositionPass.h>
#include <OGRE/OgreCompositionTargetPass.h>
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorManager.h>

#include <regex>

namespace sight::viz::scene3d::compositor
{

//-----------------------------------------------------------------------------

const chain_manager::CompositorIdType chain_manager::FINAL_CHAIN_COMPOSITOR = "FinalChainCompositor";

//-----------------------------------------------------------------------------

chain_manager::chain_manager(const viz::scene3d::Layer::sptr& _layer) :
    m_layer(_layer)
{
}

//-----------------------------------------------------------------------------

chain_manager::~chain_manager()
{
    this->unregisterServices();
}

//-----------------------------------------------------------------------------

void chain_manager::addAvailableCompositor(CompositorIdType _compositorName)
{
    const auto layer                           = m_layer.lock();
    Ogre::Viewport* viewport                   = layer->getViewport();
    Ogre::CompositorManager& compositorManager = Ogre::CompositorManager::getSingleton();

    // Remove the final chain compositor if present
    if(compositorManager.getByName(FINAL_CHAIN_COMPOSITOR, RESOURCE_GROUP))
    {
        compositorManager.setCompositorEnabled(viewport, FINAL_CHAIN_COMPOSITOR, false);
        compositorManager.removeCompositor(viewport, FINAL_CHAIN_COMPOSITOR);
    }

    // Add the new compositor
    m_compositorChain.emplace_back(_compositorName, false);
    Ogre::CompositorInstance* compositor = compositorManager.addCompositor(viewport, _compositorName);

    // TODO: Handle this with a proper registration of the listener so that future extensions do not need to modify
    // anything here
    if(_compositorName == "SAO")
    {
        compositor->addListener(new sao_listener(viewport));
    }

    this->addFinalCompositor();
}

//-----------------------------------------------------------------------------

void chain_manager::clearCompositorChain()
{
    const auto layer                           = m_layer.lock();
    Ogre::Viewport* viewport                   = layer->getViewport();
    Ogre::CompositorManager& compositorManager = Ogre::CompositorManager::getSingleton();
    for(auto& chain : m_compositorChain)
    {
        compositorManager.setCompositorEnabled(viewport, chain.first, false);
        compositorManager.removeCompositor(viewport, chain.first);
    }

    m_compositorChain.clear();
}

//-----------------------------------------------------------------------------

void chain_manager::updateCompositorState(CompositorIdType _compositorName, bool _isEnabled)
{
    const auto layer                           = m_layer.lock();
    Ogre::Viewport* viewport                   = layer->getViewport();
    Ogre::CompositorManager& compositorManager = Ogre::CompositorManager::getSingleton();

    // If there isn't any compositor available, the update operation can't be done
    if(!m_compositorChain.empty())
    {
        auto compositorToUpdate = std::find_if(
            m_compositorChain.begin(),
            m_compositorChain.end(),
            [_compositorName](const CompositorType& _compositor)
            {
                return _compositor.first == _compositorName;
            });

        if(compositorToUpdate != m_compositorChain.end())
        {
            compositorToUpdate->second = _isEnabled;
            compositorManager.setCompositorEnabled(viewport, _compositorName, _isEnabled);

            this->updateCompositorAdaptors(_compositorName, _isEnabled);

            // Send a signal, i.e. to update editors in user interfaces
            auto renderService = layer->getRenderService();
            auto sig           = renderService->signal<render::signals::compositorUpdated_signal_t>(
                render::signals::COMPOSITOR_UPDATED
            );
            sig->async_emit(_compositorName, _isEnabled, layer);
        }
    }
}

//-----------------------------------------------------------------------------

void chain_manager::setCompositorChain(const std::vector<CompositorIdType>& _compositors)
{
    this->clearCompositorChain();

    const auto layer                           = m_layer.lock();
    Ogre::Viewport* viewport                   = layer->getViewport();
    Ogre::CompositorManager& compositorManager = Ogre::CompositorManager::getSingleton();

    // Remove the final chain compositor if present
    if(compositorManager.getByName(FINAL_CHAIN_COMPOSITOR, RESOURCE_GROUP))
    {
        compositorManager.setCompositorEnabled(viewport, FINAL_CHAIN_COMPOSITOR, false);
        compositorManager.removeCompositor(viewport, FINAL_CHAIN_COMPOSITOR);
    }

    for(const CompositorIdType& compositorName : _compositors)
    {
        if(compositorManager.resourceExists(compositorName, RESOURCE_GROUP))
        {
            m_compositorChain.emplace_back(compositorName, true);
            compositorManager.addCompositor(viewport, compositorName);
            compositorManager.setCompositorEnabled(viewport, compositorName, true);

            this->updateCompositorAdaptors(compositorName, true);

            // Send a signal, i.e. to update editors in user interfaces
            auto renderService = layer->getRenderService();
            auto sig           = renderService->signal<render::signals::compositorUpdated_signal_t>(
                render::signals::COMPOSITOR_UPDATED
            );
            sig->async_emit(compositorName, true, layer);
        }
        else
        {
            SIGHT_ERROR("'" + compositorName + "' does not refer to an existing compositor");
        }
    }

    this->addFinalCompositor();
}

//-----------------------------------------------------------------------------

void chain_manager::addFinalCompositor()
{
    const auto layer                           = m_layer.lock();
    Ogre::Viewport* viewport                   = layer->getViewport();
    Ogre::CompositorManager& compositorManager = Ogre::CompositorManager::getSingleton();
    compositorManager.addCompositor(viewport, FINAL_CHAIN_COMPOSITOR);
    compositorManager.setCompositorEnabled(viewport, FINAL_CHAIN_COMPOSITOR, true);
}

//-----------------------------------------------------------------------------

void chain_manager::updateCompositorAdaptors(CompositorIdType _compositorName, bool _isEnabled)
{
    const auto layer                           = m_layer.lock();
    Ogre::Viewport* viewport                   = layer->getViewport();
    Ogre::CompositorManager& compositorManager = Ogre::CompositorManager::getSingleton();
    Ogre::CompositorChain* compChain           = compositorManager.getCompositorChain(viewport);
    Ogre::CompositorInstance* compositor       = compChain->getCompositor(_compositorName);
    SIGHT_ASSERT("The given compositor '" + _compositorName + "' doesn't exist in the compositor chain", compositor);

    Ogre::CompositionTechnique* tech = compositor->getTechnique();

    // Collect target passes
    std::vector<Ogre::CompositionTargetPass*> targetPasses = tech->getTargetPasses();
    targetPasses.push_back(tech->getOutputTargetPass());

    for(auto* const targetPass : targetPasses)
    {
        const auto& passes = targetPass->getPasses();

        for(auto* const pass : passes)
        {
            // We retrieve the parameters of the base material in a temporary material
            const Ogre::MaterialPtr material = pass->getMaterial();

            if(material)
            {
                const auto constants = viz::scene3d::helper::Shading::findMaterialConstants(*material);
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

                    const std::string shaderTypeStr = shaderType == Ogre::GPT_VERTEX_PROGRAM ? "vertex"
                                                                                             : shaderType
                                                      == Ogre::GPT_FRAGMENT_PROGRAM ? "fragment"
                                                                                    :
                                                      "geometry";

                    // Naming convention for shader parameters
                    auto renderService = layer->getRenderService();
                    const auto id      = renderService->get_id() + layer->getLayerID() + "_"
                                         + shaderTypeStr
                                         + "-" + constantName;

                    if(_isEnabled && this->getRegisteredService(id) == nullptr)
                    {
                        const auto& constantType  = std::get<1>(constant);
                        const auto& constantValue = std::get<3>(constant);

                        auto obj = viz::scene3d::helper::Shading::createObjectFromShaderParameter(
                            constantType,
                            constantValue
                        );

                        if(obj != nullptr)
                        {
                            // Creates an Ogre adaptor and associates it with the Sight object
                            auto srv = this->registerService(
                                "sight::module::viz::scene3d::adaptor::compositor_parameter",
                                id
                            );
                            srv->set_inout(obj, "parameter", true);

                            auto shaderParamService = std::dynamic_pointer_cast<viz::scene3d::adaptor>(srv);
                            shaderParamService->setRenderService(renderService);

                            service::config_t config;
                            config.add("config.<xmlattr>.compositorName", _compositorName);
                            config.add("config.<xmlattr>.parameter", constantName);
                            config.add("config.<xmlattr>.shaderType", shaderTypeStr);

                            shaderParamService->setLayerID(layer->getLayerID());
                            shaderParamService->set_config(config);
                            shaderParamService->configure();
                            shaderParamService->start();

                            m_adaptorsObjectsOwner.insert_or_assign(constantName, obj);
                        }
                    }
                    else
                    {
                        this->unregisterService(id);
                        m_adaptorsObjectsOwner.erase(constantName);
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::compositor
