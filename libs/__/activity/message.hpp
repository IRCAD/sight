/************************************************************************
 *
 * Copyright (C) 2015-2023 IRCAD France
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

#include "activity/config.hpp"
#include "activity/extension/activity.hpp"

#include <data/activity.hpp>

namespace sight::activity
{

/**
 * @brief Activity information sent by signal to launch new activities in a tab.
 */
class ACTIVITY_CLASS_API message
{
public:

    typedef activity::extension::activity_config_params_type ParametersType;

    ACTIVITY_API message(
        const data::activity::sptr& activity,
        const activity::extension::activity_info& info,
        const ParametersType& parameters
    );

    ACTIVITY_API virtual ~message()
    = default;

    /// Return if the activity can be closed
    [[nodiscard]] bool isClosable() const
    {
        return m_closable;
    }

    /// Return activity title
    [[nodiscard]] const std::string& getTitle() const
    {
        return m_title;
    }

    /// Return tab identifier
    [[nodiscard]] const std::string& getTabID() const
    {
        return m_tabID;
    }

    /// Return appConfig identifier
    [[nodiscard]] const std::string& getAppConfigID() const
    {
        return m_appConfigID;
    }

    /// Return tab information
    [[nodiscard]] const std::string& getTabInfo() const
    {
        return m_tabInfo;
    }

    /// Return activity icon path
    [[nodiscard]] const std::string& getIconPath() const
    {
        return m_iconPath;
    }

    /// Return tooltip
    [[nodiscard]] const std::string& getToolTip() const
    {
        return m_tooltip;
    }

    /// Return activity
    [[nodiscard]] const data::activity::sptr& getActivity() const
    {
        return m_activity;
    }

    /// Return the map of the string association to replace in config
    [[nodiscard]] const std::map<std::string, std::string>& getReplacementMap() const
    {
        return m_replacementMap;
    }

private:

    /// Flag if activity is closable.
    bool m_closable {true};

    /// Activity title
    std::string m_title;

    /// Tab identifier
    std::string m_tabID;

    /// config id
    std::string m_appConfigID;

    /// tab information
    std::string m_tabInfo;

    /// icon path
    std::string m_iconPath;

    /// tab tooltip
    std::string m_tooltip;

    /// activity
    data::activity::sptr m_activity;

    /// map containing string to replace in activity configuration.
    std::map<std::string, std::string> m_replacementMap;
};

} // namespace sight::activity
