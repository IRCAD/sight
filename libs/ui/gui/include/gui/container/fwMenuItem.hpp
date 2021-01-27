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

#pragma once

#include "gui/config.hpp"
#include "gui/GuiBaseObject.hpp"

namespace sight::gui
{
namespace container
{
/**
 * @brief   Defines the menu item for IHM.
 */
class GUI_CLASS_API fwMenuItem : public gui::GuiBaseObject
{

public:

    fwCoreClassMacro(fwMenuItem, gui::GuiBaseObject)

    GUI_API virtual void clean()            = 0;
    GUI_API virtual void destroyContainer() = 0;

};
} // namespace container
} // namespace sight::gui
