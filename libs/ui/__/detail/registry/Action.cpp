/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "Action.hpp"

#include "ui/__/registry.hpp"

#include <core/tools/id.hpp>

#include <service/macros.hpp>

#include <utility>

namespace sight::ui::detail::registry
{

//-----------------------------------------------------------------------------

Action::Action(std::string sid) :
    m_sid(std::move(sid))
{
}

//-----------------------------------------------------------------------------

void Action::actionServiceStopping()
{
    ui::registry::actionServiceStopping(m_sid);
}

//-----------------------------------------------------------------------------

void Action::actionServiceStarting()
{
    ui::registry::actionServiceStarting(m_sid);
}

//-----------------------------------------------------------------------------

void Action::actionServiceSetChecked(bool isChecked)
{
    ui::registry::actionServiceSetChecked(m_sid, isChecked);
}

//-----------------------------------------------------------------------------

void Action::actionServiceSetEnabled(bool isEnabled)
{
    ui::registry::actionServiceSetEnabled(m_sid, isEnabled);
}

//-----------------------------------------------------------------------------

void Action::actionServiceSetVisible(bool isVisible)
{
    ui::registry::actionServiceSetVisible(m_sid, isVisible);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::detail::registry
