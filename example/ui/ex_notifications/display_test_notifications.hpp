/************************************************************************
 *
 * Copyright (C) 2020-2026 IRCAD France
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

#include <data/boolean.hpp>
#include <data/string.hpp>

#include <sight/ex_notifications/config.hpp>

#include <core/notification/has_notifications.hpp>

#include <ui/__/action.hpp>
#include <ui/__/dialog/notification_base.hpp>

/**
 * Do not mark `ex_notifications` as incorrect.
 * cspell:ignore ex_notifications
 */

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
 *   - Values for 'type' key : INSTRUCTION, INFORMATION, WARNING, ERROR.
 * - \b set_bool_parameterbool _val, std::string _key): call this slot when changing "m_usenotifier" behavior.
 *
 * @section Signals Signals
 * - \b notification_closed(std::string): emitted with a channel name when close_channel1() is triggered.
 *
 * @subsection ConfigBindings Configuration data bindings
 * Configuration inputs accept literals or shared data objects.
 * Default values apply when no configuration value is provided.
 * - \b config.usenotifier [sight::data::boolean]: usenotifier.
 * - \b config.reach_max_characters [sight::data::boolean]: reach max characters.
 * - \b config.sound [sight::data::boolean]: sound.
 * - \b config.duration [sight::data::string]: duration.
 * - \b config.type [sight::data::string]: type.
 * - \b config.position [sight::data::string]: position.
 * - \b config.channel [sight::data::string]: channel.
 */
class SIGHT_EX_NOTIFICATIONS_CLASS_API display_test_notifications final :
    public sight::ui::action,
    public sight::core::notification::has_notifications
{
public:

    SIGHT_DECLARE_SERVICE(display_test_notifications, sight::ui::action);

    struct signals
    {
        using notification_closed_t = sight::core::com::signal<void (std::string)>;
        static inline const signal_key_t NOTIFICATION_CLOSED = "notification_closed";
    };

    struct slots
    {
        static inline const slot_key_t SET_ENUM_PARAMETER = "set_enum_parameter";
        static inline const slot_key_t SET_BOOL_PARAMETER = "set_bool_parameter";
        static inline const slot_key_t CLOSE_CHANNEL1     = "close_channel1";
    };

    /// Constructor/Destructor
    /// @{
    /// Initializes slot.
    SIGHT_EX_NOTIFICATIONS_API display_test_notifications() noexcept;
    SIGHT_EX_NOTIFICATIONS_API ~display_test_notifications() noexcept override = default;
    /// Slot: This method is used to set an enum parameter.
    SIGHT_EX_NOTIFICATIONS_API void set_enum_parameter(std::string _val, std::string _key);

    /// Slot: This method is used to set a bool parameter.
    SIGHT_EX_NOTIFICATIONS_API void set_bool_parameter(bool _val, std::string _key);

    /// Slot: This method is used to set a bool parameter.
    SIGHT_EX_NOTIFICATIONS_API void close_channel1();

protected:

    sight::service::connections_t auto_connections() const override;

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

    /// Apply settings from bound data at startup and after external modifications.

    void apply_config_sound();
    void apply_config_duration();
    void apply_config_type();
    void apply_config_position();
    void apply_config_channel();

    /// Notification position, type, duration
    sight::ui::dialog::notification_base::params m_notification;

    /// Display notification at all position, default true.
    bool m_display_all {false};

    /// To test the "Read more..." option on Notification.

    /// Typed config.usenotifier configuration binding.
    ptr_in<sight::data::boolean> m_usenotifier {this, "config.usenotifier", true};

    /// Typed config.reach_max_characters configuration binding.
    ptr_in<sight::data::boolean> m_reach_max_characters {this, "config.reach_max_characters", false};

    /// Typed config.sound configuration binding.
    ptr_in<sight::data::boolean> m_config_sound {this, "config.sound", std::nullopt};

    /// Typed config.duration configuration binding.
    ptr_in<sight::data::string> m_config_duration {this, "config.duration", std::nullopt};

    /// Typed config.type configuration binding.
    ptr_in<sight::data::string> m_config_type {this, "config.type", std::nullopt};

    /// Typed config.position configuration binding.
    ptr_in<sight::data::string> m_config_position {this, "config.position", std::nullopt};

    /// Typed config.channel configuration binding.
    ptr_in<sight::data::string> m_config_channel {this, "config.channel", std::nullopt};
};

} // namespace ex_notifications
