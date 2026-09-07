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

#include "has_notifications.hpp"

#include <core/spy_log.hpp>

namespace sight::core::notification
{

//------------------------------------------------------------------------------

has_notifications::has_notifications(sight::core::com::signals& _signals) noexcept
{
    _signals(signals::NOTIFICATION_CREATED, m_notification_created_sig);
}

//------------------------------------------------------------------------------

void has_notifications::configure_notifications(const sight::core::runtime::config_t& _config)
{
    m_notification_ids.clear();

    for(const auto& [name, notification] : _config)
    {
        if(name != "notification")
        {
            continue;
        }

        const auto key = notification.get_optional<std::string>("<xmlattr>.key");
        const auto id  = notification.get_optional<std::string>("<xmlattr>.id");

        SIGHT_ERROR_IF(
            "A <notification> element needs both a 'key' and an 'id' attribute, it will be ignored.",
            !key.has_value() || !id.has_value()
        );

        if(key.has_value() && id.has_value())
        {
            m_notification_ids[*key] = *id;
        }
    }
}

//------------------------------------------------------------------------------

std::string has_notifications::notification_id(const std::string& _key) const
{
    // An unmapped key is the normal case for an application that does not merge this notification: it simply
    // stays unidentified, so no warning here.
    const auto& it = m_notification_ids.find(_key);

    return it == m_notification_ids.cend() ? std::string {} : it->second;
}

//------------------------------------------------------------------------------

} // namespace sight::core::notification
