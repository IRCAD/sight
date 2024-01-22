/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include "modules/ui/qt/config.hpp"

#include <service/controller.hpp>
#include <service/notifier.hpp>

#include <ui/__/dialog/notification.hpp>

namespace sight::module::ui::qt
{

/**
 * @brief notifier is a general service used to display notification in a centralized way.
 * notifier needs to be connected to [Success/Failure/Info]Notified signals implemented in base.
 *
 * @section Slots Slots
 * - \b pop(): Adds a popup in the queue & display it.
 * - \b close_notification(std::string channel): Close the popup associated with the given channel.
 * - \b set_enum_parameter(std::string value, std::string key): Changes the position of notifications (key "position"),
 * accepted values are the same than the "position" tag in the XML configuration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::ui::qt::notifier">
            <message>Default Message</message>
            <parent uid="myContainerID"/>
            <channels>
                <channel position="TOP_RIGHT" duration="3000" max="3" />
                <channel uid="INFO" position="TOP_LEFT" duration="5000" max="2" />
                <channel uid="${ERROR_CHANNEL}" position="BOTTOM_RIGHT" size="400x60" />
            </channels>
        </service>
   @endcode
 *
 * @subsection Configuration Configuration
 * - \b message (optional): Default message of the notification if the emitted signal contains empty string.
 *                          (default: "Notification").
 * - \b parent (optional): UID of the gui Container where the notifications will be displayed (default the whole app),
 *                         NOTE: we use the xml attribute "uid" to resolve "${GENERIC_UID}_" prefixes.
 * - \b channels (optional): Channels sub-configuration.
 *   - \b channel (optional): Channel specific configuration.
 *
 *     - \b uid (optional): The uid that identify the channel. If no uid is set, then the settings apply to the global
 *              default, where notification widgets are not shared, but will share the configuration
 *              (position, duration, ...).
 *              All notification request that use the same channel, will share the same notification dialog widget and
 *              the same configuration.
 *
 *     - \b position (optional): Position of the notification queue (default: TOP_RIGHT).
 *                   Accepted values are:
 *                   - TOP_RIGHT: default value.
 *                   - TOP_LEFT
 *                   - CENTERED_TOP
 *                   - CENTERED
 *                   - BOTTOM_RIGHT
 *                   - BOTTOM_LEFT
 *                   - CENTERED_BOTTOM
 *
 *                   Different channels can share the same postion. However, some configuration (most notably "max"
 *                   and size) will be shared since they configure the behavior of the stack. If so, the value may be
 *                   "harmonized" across all dialogs by taking the maximum value.
 *                   @note This may change if the positioning algorithm got updated
 *
 *     - \b duration (optional): Override duration in ms (+ 1 sec for fadein & fadeout effects).
 *
 *     - \b max (optional): maximum number of notifications in the same position.
 *              Permanent notification are not counted.
 *
 *     - \b size (optional): size of notifications in the same position.
 *
 *     - \b closable (optional): override default which is closable for timed notification. This is mostly useful to
 *                   allow closing of permanent notification
 *
 */
class MODULE_UI_QT_CLASS_API notifier final : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(notifier, service::controller);

    /// Constructor, initializes position map & slots.
    MODULE_UI_QT_API notifier() noexcept;

    /// Destructor, clears the position map.
    MODULE_UI_QT_API ~notifier() noexcept override = default;

    /// Slot: This method is used to set an enum parameter.
    MODULE_UI_QT_API void set_enum_parameter(std::string _val, std::string _key);

    /// Slot: pops a notification.
    /// @param _notification notification.
    MODULE_UI_QT_API void pop(service::notification _notification);

    /// Slot: close a notification identified by the channel name.
    /// @param _channel notification channel.
    MODULE_UI_QT_API void close_notification(std::string _channel);

protected:

    /** @name Service methods ( override from service::base )
     * @{
     */

    /// This method configures the service
    MODULE_UI_QT_API void configuring() override;

    /**
     * @brief Starts and setups the service optionally gets the parent container SID/WID if set.
     */
    MODULE_UI_QT_API void starting() override;

    /**
     * @brief Stops & clears the service
     */
    MODULE_UI_QT_API void stopping() override;

    /**
     * @brief This method does nothing.
     */
    MODULE_UI_QT_API void updating() override;

private:

    /// Called when a notification is closed
    void on_notification_closed(const sight::ui::dialog::notification::sptr& _notif);

    /// Erase a notification from m_popups and move down the remaining
    /// @param _position The stack where we need to erase a notification
    /// @param _it the iterator pointing on the element to erase
    std::list<sight::ui::dialog::notification::sptr>::iterator erase_notification(
        const enum service::notification::position& _position,
        const std::list<sight::ui::dialog::notification::sptr>::iterator& _it
    );

    /// Count the number of notifications and remove the oldest if > m_maxStackedNotifs
    /// @param _position The stack to clean
    /// @param _max The maximum number of element
    /// @param _skip_permanent if true, only non permanent notifications are counted
    void clean_notifications(
        const enum service::notification::position& _position,
        std::size_t _max,
        std::array<int, 2> _size,
        bool _skip_permanent = true
    );

    /// Default message (if message in slot are empty), the default message can be configured in xml.
    std::string m_default_message {"Notification"};

    struct configuration final
    {
        std::optional<enum service::notification::position> position {std::nullopt};
        std::optional<std::chrono::milliseconds> duration {std::nullopt};
        std::optional<std::array<int, 2> > size {std::nullopt};
        std::optional<std::size_t> max {std::nullopt};
        std::optional<bool> closable {std::nullopt};
    };

    std::map<std::string, configuration> m_channels {{"", {.max = {3}}}};

    /// A stack of notification
    struct stack final
    {
        std::optional<std::array<int, 2> > size {std::nullopt};
        std::optional<std::size_t> max {std::nullopt};
        std::list<sight::ui::dialog::notification::sptr> popups {};
    };

    /// Map of displayed Stack
    std::map<enum service::notification::position, stack> m_stacks {
        {service::notification::position::top_right, {}},
        {service::notification::position::top_left, {}},
        {service::notification::position::bottom_right, {}},
        {service::notification::position::bottom_left, {}},
        {service::notification::position::centered, {}},
        {service::notification::position::centered_top, {}},
        {service::notification::position::centered_bottom, {}},
    };

    /// widget where notifications will be displayed in, nullptr by default.
    sight::ui::container::widget::csptr m_container_where_to_display_notifs {nullptr};

    /// Parent container ID (SID or WID), empty by default.
    std::string m_parent_container_id;
};

} //namespace sight::module::ui::qt
