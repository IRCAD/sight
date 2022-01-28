/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
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

#include <service/IController.hpp>

#include <ui/base/dialog/NotificationDialog.hpp>

namespace sight::module::ui::qt
{

/**
 * @brief SNotifier is a general service used to display notification in a centralized way.
 * SNotifier needs to be connected to [Success/Failure/Info]Notified signals implemented in IService.
 *
 * @section Slots Slots
 * - \b popInfo(): Adds an INFO popup in the queue & display it.
 * - \b popFailure(): Adds a FAILURE popup in the queue & display it.
 * - \b popSuccess():Adds a SUCCESS popup in the queue & display it.
 * - \b setEnumParameter(std::string value, std::string key): Changes the position of notifications (key "position"),
 * accepted values are the same than the "position" tag in the XML configuration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::ui::qt::SNotifier">
            <message>Default Message</message>
            <maxNotifications>3</maxNotifications>
            <position>TOP_RIGHT</position>
            <duration>3000</duration>
            <parent uid="myContainerID"/>
        </service>
   @endcode
 *
 * @subsection Configuration Configuration
 * - \b message (optional): Default message of the notification if the emitted signal contains empty string (default:
 * "Notification").
 * - \b maxNotifications (optional): Maximum number of queued notifications (default: 3).
 * - \b position (optional): Position of the notification queue (default: TOP_RIGHT).
 *  Accepted values are:
 *   - TOP_RIGHT: default value.
 *   - TOP_LEFT
 *   - CENTERED_TOP
 *   - CENTERED: when choosing CENTERED, only ONE notification can be displayed at once (maxNotifications is ignored).
 *   - BOTTOM_RIGHT
 *   - BOTTOM_LEFT
 *   - CENTERED_BOTTOM
 *
 * - \b duration (optional): Duration in ms of the notification (+ 1 sec for fadein & fadeout effects) (default:
 * 3000ms).
 * - \b parent (optional): UID of the gui Container where the notifications will be displayed (default the whole app),
 * NOTE: we use the xml attribute "uid" to resolve "${GENERIC_UID}_" prefixes.
 */
class MODULE_UI_QT_CLASS_API SNotifier final : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SNotifier, service::IController);

    /// Constructor, initializes position map & slots.
    MODULE_UI_QT_API SNotifier() noexcept;

    /// Destructor, clears the position map.
    MODULE_UI_QT_API ~SNotifier() noexcept override;

protected:

    /** @name Service methods ( override from service::IService )
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

    /// Slot: This method is used to set an enum parameter.
    void setEnumParameter(std::string _val, std::string _key);

    /**
     * @brief Slot: pop info notification
     * @param _message text of the notification
     */
    void popInfo(std::string _message);

    /**
     * @brief Slot: pop success notification
     * @param _message text of the notification
     */
    void popSuccess(std::string _message);

    /**
     * @brief Slot: pop failure notification
     * @param _message text of the notification
     */
    void popFailure(std::string _message);

    /**
     * @brief Queue the notification and display it (called by popInfo/Success/Failure Slot).
     * @param _message message to display.
     * @param _type type of the notification.
     */
    void showNotification(
        const std::string& _message,
        sight::ui::base::dialog::NotificationDialog::Type _type
    );

    /// Called when a notification is closed
    void onNotificationClosed(const sight::ui::base::dialog::NotificationDialog::sptr& notif);

    /// Max number of displayed notifications.
    std::uint8_t m_maxStackedNotifs {3};

    /// Duration of the notifications before closing (in ms).
    int m_durationInMs {3000};

    /// Set position once, all notifications of the app/config are displayed here.
    sight::ui::base::dialog::NotificationDialog::Position m_notificationsPosition
    {sight::ui::base::dialog::NotificationDialog::Position::TOP_RIGHT};

    /// Map to convert string position like "TOP_RIGHT" to NotificationDialog::Position.
    std::map<std::string, sight::ui::base::dialog::NotificationDialog::Position> m_positionMap;

    /// Default message (if message in slot are empty), the default message can be configured in xml.
    std::string m_defaultMessage = "Notification";

    /// Vector of displayed NotificationDialog, resized with "m_maxStackedNotifs" at start.
    std::list<sight::ui::base::dialog::NotificationDialog::sptr> m_popups {};

    /// fwContainer where notifications will be displayed in, nullptr by default.
    sight::ui::base::container::fwContainer::csptr m_containerWhereToDisplayNotifs {nullptr};

    /// Parent container ID (SID or WID), empty by default.
    std::string m_parentContainerID;
};

} //namespace sight::module::ui::qt
