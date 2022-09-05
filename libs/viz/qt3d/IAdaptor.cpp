/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "IAdaptor.hpp"

#include "viz/qt3d/registry/Adaptor.hpp"

#include <core/com/Slots.hxx>

namespace sight::viz::qt3d
{

//-----------------------------------------------------------------------------

const sight::core::com::Slots::SlotKeyType IAdaptor::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";
const sight::core::com::Slots::SlotKeyType IAdaptor::s_TOGGLE_VISIBILITY_SLOT = "toggleVisibility";
const sight::core::com::Slots::SlotKeyType IAdaptor::s_SHOW_SLOT              = "show";
const sight::core::com::Slots::SlotKeyType IAdaptor::s_HIDE_SLOT              = "hide";

static const std::string s_VISIBLE_CONFIG = "visible";

//------------------------------------------------------------------------------

IAdaptor::IAdaptor()
{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &IAdaptor::updateVisibility, this);
    newSlot(s_TOGGLE_VISIBILITY_SLOT, &IAdaptor::toggleVisibility, this);
    newSlot(s_SHOW_SLOT, &IAdaptor::show, this);
    newSlot(s_HIDE_SLOT, &IAdaptor::hide, this);
}

//------------------------------------------------------------------------------

IAdaptor::~IAdaptor()
= default;

//------------------------------------------------------------------------------

void IAdaptor::configureParams()
{
    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");
    m_isVisible = config.get<bool>(s_VISIBLE_CONFIG, m_isVisible);
}

//------------------------------------------------------------------------------

void IAdaptor::initialize()
{
    // Retrieve the render service attached to the adaptor.
    if(m_renderService.expired())
    {
        auto servicesVector = service::OSR::getServices("sight::viz::qt3d::SRender");

        auto& registry       = viz::qt3d::registry::getAdaptorRegistry();
        auto renderServiceId = registry[this->getID()];

        auto result =
            std::find_if(
                servicesVector.begin(),
                servicesVector.end(),
                [renderServiceId](const service::IService::sptr& srv)
            {
                return srv->getID() == renderServiceId;
            });
        SIGHT_ASSERT("Can't find '" + renderServiceId + "' SRender service.", result != servicesVector.end());

        m_renderService = viz::qt3d::SRender::dynamicCast(*result);
    }
}

//-----------------------------------------------------------------------------

void IAdaptor::updateVisibility(bool _visibility)
{
    // Enable/disable qt3d entity according to _visibility.
    m_isVisible = _visibility;
    this->setVisible(m_isVisible);
}

//-----------------------------------------------------------------------------

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

SRender::sptr IAdaptor::getRenderService() const
{
    return m_renderService.lock();
}

//------------------------------------------------------------------------------

} // namespace sight::viz::qt3d.
