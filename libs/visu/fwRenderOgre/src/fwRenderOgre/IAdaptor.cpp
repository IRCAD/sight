/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
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

#include "fwRenderOgre/IAdaptor.hpp"

#include <fwRenderOgre/registry/Adaptor.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwCom/Slots.hxx>

#include <fwServices/helper/Config.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

namespace fwRenderOgre
{

const ::fwCom::Slots::SlotKeyType IAdaptor::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";
const ::fwCom::Slots::SlotKeyType IAdaptor::s_TOGGLE_VISIBILITY_SLOT = "toggleVisibility";
const ::fwCom::Slots::SlotKeyType IAdaptor::s_SHOW_SLOT              = "show";
const ::fwCom::Slots::SlotKeyType IAdaptor::s_HIDE_SLOT              = "hide";

static const std::string s_LAYER_CONFIG   = "layer";
static const std::string s_VISIBLE_CONFIG = "visible";

//------------------------------------------------------------------------------

IAdaptor::IAdaptor() noexcept
{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &IAdaptor::updateVisibility, this);
    newSlot(s_TOGGLE_VISIBILITY_SLOT, &IAdaptor::toggleVisibility, this);
    newSlot(s_SHOW_SLOT, &IAdaptor::show, this);
    newSlot(s_HIDE_SLOT, &IAdaptor::hide, this);
}

//------------------------------------------------------------------------------

IAdaptor::~IAdaptor() noexcept
{
}

//------------------------------------------------------------------------------

void IAdaptor::info(std::ostream& _sstream )
{
    _sstream << "IAdaptor : ";
    this->fwServices::IService::info( _sstream );
}

//------------------------------------------------------------------------------

void IAdaptor::configureParams()
{
    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");
    m_layerID   = config.get<std::string>(s_LAYER_CONFIG);
    m_isVisible = config.get<bool>(s_VISIBLE_CONFIG, m_isVisible);
}

//------------------------------------------------------------------------------

void IAdaptor::initialize()
{
    if(m_renderService.expired())
    {
        auto servicesVector = ::fwServices::OSR::getServices("::fwRenderOgre::SRender");

        auto& registry       = ::fwRenderOgre::registry::getAdaptorRegistry();
        auto renderServiceId = registry[this->getID()];

        auto result =
            std::find_if(servicesVector.begin(), servicesVector.end(),
                         [renderServiceId](const ::fwServices::IService::sptr& srv)
            {
                return srv->getID() == renderServiceId;
            });
        SLM_ASSERT("Can't find '" + renderServiceId + "' SRender service.", result != servicesVector.end());

        m_renderService = ::fwRenderOgre::SRender::dynamicCast(*result);
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

void IAdaptor::setRenderService( SRender::sptr _service)
{
    SLM_ASSERT("service not instanced", _service);
    SLM_ASSERT("The adaptor ('"+this->getID()+"') is not stopped", this->isStopped());

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

::Ogre::SceneManager* IAdaptor::getSceneManager()
{
    return m_renderService.lock()->getSceneManager(m_layerID);
}

//------------------------------------------------------------------------------

void IAdaptor::requestRender()
{
    /*
    auto renderService = this->getRenderService();
    if ( (renderService->getStatus() == ::fwServices::IService::STARTED ||
          renderService->getStatus() == ::fwServices::IService::SWAPPING) &&
         renderService->getRenderMode() == ::fwRenderOgre::SRender::RenderMode::AUTO )
    {
        this->getRenderService()->requestRender();
    }
    */
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

void IAdaptor::setVisible(bool)
{
    SLM_WARN("This adaptor has no method 'setVisible(bool)', it needs to be overridden to be called.");
}

//------------------------------------------------------------------------------

} // namespace fwRenderOgre.
