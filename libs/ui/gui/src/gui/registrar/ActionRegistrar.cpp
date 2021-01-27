/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "gui/registrar/ActionRegistrar.hpp"

#include "gui/GuiRegistry.hpp"

#include <core/tools/fwID.hpp>

#include <services/macros.hpp>

#include <utility>

namespace sight::gui
{
namespace registrar
{

//-----------------------------------------------------------------------------

ActionRegistrar::ActionRegistrar(const std::string& sid) :
    m_sid(sid)
{
}

//-----------------------------------------------------------------------------

ActionRegistrar::~ActionRegistrar()
{
}

//-----------------------------------------------------------------------------

void ActionRegistrar::actionServiceStopping()
{
    gui::GuiRegistry::actionServiceStopping(m_sid);
}

//-----------------------------------------------------------------------------

void ActionRegistrar::actionServiceStarting()
{
    gui::GuiRegistry::actionServiceStarting(m_sid);
}

//-----------------------------------------------------------------------------

void ActionRegistrar::actionServiceSetActive(bool isActive)
{
    gui::GuiRegistry::actionServiceSetActive(m_sid, isActive);
}

//-----------------------------------------------------------------------------

void ActionRegistrar::actionServiceSetExecutable(bool isExecutable)
{
    gui::GuiRegistry::actionServiceSetExecutable(m_sid, isExecutable);
}

//-----------------------------------------------------------------------------

void ActionRegistrar::actionServiceSetVisible(bool isVisible)
{
    gui::GuiRegistry::actionServiceSetVisible(m_sid, isVisible);
}

//-----------------------------------------------------------------------------

} // namespace registrar
} //namespace sight::gui
