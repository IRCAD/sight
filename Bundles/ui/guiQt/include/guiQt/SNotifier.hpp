/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
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

#include "guiQt/config.hpp"

#include <fwGui/dialog/NotificationDialog.hpp>

#include <fwServices/IController.hpp>

#include <boost/circular_buffer.hpp>

namespace guiQt
{
/**
 * @brief SNotifier is a general service to display notification in a centralized way.
 * SNotifier needs to be connected to notifySuccess/Failure/Info signals implemented in IService.
 *
 * @section Slots Slots
 * - \b popInfo() : Adds an INFO popup in the queue & display it.
 * - \b popFailure() : Adds a FAILURE popup in the queue & display it.
 * - \b popSuccess() :Adds a SUCCESS popup in the queue & display it.
 * - \b setEnumParameter() : Use to change the position of notifications (key "position"),
 * accepted values are the same than the "position" tag in the XML configuration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::guiQt::SNotifier">
            <message>Default Message</message>
            <maxNotifications>3</maxNotifications>
            <position>TOP_RIGHT</position>
            <duration>3000</duration>
        </service>
   @endcode
 *
 * @subsection Configuration Configuration
 * - \b message (optional) : default message of the notification is the emited signal contains empty string (default:
 *"Notification").
 * - \b maxNotifications (optional): max number of queued notifications (default 3).
 * - \b position (optional) : position of the notification queue (default: TOP_RIGHT).
 *  accepted values are:
 *   - TOP_RIGHT(default).
 *   - TOP_LEFT
 *   - CENTERED_TOP
 *   - CENTERED: when choosing CENTERED, only ONE notification can be displayed at time (maxNotifications is ignored).
 *   - BOTTOM_RIGHT
 *   - BOTTOM_LEFT
 *   - CENTERED_BOTTOM.
 *
 * - \b duration (optional) : duration in ms of the notification (+ 1 sec for fadein & fadeout effects) (default 3000).
 */
class GUIQT_CLASS_API SNotifier : public ::fwServices::IController
{
public:

    fwCoreServiceMacro(SNotifier, ::fwServices::IController)

    /// Constructor. Do nothing.
    GUIQT_API SNotifier() noexcept;

    /// Destructor. Do nothing.
    GUIQT_API virtual ~SNotifier() noexcept override;

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /// This method configures the service
    GUIQT_API virtual void configuring() override;

    /**
     * @brief This method enables the eventFilter
     */
    GUIQT_API virtual void starting() override;

    /**
     * @brief This method deletes the eventFilter
     */
    GUIQT_API virtual void stopping() override;

    /**
     * @brief This method does nothing.
     */
    GUIQT_API virtual void updating() override;

private:

    /// Slot: This method is used to set an enum parameter.
    void setEnumParameter(std::string _val, std::string _key);

    /**
     * @brief Slot pop info notification
     * @param _message: text of the notification
     */
    void popInfo(std::string _message);

    /**
     * @brief Slot pop info notification
     * @param _message: text of the notification
     */
    void popSuccess(std::string _message);

    /**
     * @brief Slot pop info notification
     * @param _message: text of the notification
     */
    void popFailure(std::string _message);

    /**
     * @brief Queue the notification and display it (called by popInfo/Success/Failure Slot).
     * @param _message: message to display.
     * @param _type: type of the notification.
     */
    void showNotification(const std::string& _message, ::fwGui::dialog::NotificationDialog::Type _type);

    /// Max number of displayed notifications.
    std::uint8_t m_maxStackedNotifs {3};

    /// Duration of the notifications before closing (in ms).
    int m_durationInMs {3000};

    /// Set position once, all notifications of the app/config are displayed here.
    ::fwGui::dialog::NotificationDialog::Position m_notifcationsPosition
    {::fwGui::dialog::NotificationDialog::Position::TOP_RIGHT};

    /// Map to convert string position like "TOP_RIGHT" to NotificationDialog::Position.
    std::map< std::string, ::fwGui::dialog::NotificationDialog::Position> m_positionMap;

    /// Default message (if message in slot are empty), the default message can be configured in xml.
    std::string m_defaultMessage = "Notification";

    /// Vector of displayed NotificationDialog
    std::vector< ::fwGui::dialog::NotificationDialog::sptr > m_popups {};
    /// Queue of index in m_popups to remove oldest if m_maxStackedNotifs is reached.
    std::queue< size_t > m_indexQueue;

};

} //namespace guiQt
