/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "guiQt/ActionCallback.hpp"

#include <gui/registry/macros.hpp>

fwGuiRegisterMacro( ::sight::guiQt::ActionCallback, ::sight::gui::ActionCallbackBase::REGISTRY_KEY );

namespace sight::guiQt
{

//-----------------------------------------------------------------------------

ActionCallback::ActionCallback(gui::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

ActionCallback::~ActionCallback()
{
}

//-----------------------------------------------------------------------------

// if action is checkable, executeQt is executed after checkQt
void ActionCallback::executeQt(bool checked)
{
    this->execute();
}

//-----------------------------------------------------------------------------

void ActionCallback::checkQt(bool checked)
{
    this->check(checked);
}

} // namespace sight::guiQt
