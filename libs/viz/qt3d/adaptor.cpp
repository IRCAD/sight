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

const sight::core::com::slots::key_t adaptor::UPDATE_VISIBILITY_SLOT = "update_visibility";
const sight::core::com::slots::key_t adaptor::TOGGLE_VISIBILITY_SLOT = "toggle_visibility";
const sight::core::com::slots::key_t adaptor::SHOW_SLOT              = "show";
const sight::core::com::slots::key_t adaptor::HIDE_SLOT              = "hide";

static const std::string VISIBLE_CONFIG = "visible";

//------------------------------------------------------------------------------

adaptor::adaptor()
{
    new_slot(UPDATE_VISIBILITY_SLOT, &adaptor::update_visibility, this);
    new_slot(TOGGLE_VISIBILITY_SLOT, &adaptor::toggle_visibility, this);
    new_slot(SHOW_SLOT, &adaptor::show, this);
    new_slot(HIDE_SLOT, &adaptor::hide, this);
}

//------------------------------------------------------------------------------

adaptor::~adaptor()
= default;

//------------------------------------------------------------------------------

void adaptor::configure_params()
{
    const config_t config = this->get_config().get_child("config.<xmlattr>");
    m_visible = config.get<bool>(VISIBLE_CONFIG, m_visible);
}

//------------------------------------------------------------------------------

void adaptor::initialize()
{
    // Retrieve the render service attached to the adaptor.
    if(m_render_service.expired())
    {
        auto services_vector = sight::service::get_services("sight::viz::qt3d::render");

        auto& registry         = viz::qt3d::registry::get_adaptor_registry();
        auto render_service_id = registry[this->get_id()];

        auto result =
            std::find_if(
                services_vector.begin(),
                services_vector.end(),
                [render_service_id](const service::base::sptr& _srv)
            {
                return _srv->get_id() == render_service_id;
            });
        SIGHT_ASSERT("Can't find '" + render_service_id + "' render service.", result != services_vector.end());

        m_render_service = std::dynamic_pointer_cast<viz::qt3d::render>(*result);
    }
}

//-----------------------------------------------------------------------------

void adaptor::update_visibility(bool _visibility)
{
    // Enable/disable qt3d entity according to _visibility.
    m_visible = _visibility;
    this->set_visible(m_visible);
}

//-----------------------------------------------------------------------------

void adaptor::toggle_visibility()
{
    this->update_visibility(!m_visible);
}

//------------------------------------------------------------------------------

void adaptor::show()
{
    this->update_visibility(true);
}

//------------------------------------------------------------------------------

void adaptor::hide()
{
    this->update_visibility(false);
}

//------------------------------------------------------------------------------

void adaptor::set_visible(bool /*unused*/)
{
    SIGHT_WARN("This adaptor has no method 'setVisible(bool)', it needs to be overridden to be called.");
}

//------------------------------------------------------------------------------

render::sptr adaptor::render_service() const
{
    return m_render_service.lock();
}

//------------------------------------------------------------------------------

} // namespace sight::viz::qt3d.
