/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "action.hpp"

#include "ui/__/registry.hpp"

#include <core/id.hpp>

#include <service/macros.hpp>

#include <utility>

namespace sight::ui::detail::registry
{

//-----------------------------------------------------------------------------

action::action(std::string _sid) :
    m_sid(std::move(_sid))
{
}

//-----------------------------------------------------------------------------

void action::action_service_stopping()
{
    ui::registry::action_service_stopping(m_sid);
}

//-----------------------------------------------------------------------------

void action::action_service_starting()
{
    ui::registry::action_service_starting(m_sid);
}

//-----------------------------------------------------------------------------

void action::action_service_set_checked(bool _is_checked)
{
    ui::registry::action_service_set_checked(m_sid, _is_checked);
}

//-----------------------------------------------------------------------------

void action::action_service_set_enabled(bool _is_enabled)
{
    ui::registry::action_service_set_enabled(m_sid, _is_enabled);
}

//-----------------------------------------------------------------------------

void action::action_service_set_visible(bool _is_visible)
{
    ui::registry::action_service_set_visible(m_sid, _is_visible);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::detail::registry
