/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "ui/base/config.hpp"
#include "ui/base/container/fwContainer.hpp"
#include "ui/base/GuiBaseObject.hpp"

#include <service/INotifier.hpp>

#include <array>
#include <string>

namespace sight::ui::base::dialog
{

/**
 * @brief Defines the generic notification popup GUI.
 */
class UI_BASE_CLASS_API INotificationDialog : public ui::base::GuiBaseObject
{
public:

    SIGHT_DECLARE_CLASS(INotificationDialog, ui::base::GuiBaseObject);

    /// Notification Type (changes Qss style).
    using Type = service::Notification::Type;

    /// Where to display notifications.
    using Position = service::Notification::Position;

    /// Constructor. Does nothing.
    UI_BASE_API INotificationDialog();

    /// Destructor. Does nothing.
    UI_BASE_API ~INotificationDialog() override = default;

    typedef std::string FactoryRegistryKeyType;
    UI_BASE_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /// Sets the message.
    /// @param _msg message as a std::string, it can be empty.
    UI_BASE_API virtual void setMessage(std::string _msg);

    /**
     * @brief Sets the notification type.
     * @param _type notification type @see Type.
     */
    UI_BASE_API virtual void setType(Type _type);

    /**
     * @brief Sets the position.
     * @param _position notification position @see Position.
     */
    UI_BASE_API virtual void setPosition(Position _position);

    /**
     * @brief Sets the size
     * @param _width width of the notification in pixel.
     * @param _height height of the notification in pixel.
     */
    UI_BASE_API virtual void setSize(std::array<int, 2> _size);
    UI_BASE_API virtual std::array<int, 2> getSize() const;

    /**
     * @brief Sets the queue index of the notification (when notifications are queued).
     * @param _index index in the queue (0 = first, 1 = second,...).
     */
    UI_BASE_API virtual void setIndex(unsigned int _index);

    /**
     * @brief Sets the duration in ms.
     * @param _durationInMs duration of the notification before closing (+ 1 sec of fade in/out effects).
     *                      std::nullopt means infinite duration.
     */
    UI_BASE_API virtual void setDuration(std::optional<std::chrono::milliseconds> _durationInMs);
    UI_BASE_API virtual std::optional<std::chrono::milliseconds> getDuration() const;

    /// Set the channel property
    /// @param _channel the channel. Empty string for default global channel.
    UI_BASE_API virtual void setChannel(std::string);
    UI_BASE_API virtual std::string getChannel() const;

    /// Set the closable property
    /// @param _closable true if closable. std::nullopt means finite duration is closable.
    UI_BASE_API virtual void setClosable(std::optional<bool> _closable);
    UI_BASE_API virtual std::optional<bool> isClosable() const;

    /// Set the notification attributes (type, message, duration...) in one step.
    /// @param _notification notification attributes.
    UI_BASE_API virtual void setNotification(service::Notification _notification);

    /// Shows the message box and return the clicked button.
    UI_BASE_API virtual void show() = 0;

    /// Gets the visibility, needs to be reimplemented.
    UI_BASE_API virtual bool isVisible() const = 0;

    /// Closes the notification, needs to be reimplemented.
    UI_BASE_API virtual void close() const = 0;

    /// Move the notification to the lower index
    UI_BASE_API virtual void moveDown() = 0;

    /**
     * @brief Attaches a parent container to the notification.
     * @param _container the parent container, nullptr is handled.
     */
    UI_BASE_API virtual void setContainer(ui::base::container::fwContainer::csptr _container)
    {
        m_parentContainer = _container;
    }

    /// Define the callback called when the dialog is closed
    UI_BASE_API virtual void setClosedCallback(std::function<void()> f)
    {
        m_closedCallBack = f;
    }

protected:

    /// Notification attributes (message, position, type, ...).
    service::Notification m_notification {};

    /// Position of the notification, used to avoid overlapping
    /// when several notifications are shown. (0 = first, 1 = second, ...)
    unsigned int m_index {0};

    /// Pointer to the parent container, default nullptr.
    ui::base::container::fwContainer::csptr m_parentContainer {nullptr};

    std::function<void()> m_closedCallBack;
};

} // namespace sight::ui::base::dialog
