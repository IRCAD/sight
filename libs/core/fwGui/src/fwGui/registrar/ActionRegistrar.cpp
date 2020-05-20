/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
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

#include <utility>

#include <fwTools/fwID.hpp>
#include <fwServices/macros.hpp>

#include "fwGui/GuiRegistry.hpp"
#include "fwGui/registrar/ActionRegistrar.hpp"

namespace fwGui
{
namespace registrar
{

//-----------------------------------------------------------------------------

ActionRegistrar::ActionRegistrar(const std::string &sid) : m_sid(sid)
{
}

//-----------------------------------------------------------------------------

ActionRegistrar::~ActionRegistrar()
{
}

//-----------------------------------------------------------------------------

void ActionRegistrar::actionServiceStopping()
{
    ::fwGui::GuiRegistry::actionServiceStopping(m_sid);
}

//-----------------------------------------------------------------------------

void ActionRegistrar::actionServiceStarting()
{
    ::fwGui::GuiRegistry::actionServiceStarting(m_sid);
}

//-----------------------------------------------------------------------------

void ActionRegistrar::actionServiceSetActive(bool isActive)
{
    ::fwGui::GuiRegistry::actionServiceSetActive(m_sid, isActive);
}

//-----------------------------------------------------------------------------

void ActionRegistrar::actionServiceSetExecutable(bool isExecutable)
{
    ::fwGui::GuiRegistry::actionServiceSetExecutable(m_sid, isExecutable);
}

//-----------------------------------------------------------------------------

void ActionRegistrar::actionServiceSetVisible(bool isVisible)
{
    ::fwGui::GuiRegistry::actionServiceSetVisible(m_sid, isVisible);
}

//-----------------------------------------------------------------------------

} // namespace registrar
} //namespace fwGui
