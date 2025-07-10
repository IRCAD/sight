/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include "notifier.hpp"

#include <core/com/signal.hxx>

#include <boost/range/iterator_range_core.hpp>

namespace sight::service
{

notifier::notifier(core::com::signals& _signals) noexcept
{
    _signals(signals::NOTIFIED, m_notified_sig);
    _signals(signals::NOTIFICATION_CLOSED, m_notification_closed_sig);
}

//------------------------------------------------------------------------------

void notifier::notify(notification _notification) const
{
    SIGHT_FATAL_IF(
        "channel '" + _notification.m_channel + "' not found.",
        !m_channels.contains(_notification.m_channel)
    );

    _notification.m_channel = m_channels.at(_notification.m_channel);
    m_notified_sig->async_emit(std::move(_notification));
}

//------------------------------------------------------------------------------

void notifier::close_notification(std::string _channel) const
{
    SIGHT_FATAL_IF("channel '" + _channel + "' not found.", !m_channels.contains(_channel));
    _channel = m_channels.at(_channel);
    m_notification_closed_sig->async_emit(std::move(_channel));
}

//------------------------------------------------------------------------------

void notifier::initialize(const service::config_t& _config)
{
    if(const auto& notification = _config.get_child_optional("notification"); notification)
    {
        for(const auto& channel : boost::make_iterator_range(notification->equal_range("channel")))
        {
            const auto& key = channel.second.get<std::string>("<xmlattr>.key");
            const auto& uid = channel.second.get<std::string>("<xmlattr>.uid");
            m_channels.insert_or_assign(key, uid);
        }
    }
}

} // namespace sight::service
