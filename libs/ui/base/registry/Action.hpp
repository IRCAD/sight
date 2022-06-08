/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ui/base/config.hpp"
#include "ui/base/GuiBaseObject.hpp"

namespace sight::ui::base
{

namespace registry
{

/**
 * @brief   Defines the action registry for IHM.
 */
class UI_BASE_CLASS_API Action : public ui::base::GuiBaseObject
{
public:

    SIGHT_DECLARE_CLASS(Action, ui::base::GuiBaseObject);

    //------------------------------------------------------------------------------

    static sptr New(const std::string& sid)
    {
        return std::make_shared<Action>(sid);
    }

    /// Constructor.
    UI_BASE_API Action(const std::string& sid);

    /// Destructor. Do nothing
    UI_BASE_API virtual ~Action();

    /// Method called when the action service is stopping
    UI_BASE_API void actionServiceStopping();

    /// Method called when the action service is starting
    UI_BASE_API void actionServiceStarting();

    /// Method called when the action service is activated or not
    UI_BASE_API void actionServiceSetChecked(bool isChecked);

    /// Method called when the action service is enabled or not
    UI_BASE_API void actionServiceSetEnabled(bool isEnabled);

    /// Method called when the action service is visible or not
    UI_BASE_API void actionServiceSetVisible(bool isVisible);

protected:

    /// Main service ID associate with this Action
    std::string m_sid;
};

} // namespace registry

} // namespace sight::ui::base
