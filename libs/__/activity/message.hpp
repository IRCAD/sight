/************************************************************************
 *
 * Copyright (C) 2015-2024 IRCAD France
 * Copyright (C) 2015 IHU Strasbourg
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

#include <sight/activity/config.hpp>

#include "activity/extension/activity.hpp"

#include <data/activity.hpp>

namespace sight::activity
{

/**
 * @brief Activity information sent by signal to launch new activities in a tab.
 */
class SIGHT_ACTIVITY_CLASS_API message
{
public:

    using parameters_t = activity::extension::activity_config_params_type;

    SIGHT_ACTIVITY_API message(
        const data::activity::sptr& _activity,
        const activity::extension::activity_info& _info,
        const parameters_t& _parameters
    );

    SIGHT_ACTIVITY_API virtual ~message()
    = default;

    /// Return if the activity can be closed
    [[nodiscard]] bool is_closable() const
    {
        return m_closable;
    }

    /// Return activity title
    [[nodiscard]] const std::string& get_title() const
    {
        return m_title;
    }

    /// Return tab identifier
    [[nodiscard]] const std::string& get_tab_id() const
    {
        return m_tab_id;
    }

    /// Return appConfig identifier
    [[nodiscard]] const std::string& get_app_config_id() const
    {
        return m_app_config_id;
    }

    /// Return tab information
    [[nodiscard]] const std::string& get_tab_info() const
    {
        return m_tab_info;
    }

    /// Return activity icon path
    [[nodiscard]] const std::string& get_icon_path() const
    {
        return m_icon_path;
    }

    /// Return tooltip
    [[nodiscard]] const std::string& get_tool_tip() const
    {
        return m_tooltip;
    }

    /// Return activity
    [[nodiscard]] const data::activity::sptr& get_activity() const
    {
        return m_activity;
    }

    /// Return the map of the string association to replace in config
    [[nodiscard]] const std::map<std::string, std::string>& get_replacement_map() const
    {
        return m_replacement_map;
    }

private:

    /// Flag if activity is closable.
    bool m_closable {true};

    /// Activity title
    std::string m_title;

    /// Tab identifier
    std::string m_tab_id;

    /// config id
    std::string m_app_config_id;

    /// tab information
    std::string m_tab_info;

    /// icon path
    std::string m_icon_path;

    /// tab tooltip
    std::string m_tooltip;

    /// activity
    data::activity::sptr m_activity;

    /// map containing string to replace in activity configuration.
    std::map<std::string, std::string> m_replacement_map;
};

} // namespace sight::activity
