/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "adaptor.hpp"

#include "viz/qt3d/registry/adaptor.hpp"

#include <core/com/slots.hxx>

#include <service/registry.hpp>

namespace sight::viz::qt3d
{

//-----------------------------------------------------------------------------

const sight::core::com::slots::key_t adaptor::UPDATE_VISIBILITY_SLOT = "updateVisibility";
const sight::core::com::slots::key_t adaptor::TOGGLE_VISIBILITY_SLOT = "toggleVisibility";
const sight::core::com::slots::key_t adaptor::SHOW_SLOT              = "show";
const sight::core::com::slots::key_t adaptor::HIDE_SLOT              = "hide";

static const std::string s_VISIBLE_CONFIG = "visible";

//------------------------------------------------------------------------------

adaptor::adaptor()
{
    new_slot(UPDATE_VISIBILITY_SLOT, &adaptor::updateVisibility, this);
    new_slot(TOGGLE_VISIBILITY_SLOT, &adaptor::toggleVisibility, this);
    new_slot(SHOW_SLOT, &adaptor::show, this);
    new_slot(HIDE_SLOT, &adaptor::hide, this);
}

//------------------------------------------------------------------------------

adaptor::~adaptor()
= default;

//------------------------------------------------------------------------------

void adaptor::configureParams()
{
    const config_t config = this->get_config().get_child("config.<xmlattr>");
    m_isVisible = config.get<bool>(s_VISIBLE_CONFIG, m_isVisible);
}

//------------------------------------------------------------------------------

void adaptor::initialize()
{
    // Retrieve the render service attached to the adaptor.
    if(m_renderService.expired())
    {
        auto servicesVector = sight::service::getServices("sight::viz::qt3d::render");

        auto& registry       = viz::qt3d::registry::get_adaptor_registry();
        auto renderServiceId = registry[this->get_id()];

        auto result =
            std::find_if(
                servicesVector.begin(),
                servicesVector.end(),
                [renderServiceId](const service::base::sptr& srv)
            {
                return srv->get_id() == renderServiceId;
            });
        SIGHT_ASSERT("Can't find '" + renderServiceId + "' render service.", result != servicesVector.end());

        m_renderService = std::dynamic_pointer_cast<viz::qt3d::render>(*result);
    }
}

//-----------------------------------------------------------------------------

void adaptor::updateVisibility(bool _visibility)
{
    // Enable/disable qt3d entity according to _visibility.
    m_isVisible = _visibility;
    this->setVisible(m_isVisible);
}

//-----------------------------------------------------------------------------

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

render::sptr adaptor::getRenderService() const
{
    return m_renderService.lock();
}

//------------------------------------------------------------------------------

} // namespace sight::viz::qt3d.
