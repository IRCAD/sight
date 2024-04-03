/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

#include <sight/ui/__/config.hpp>

#include "ui/__/container/widget.hpp"
#include "ui/__/object.hpp"

#include <service/notifier.hpp>

#include <array>
#include <string>

namespace sight::ui::dialog
{

/**
 * @brief Defines the generic notification popup GUI.
 */
class SIGHT_UI_CLASS_API notification_base : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(notification_base, ui::object);

    /// Notification Type (changes Qss style).
    using type = enum sight::service::notification::type;

    /// Where to display notifications.
    using position = enum sight::service::notification::position;

    /// Constructor. Does nothing.
    SIGHT_UI_API notification_base();

    /// Destructor. Does nothing.
    SIGHT_UI_API ~notification_base() override = default;

    using factory_registry_key_t = std::string;
    SIGHT_UI_API static const factory_registry_key_t REGISTRY_KEY;

    /// Sets the message.
    /// @param _msg message as a std::string, it can be empty.
    SIGHT_UI_API virtual void set_message(std::string _msg);

    /**
     * @brief Sets the notification type.
     * @param _type notification type @see Type.
     */
    SIGHT_UI_API virtual void set_type(type _type);

    /**
     * @brief Sets the position.
     * @param _position notification position @see Position.
     */
    SIGHT_UI_API virtual void set_position(position _position);

    /**
     * @brief Sets the size
     * @param _size (width, height) of the notification in pixel.
     */
    SIGHT_UI_API virtual void set_size(std::array<int, 2> _size);
    SIGHT_UI_API virtual std::array<int, 2> size() const;

    /**
     * @brief Sets the queue index of the notification (when notifications are queued).
     * @param _index index in the queue (0 = first, 1 = second,...).
     */
    SIGHT_UI_API virtual void set_index(unsigned int _index);

    /**
     * @brief Sets the duration in ms.
     * @param _duration_in_ms duration of the notification before closing (+ 1 sec of fade in/out effects).
     *                      std::nullopt means infinite duration.
     */
    SIGHT_UI_API virtual void set_duration(std::optional<std::chrono::milliseconds> _duration_in_ms);
    SIGHT_UI_API virtual std::optional<std::chrono::milliseconds> get_duration() const;

    /// Set the channel property
    /// @param _channel the channel. Empty string for default global channel.
    SIGHT_UI_API virtual void set_channel(std::string _channel);
    SIGHT_UI_API virtual std::string get_channel() const;

    /// Set the closable property
    /// @param _closable true if closable. std::nullopt means finite duration is closable.
    SIGHT_UI_API virtual void set_closable(std::optional<bool> _closable);
    SIGHT_UI_API virtual std::optional<bool> is_closable() const;

    /// Set the notification attributes (type, message, duration...) in one step.
    /// @param _notification notification attributes.
    SIGHT_UI_API virtual void set_notification(sight::service::notification _notification);

    /// Shows the message box and return the clicked button.
    SIGHT_UI_API virtual void show() = 0;

    /// Gets the visibility, needs to be reimplemented.
    SIGHT_UI_API virtual bool is_visible() const = 0;

    /// Closes the notification, needs to be reimplemented.
    SIGHT_UI_API virtual void close() const = 0;

    /// Move the notification to the lower index
    SIGHT_UI_API virtual void move_down() = 0;

    /**
     * @brief Attaches a parent container to the notification.
     * @param _container the parent container, nullptr is handled.
     */
    SIGHT_UI_API virtual void set_container(ui::container::widget::csptr _container)
    {
        m_parent_container = _container;
    }

    /// Define the callback called when the dialog is closed
    SIGHT_UI_API virtual void set_closed_callback(std::function<void()> _f)
    {
        m_closed_call_back = _f;
    }

protected:

    /// Notification attributes (message, position, type, ...).
    sight::service::notification m_notification {};

    /// Position of the notification, used to avoid overlapping
    /// when several notifications are shown. (0 = first, 1 = second, ...)
    unsigned int m_index {0};

    /// Pointer to the parent container, default nullptr.
    ui::container::widget::csptr m_parent_container {nullptr};

    std::function<void()> m_closed_call_back;
};

} // namespace sight::ui::dialog
