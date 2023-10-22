/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "viz/scene3d/adaptor.hpp"

#include <core/com/slots.hxx>

#include <service/macros.hpp>
#include <service/registry.hpp>

#include <viz/scene3d/registry/adaptor.hpp>
#include <viz/scene3d/utils.hpp>

namespace sight::viz::scene3d
{

const core::com::slots::key_t adaptor::UPDATE_VISIBILITY_SLOT = "updateVisibility";
const core::com::slots::key_t adaptor::TOGGLE_VISIBILITY_SLOT = "toggleVisibility";
const core::com::slots::key_t adaptor::SHOW_SLOT              = "show";
const core::com::slots::key_t adaptor::HIDE_SLOT              = "hide";

const std::string adaptor::s_CONFIG = "config.<xmlattr>.";

//------------------------------------------------------------------------------

adaptor::adaptor() noexcept
{
    new_slot(UPDATE_VISIBILITY_SLOT, &adaptor::updateVisibility, this);
    new_slot(TOGGLE_VISIBILITY_SLOT, &adaptor::toggleVisibility, this);
    new_slot(SHOW_SLOT, &adaptor::show, this);
    new_slot(HIDE_SLOT, &adaptor::hide, this);
}

//------------------------------------------------------------------------------

void adaptor::info(std::ostream& _sstream)
{
    _sstream << "adaptor : " << this->get_id();
    this->service::base::info(_sstream);
}

//------------------------------------------------------------------------------

void adaptor::configureParams()
{
    const config_t config = this->get_config();
    m_cfgLayerID = config.get<std::string>("config.<xmlattr>.layer", "");
    m_isVisible  = config.get<bool>("config.<xmlattr>.visible", m_isVisible);

    SIGHT_WARN_IF(
        "In [" + this->get_id() + "] adaptor, specifying a layer is now deprecated. "
                                  "Please place the adaptor in the layer tag in the render scene configuration.",
        !m_cfgLayerID.empty()
    );
}

//------------------------------------------------------------------------------

void adaptor::initialize()
{
    if(m_renderService.expired())
    {
        auto services_vector = sight::service::get_services("sight::viz::scene3d::render");

        auto& registry = viz::scene3d::registry::get_adaptor_registry();
        auto layer_cfg = registry[this->get_id()];

        auto result =
            std::find_if(
                services_vector.begin(),
                services_vector.end(),
                [layer_cfg](const service::base::sptr& _srv)
            {
                return _srv->get_id() == layer_cfg.render;
            });
        SIGHT_ASSERT("Can't find '" + layer_cfg.render + "' render service.", result != services_vector.end());

        m_renderService = std::dynamic_pointer_cast<viz::scene3d::render>(*result);

        m_layerID = layer_cfg.layer.empty() ? m_cfgLayerID : layer_cfg.layer;
    }
}

//------------------------------------------------------------------------------

void adaptor::setLayerID(const std::string& _id)
{
    m_layerID = _id;
}

//------------------------------------------------------------------------------

const std::string& adaptor::getLayerID() const
{
    return m_layerID;
}

//------------------------------------------------------------------------------

void adaptor::setRenderService(render::sptr _service)
{
    SIGHT_ASSERT("service not instanced", _service);
    SIGHT_ASSERT("The adaptor ('" + this->get_id() + "') is not stopped", this->stopped());

    m_renderService = _service;
}

//------------------------------------------------------------------------------

render::sptr adaptor::getRenderService() const
{
    return m_renderService.lock();
}

//------------------------------------------------------------------------------

layer::sptr adaptor::getLayer() const
{
    return this->getRenderService()->getLayer(m_layerID);
}

//------------------------------------------------------------------------------

Ogre::SceneManager* adaptor::getSceneManager()
{
    return m_renderService.lock()->getSceneManager(m_layerID);
}

//------------------------------------------------------------------------------

void adaptor::requestRender()
{
    auto render_service = this->getRenderService();
    if((render_service->status() == service::base::STARTED
        || render_service->status() == service::base::SWAPPING)
       && render_service->getRenderMode() == viz::scene3d::render::RenderMode::AUTO)
    {
        render_service->requestRender();
    }
}

//-----------------------------------------------------------------------------

void adaptor::updateVisibility(bool _is_visible)
{
    m_isVisible = _is_visible;
    this->setVisible(m_isVisible);
}

//------------------------------------------------------------------------------

void adaptor::toggleVisibility()
{
    this->updateVisibility(!m_isVisible);
}

//------------------------------------------------------------------------------

void adaptor::show()
{
    this->updateVisibility(true);
}

//------------------------------------------------------------------------------

void adaptor::hide()
{
    this->updateVisibility(false);
}

//------------------------------------------------------------------------------

void adaptor::setVisible(bool /*unused*/)
{
    SIGHT_WARN("This adaptor has no method 'setVisible(bool)', it needs to be overridden to be called.");
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d.
