/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <core/notification/has_notifications.hpp>
#include <service/controller.hpp>

#include <vector>

namespace ex_notification_zone
{

class notification_sender final : public sight::service::controller,
                                  private sight::core::notification::has_notifications
{
public:

    SIGHT_DECLARE_SERVICE(notification_sender, sight::service::controller);

    notification_sender();
    ~notification_sender() noexcept final = default;

    struct slots final
    {
        static inline const slot_key_t INSTRUCTION  = "instruction";
        static inline const slot_key_t INFORMATION  = "information";
        static inline const slot_key_t WARNING      = "warning";
        static inline const slot_key_t ERROR        = "error";
        static inline const slot_key_t CLOSE_ERRORS = "close_errors";
    };

    // slot called to send a notification (example stub)
    void instruction();
    void information();
    void warning();
    void error();

    /// Slot closing every error notification sent by error() and still kept in m_errors, via cancel().
    void close_errors();

protected:

    //------------------------------------------------------------------------------

    void configuring() final
    {
    }

    //------------------------------------------------------------------------------

    void starting() final
    {
    }

    //------------------------------------------------------------------------------

    void updating() final
    {
    }

    //------------------------------------------------------------------------------

    void stopping() final
    {
    }

    //------------------------------------------------------------------------------

    void swapping(std::string_view /*unused*/) final
    {
    }

private:

    /// Error notifications sent by error() and not yet closed by close_errors().
    std::vector<sight::core::notification::error::sptr> m_errors;
};

} // namespace ex_notification_zone
