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

#include "viz/scene3d/IAdaptor.hpp"

#include <core/com/Slots.hxx>

#include <service/helper/Config.hpp>
#include <service/macros.hpp>
#include <service/registry.hpp>

#include <viz/scene3d/registry/Adaptor.hpp>
#include <viz/scene3d/Utils.hpp>

namespace sight::viz::scene3d
{

const core::com::Slots::SlotKeyType IAdaptor::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";
const core::com::Slots::SlotKeyType IAdaptor::s_TOGGLE_VISIBILITY_SLOT = "toggleVisibility";
const core::com::Slots::SlotKeyType IAdaptor::s_SHOW_SLOT              = "show";
const core::com::Slots::SlotKeyType IAdaptor::s_HIDE_SLOT              = "hide";

const std::string IAdaptor::s_CONFIG = "config.<xmlattr>.";

//------------------------------------------------------------------------------

IAdaptor::IAdaptor() noexcept
{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &IAdaptor::updateVisibility, this);
    newSlot(s_TOGGLE_VISIBILITY_SLOT, &IAdaptor::toggleVisibility, this);
    newSlot(s_SHOW_SLOT, &IAdaptor::show, this);
    newSlot(s_HIDE_SLOT, &IAdaptor::hide, this);
}

//------------------------------------------------------------------------------

void IAdaptor::info(std::ostream& _sstream)
{
    _sstream << "IAdaptor : " << this->getID();
    this->service::IService::info(_sstream);
}

//------------------------------------------------------------------------------

void IAdaptor::configureParams()
{
    const ConfigType config = this->getConfiguration();
    m_cfgLayerID = config.get<std::string>("config.<xmlattr>.layer", "");
    m_isVisible  = config.get<bool>("config.<xmlattr>.visible", m_isVisible);

    SIGHT_WARN_IF(
        "In [" + this->getID() + "] adaptor, specifying a layer is now deprecated. "
                                 "Please place the adaptor in the layer tag in the render scene configuration.",
        !m_cfgLayerID.empty()
    );
}

//------------------------------------------------------------------------------

void IAdaptor::initialize()
{
    if(m_renderService.expired())
    {
        auto servicesVector = service::getServices("sight::viz::scene3d::SRender");

        auto& registry = viz::scene3d::registry::getAdaptorRegistry();
        auto layerCfg  = registry[this->getID()];

        auto result =
            std::find_if(
                servicesVector.begin(),
                servicesVector.end(),
                [layerCfg](const service::IService::sptr& srv)
            {
                return srv->getID() == layerCfg.render;
            });
        SIGHT_ASSERT("Can't find '" + layerCfg.render + "' SRender service.", result != servicesVector.end());

        m_renderService = viz::scene3d::SRender::dynamicCast(*result);

        m_layerID = layerCfg.layer.empty() ? m_cfgLayerID : layerCfg.layer;
    }
}

//------------------------------------------------------------------------------

void IAdaptor::setLayerID(const std::string& _id)
{
    m_layerID = _id;
}

//------------------------------------------------------------------------------

const std::string& IAdaptor::getLayerID() const
{
    return m_layerID;
}

//------------------------------------------------------------------------------

void IAdaptor::setRenderService(SRender::sptr _service)
{
    SIGHT_ASSERT("service not instanced", _service);
    SIGHT_ASSERT("The adaptor ('" + this->getID() + "') is not stopped", this->isStopped());

    m_renderService = _service;
}

//------------------------------------------------------------------------------

SRender::sptr IAdaptor::getRenderService() const
{
    return m_renderService.lock();
}

//------------------------------------------------------------------------------

Layer::sptr IAdaptor::getLayer() const
{
    return this->getRenderService()->getLayer(m_layerID);
}

//------------------------------------------------------------------------------

Ogre::SceneManager* IAdaptor::getSceneManager()
{
    return m_renderService.lock()->getSceneManager(m_layerID);
}

//------------------------------------------------------------------------------

void IAdaptor::requestRender()
{
    auto renderService = this->getRenderService();
    if((renderService->getStatus() == service::IService::STARTED
        || renderService->getStatus() == service::IService::SWAPPING)
       && renderService->getRenderMode() == viz::scene3d::SRender::RenderMode::AUTO)
    {
        renderService->requestRender();
    }
}

//-----------------------------------------------------------------------------

void IAdaptor::updateVisibility(bool _isVisible)
{
    m_isVisible = _isVisible;
    this->setVisible(m_isVisible);
}

//------------------------------------------------------------------------------

void IAdaptor::toggleVisibility()
{
    this->updateVisibility(!m_isVisible);
}

//------------------------------------------------------------------------------

void IAdaptor::show()
{
    this->updateVisibility(true);
}

//------------------------------------------------------------------------------

void IAdaptor::hide()
{
    this->updateVisibility(false);
}

//------------------------------------------------------------------------------

void IAdaptor::setVisible(bool /*unused*/)
{
    SIGHT_WARN("This adaptor has no method 'setVisible(bool)', it needs to be overridden to be called.");
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d.
