/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include <sight/ex_notifications/config.hpp>

#include <service/notifier.hpp>

#include <ui/__/action.hpp>
#include <ui/__/dialog/notification.hpp>

/**
 * Do not mark `ex_notifications` as incorrect.
 * cspell:ignore ex_notifications
 */

namespace dial = sight::ui::dialog;

namespace ex_notifications
{

/**
 * @brief  This action displays test notifications by calling updating().
 *
 * @section Slots Slots
 * - \b set_enum_parameter (std::string _value, std::string _key) : call this slot when changing the position or
 * the type of displayed notification( accepted _key are 'position' & 'type').
 *   - Values for 'position' key : ALL, TOP_LEFT, TOP_RIGHT, CENTERED_TOP, CENTERED, BOTTOM_LEFT, BOTTOM_RIGHT,
 * CENTERED_BOTTOM.
 *   - Values for 'type' key : INFO, SUCCESS, FAILURE.
 * - \b set_bool_parameterbool _val, std::string _key): call this slot when changing "m_usenotifier" behavior.
 */
class SIGHT_EX_NOTIFICATIONS_CLASS_API display_test_notifications final :
    public sight::ui::action,
    public sight::service::notifier
{
public:

    SIGHT_DECLARE_SERVICE(display_test_notifications, sight::ui::action);

    /// Constructor/Destructor
    /// @{
    /// Initializes slot.
    SIGHT_EX_NOTIFICATIONS_API display_test_notifications() noexcept;
    SIGHT_EX_NOTIFICATIONS_API ~display_test_notifications() noexcept override = default;
    /// @}

    /// Slot: This method is used to set an enum parameter.
    SIGHT_EX_NOTIFICATIONS_API void set_enum_parameter(std::string _val, std::string _key);

    /// Slot: This method is used to set a bool parameter.
    SIGHT_EX_NOTIFICATIONS_API void set_bool_parameter(bool _val, std::string _key);

    /// Slot: This method is used to set a bool parameter.
    SIGHT_EX_NOTIFICATIONS_API void close_channel1();

protected:

    /// Initializes action.
    void configuring() override;

    /// Starts action
    void starting() override;

    /// Display notification(s).
    void updating() override;

    /// Stops action
    void stopping() override;

    /// Prints service info
    void info(std::ostream& _sstream) override;

private:

    /// Notification position, type, duration
    sight::service::notification m_notification;

    /// Display notification at all position, default true.
    bool m_display_all {false};

    /// Use centralized Mode.
    bool m_usenotifier {true};

    /// To test the "Read more..." option on Notification.
    bool m_reach_max_characters {false};
};

} // namespace ex_notifications
