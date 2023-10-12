/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/config.hpp"
#include "ui/__/menu_item_callback.hpp"

#include <core/base.hpp>

namespace sight::ui
{

/**
 * @brief   Defines the menu item callback.
 */
class UI_CLASS_API ActionCallbackBase : public ui::menu_item_callback
{
public:

    SIGHT_DECLARE_CLASS(ActionCallbackBase, ui::menu_item_callback);

    typedef std::string RegistryKeyType;

    /**
     * @brief  Sets service ID updated in executed method.
     */
    UI_API virtual void setSID(std::string sid);

    /**
     * @brief  Called method when callback is executed.
     */
    UI_API void execute() override;

    /**
     * @brief  Called method when callback is checked.
     */
    UI_API virtual void check(bool checked);

    UI_API static const RegistryKeyType REGISTRY_KEY;

protected:

    UI_API ActionCallbackBase()           = default;
    UI_API ~ActionCallbackBase() override = default;

private:

    std::string m_sid;
};

} // namespace sight::ui
