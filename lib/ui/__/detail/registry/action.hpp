/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/ui/__/config.hpp>

#include "ui/__/object.hpp"

namespace sight::ui::detail::registry
{

/**
 * @brief   Defines the action registry for UI.
 */
class action : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(action, ui::object);

    //------------------------------------------------------------------------------

    static sptr make(const std::string& _sid)
    {
        return std::make_shared<action>(_sid);
    }

    /// Constructor.
    action(std::string _sid);

    /// Destructor. Do nothing
    ~action() override = default;

    /// Method called when the action service is stopping
    void action_service_stopping();

    /// Method called when the action service is starting
    void action_service_starting();

    /// Method called when the action service is activated or not
    void action_service_set_checked(bool _is_checked);

    /// Method called when the action service is enabled or not
    void action_service_set_enabled(bool _is_enabled);

    /// Method called when the action service is visible or not
    void action_service_set_visible(bool _is_visible);

protected:

    /// Main service ID associate with this action
    std::string m_sid;
};

} // namespace sight::ui::detail::registry
