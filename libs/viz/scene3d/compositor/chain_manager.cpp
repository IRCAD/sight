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
#include "viz/scene3d/helper/shading.hpp"
#include "viz/scene3d/layer.hpp"
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

const chain_manager::compositor_id_t chain_manager::FINAL_CHAIN_COMPOSITOR = "FinalChainCompositor";

//-----------------------------------------------------------------------------

chain_manager::chain_manager(const viz::scene3d::layer::sptr& _layer) :
    m_layer(_layer)
{
}

//-----------------------------------------------------------------------------

chain_manager::~chain_manager()
{
    this->unregisterServices();
}

//-----------------------------------------------------------------------------

void chain_manager::addAvailableCompositor(compositor_id_t _compositor_name)
{
    const auto layer                            = m_layer.lock();
    Ogre::Viewport* viewport                    = layer->getViewport();
    Ogre::CompositorManager& compositor_manager = Ogre::CompositorManager::getSingleton();

    // Remove the final chain compositor if present
    if(compositor_manager.getByName(FINAL_CHAIN_COMPOSITOR, RESOURCE_GROUP))
    {
        compositor_manager.setCompositorEnabled(viewport, FINAL_CHAIN_COMPOSITOR, false);
        compositor_manager.removeCompositor(viewport, FINAL_CHAIN_COMPOSITOR);
    }

    // Add the new compositor
    m_compositorChain.emplace_back(_compositor_name, false);
    Ogre::CompositorInstance* compositor = compositor_manager.addCompositor(viewport, _compositor_name);

    // TODO: Handle this with a proper registration of the listener so that future extensions do not need to modify
    // anything here
    if(_compositor_name == "SAO")
    {
        compositor->addListener(new sao_listener(viewport));
    }

    this->addFinalCompositor();
}

//-----------------------------------------------------------------------------

void chain_manager::clearCompositorChain()
{
    const auto layer                            = m_layer.lock();
    Ogre::Viewport* viewport                    = layer->getViewport();
    Ogre::CompositorManager& compositor_manager = Ogre::CompositorManager::getSingleton();
    for(auto& chain : m_compositorChain)
    {
        compositor_manager.setCompositorEnabled(viewport, chain.first, false);
        compositor_manager.removeCompositor(viewport, chain.first);
    }

    m_compositorChain.clear();
}

//-----------------------------------------------------------------------------

void chain_manager::updateCompositorState(compositor_id_t _compositor_name, bool _is_enabled)
{
    const auto layer                            = m_layer.lock();
    Ogre::Viewport* viewport                    = layer->getViewport();
    Ogre::CompositorManager& compositor_manager = Ogre::CompositorManager::getSingleton();

    // If there isn't any compositor available, the update operation can't be done
    if(!m_compositorChain.empty())
    {
        auto compositor_to_update = std::find_if(
            m_compositorChain.begin(),
            m_compositorChain.end(),
            [_compositor_name](const compositor_t& _compositor)
            {
                return _compositor.first == _compositor_name;
            });

        if(compositor_to_update != m_compositorChain.end())
        {
            compositor_to_update->second = _is_enabled;
            compositor_manager.setCompositorEnabled(viewport, _compositor_name, _is_enabled);

            this->updateCompositorAdaptors(_compositor_name, _is_enabled);

            // Send a signal, i.e. to update editors in user interfaces
            auto render_service = layer->getRenderService();
            auto sig            = render_service->signal<render::signals::compositorUpdated_signal_t>(
                render::signals::COMPOSITOR_UPDATED
            );
            sig->async_emit(_compositor_name, _is_enabled, layer);
        }
    }
}

//-----------------------------------------------------------------------------

void chain_manager::setCompositorChain(const std::vector<compositor_id_t>& _compositors)
{
    this->clearCompositorChain();

    const auto layer                            = m_layer.lock();
    Ogre::Viewport* viewport                    = layer->getViewport();
    Ogre::CompositorManager& compositor_manager = Ogre::CompositorManager::getSingleton();

    // Remove the final chain compositor if present
    if(compositor_manager.getByName(FINAL_CHAIN_COMPOSITOR, RESOURCE_GROUP))
    {
        compositor_manager.setCompositorEnabled(viewport, FINAL_CHAIN_COMPOSITOR, false);
        compositor_manager.removeCompositor(viewport, FINAL_CHAIN_COMPOSITOR);
    }

    for(const compositor_id_t& compositor_name : _compositors)
    {
        if(compositor_manager.resourceExists(compositor_name, RESOURCE_GROUP))
        {
            m_compositorChain.emplace_back(compositor_name, true);
            compositor_manager.addCompositor(viewport, compositor_name);
            compositor_manager.setCompositorEnabled(viewport, compositor_name, true);

            this->updateCompositorAdaptors(compositor_name, true);

            // Send a signal, i.e. to update editors in user interfaces
            auto render_service = layer->getRenderService();
            auto sig            = render_service->signal<render::signals::compositorUpdated_signal_t>(
                render::signals::COMPOSITOR_UPDATED
            );
            sig->async_emit(compositor_name, true, layer);
        }
        else
        {
            SIGHT_ERROR("'" + compositor_name + "' does not refer to an existing compositor");
        }
    }

    this->addFinalCompositor();
}

