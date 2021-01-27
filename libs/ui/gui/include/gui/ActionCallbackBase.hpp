/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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
#include "gui/IMenuItemCallback.hpp"

#include <core/base.hpp>

namespace sight::gui
{

/**
 * @brief   Defines the menu item callback.
 */
class GUI_CLASS_API ActionCallbackBase : public gui::IMenuItemCallback
{

public:

    fwCoreClassMacro(ActionCallbackBase, gui::IMenuItemCallback)

    typedef std::string RegistryKeyType;

    /**
     * @brief  Sets service ID updated in executed method.
     */
    GUI_API virtual void setSID(std::string sid);

    /**
     * @brief  Called method when callback is executed.
     */
    GUI_API virtual void execute() override;

    /**
     * @brief  Called method when callback is checked.
     */
    GUI_API virtual void check(bool checked);

    GUI_API static const RegistryKeyType REGISTRY_KEY;

protected:

    /**
     * @brief   Constructor.
     */
    GUI_API ActionCallbackBase();

    /**
     * @brief   Destructor. Do nothing.
     */
    GUI_API virtual ~ActionCallbackBase();

private:
    std::string m_sid;

};

} // namespace sight::gui
