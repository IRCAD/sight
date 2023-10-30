/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "service/base.hpp"
#include "service/config.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>

#include <array>
#include <chrono>
#include <optional>

namespace sight::service
{

/// Defines Notification type
struct SERVICE_CLASS_API notification final
{
    // Notification Type (changes Qss style).
    enum type
    {
        info = 0,
        success,
        failure
    } type {type::info};

    /// Where to display notifications.
    enum position
    {
        top_right = 0,
        top_left,
        bottom_right,
        bottom_left,
        centered,
        centered_top,
        centered_bottom
    } position {position::top_right};

    std::string message {};
    std::optional<std::chrono::milliseconds> duration {std::chrono::seconds(3)};
    std::string channel {};
    std::optional<bool> closable {std::nullopt};
    std::array<int, 2> size {200, 60};
};

/**
 * Interface for services that need to display notifications to user.
 *
 * @section Signals Signals
 * - \b notified(Notification notification): Emitted when the service wants to pop a notification.
 * - \b notificationClosed(std::string channel): Emitted when the service wants to close a notification.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="...">
    <notification>
        <channel key="key1" uid="INFO" />
        <channel key="key2" uid="${ERROR_CHANNEL}" />
    </notification>
   </service>
   @endcode
 *
 * @subsection Configuration Configuration
 * - \b notification (optional): Specific notification configuration
 *   - \b channel (optional): Specific channel configuration
 *     - \b key: The key used in the service to map the uid.
 *     - \b uid: The uid that identify the channel. All notification request that use the same channel, will share the
 *               same notification dialog widget
 */
class SERVICE_CLASS_API notifier
{
public:

    SERVICE_API notifier(core::com::signals& _signals) noexcept;
    virtual ~notifier() noexcept = default;

    /// Defines signals for notifier
    struct SERVICE_CLASS_API signals final
    {
        using notification_t = core::com::signal<void (notification)>;
        static inline const core::com::signals::key_t NOTIFIED = "notified";

        using channel_t = core::com::signal<void (std::string)>;
        static inline const core::com::signals::key_t NOTIFICATION_CLOSED = "notificationClosed";
    };

protected:

    /// Emits notification signal
    /// @param[in] notification
    /// @{
    SERVICE_API void notify(notification _notification) const;
    inline void notify(enum notification::type _type, std::string _message, std::string _channel = "") const;
    inline void info(std::string _message, std::string _channel                                  = "") const;
    inline void success(std::string _message, std::string _channel                               = "") const;
    inline void failure(std::string _message, std::string _channel                               = "") const;
    /// @}

    /// Emits close channel signal
    SERVICE_API void close_notification(std::string _channel) const;

    /// Method to call to configure notification "channels"
    SERVICE_API void initialize(const service::config_t& _config);

    /// Signal emitted when notify() is called
    const signals::notification_t::sptr m_notified_sig {std::make_shared<signals::notification_t>()};

    /// Signal emitted when closeChannel() is called
    const signals::channel_t::sptr m_notification_closed_sig {std::make_shared<signals::channel_t>()};

private:

    std::map<std::string, std::string> m_channels {{std::string(""), std::string("")}};
};

//------------------------------------------------------------------------------

inline void notifier::notify(enum notification::type _type, std::string _message, std::string _channel) const
{
    this->notify({.type = std::move(_type), .message = std::move(_message), .channel = std::move(_channel)});
}

//------------------------------------------------------------------------------

inline void notifier::info(std::string _message, std::string _channel) const
{
    this->notify(notification::type::info, std::move(_message), std::move(_channel));
}

//------------------------------------------------------------------------------

inline void notifier::success(std::string _message, std::string _channel) const
{
    this->notify(notification::type::success, std::move(_message), std::move(_channel));
}

//------------------------------------------------------------------------------

inline void notifier::failure(std::string _message, std::string _channel) const
{
    this->notify(notification::type::failure, std::move(_message), std::move(_channel));
}

} // namespace sight::service