//-----------------------------------------------------------------------------

void chain_manager::addFinalCompositor()
{
    const auto layer                            = m_layer.lock();
    Ogre::Viewport* viewport                    = layer->getViewport();
    Ogre::CompositorManager& compositor_manager = Ogre::CompositorManager::getSingleton();
    compositor_manager.addCompositor(viewport, FINAL_CHAIN_COMPOSITOR);
    compositor_manager.setCompositorEnabled(viewport, FINAL_CHAIN_COMPOSITOR, true);
}

//-----------------------------------------------------------------------------

void chain_manager::updateCompositorAdaptors(compositor_id_t _compositor_name, bool _is_enabled)
{
    const auto layer                            = m_layer.lock();
    Ogre::Viewport* viewport                    = layer->getViewport();
    Ogre::CompositorManager& compositor_manager = Ogre::CompositorManager::getSingleton();
    Ogre::CompositorChain* comp_chain           = compositor_manager.getCompositorChain(viewport);
    Ogre::CompositorInstance* compositor        = comp_chain->getCompositor(_compositor_name);
    SIGHT_ASSERT("The given compositor '" + _compositor_name + "' doesn't exist in the compositor chain", compositor);

    Ogre::CompositionTechnique* tech = compositor->getTechnique();

    // Collect target passes
    std::vector<Ogre::CompositionTargetPass*> target_passes = tech->getTargetPasses();
    target_passes.push_back(tech->getOutputTargetPass());

    for(auto* const target_pass : target_passes)
    {
        const auto& passes = target_pass->getPasses();

        for(auto* const pass : passes)
        {
            // We retrieve the parameters of the base material in a temporary material
            const Ogre::MaterialPtr material = pass->getMaterial();

            if(material)
            {
                const auto constants = viz::scene3d::helper::shading::findMaterialConstants(*material);
                for(const auto& constant : constants)
                {
                    const std::string& constant_name = std::get<0>(constant);

                    // Skip constant that start with "eu_". They are not supposed to be set by the user.
                    const std::regex eu("eu.*");
                    if(std::regex_match(constant_name, eu))
                    {
                        continue;
                    }

                    const auto shader_type = std::get<2>(constant);

                    const std::string shader_type_str = shader_type == Ogre::GPT_VERTEX_PROGRAM ? "vertex"
                                                                                                : shader_type
                                                        == Ogre::GPT_FRAGMENT_PROGRAM ? "fragment"
                                                                                      :
                                                        "geometry";

                    // Naming convention for shader parameters
                    auto render_service = layer->getRenderService();
                    const auto id       = render_service->get_id() + layer->getLayerID() + "_"
                                          + shader_type_str
                                          + "-" + constant_name;

                    if(_is_enabled && this->getRegisteredService(id) == nullptr)
                    {
                        const auto& constant_type  = std::get<1>(constant);
                        const auto& constant_value = std::get<3>(constant);

                        auto obj = viz::scene3d::helper::shading::createObjectFromShaderParameter(
                            constant_type,
                            constant_value
                        );

                        if(obj != nullptr)
                        {
                            // Creates an Ogre adaptor and associates it with the Sight object
                            auto srv = this->registerService(
                                "sight::module::viz::scene3d::adaptor::compositor_parameter",
                                id
                            );
                            srv->set_inout(obj, "parameter", true);

                            auto shader_param_service = std::dynamic_pointer_cast<viz::scene3d::adaptor>(srv);
                            shader_param_service->setRenderService(render_service);

                            service::config_t config;
                            config.add("config.<xmlattr>.compositorName", _compositor_name);
                            config.add("config.<xmlattr>.parameter", constant_name);
                            config.add("config.<xmlattr>.shaderType", shader_type_str);

                            shader_param_service->setLayerID(layer->getLayerID());
                            shader_param_service->set_config(config);
                            shader_param_service->configure();
                            shader_param_service->start();

                            m_adaptorsObjectsOwner.insert_or_assign(constant_name, obj);
                        }
                    }
                    else
                    {
                        this->unregisterService(id);
                        m_adaptorsObjectsOwner.erase(constant_name);
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::compositor
