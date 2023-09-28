/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/config.hpp"
#include "ui/__/object.hpp"

namespace sight::ui::detail::registry
{

/**
 * @brief   Defines the action registry for IHM.
 */
class Action : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(Action, ui::object);

    //------------------------------------------------------------------------------

    static sptr make(const std::string& sid)
    {
        return std::make_shared<Action>(sid);
    }

    /// Constructor.
    Action(std::string sid);

    /// Destructor. Do nothing
    ~Action() override = default;

    /// Method called when the action service is stopping
    void actionServiceStopping();

    /// Method called when the action service is starting
    void actionServiceStarting();

    /// Method called when the action service is activated or not
    void actionServiceSetChecked(bool isChecked);

    /// Method called when the action service is enabled or not
    void actionServiceSetEnabled(bool isEnabled);

    /// Method called when the action service is visible or not
    void actionServiceSetVisible(bool isVisible);

protected:

    /// Main service ID associate with this Action
    std::string m_sid;
};

} // namespace sight::ui::detail::registry
