/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ui/base/GuiRegistry.hpp"

#include <core/tools/fwID.hpp>

#include <service/macros.hpp>

#include <utility>

namespace sight::ui::base
{

namespace registry
{

//-----------------------------------------------------------------------------

Action::Action(const std::string& sid) :
    m_sid(sid)
{
}

//-----------------------------------------------------------------------------

Action::~Action()
{
}

//-----------------------------------------------------------------------------

void Action::actionServiceStopping()
{
    ui::base::GuiRegistry::actionServiceStopping(m_sid);
}

//-----------------------------------------------------------------------------

void Action::actionServiceStarting()
{
    ui::base::GuiRegistry::actionServiceStarting(m_sid);
}

//-----------------------------------------------------------------------------

void Action::actionServiceSetChecked(bool isChecked)
{
    ui::base::GuiRegistry::actionServiceSetChecked(m_sid, isChecked);
}

//-----------------------------------------------------------------------------

void Action::actionServiceSetEnabled(bool isEnabled)
{
    ui::base::GuiRegistry::actionServiceSetEnabled(m_sid, isEnabled);
}

//-----------------------------------------------------------------------------

void Action::actionServiceSetVisible(bool isVisible)
{
    ui::base::GuiRegistry::actionServiceSetVisible(m_sid, isVisible);
}

//-----------------------------------------------------------------------------

} // namespace registry

} //namespace sight::ui::base
