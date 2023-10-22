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

#include "notifier.hpp"

#include <core/com/signal.hxx>

#include <boost/range/iterator_range_core.hpp>

namespace sight::service
{

notifier::notifier(core::com::signals& _signals) noexcept
{
    _signals(signals::NOTIFIED, M_NOTIFIED_SIG);
    _signals(signals::NOTIFICATION_CLOSED, M_NOTIFICATION_CLOSED_SIG);
}

//------------------------------------------------------------------------------

void notifier::notify(Notification _notification) const
{
    SIGHT_FATAL_IF("channel '" + _notification.channel + "' not found.", !m_channels.contains(_notification.channel));
    _notification.channel = m_channels.at(_notification.channel);
    M_NOTIFIED_SIG->async_emit(std::move(_notification));
}

//------------------------------------------------------------------------------

void notifier::closeNotification(std::string _channel) const
{
    SIGHT_FATAL_IF("channel '" + _channel + "' not found.", !m_channels.contains(_channel));
    _channel = m_channels.at(_channel);
    M_NOTIFICATION_CLOSED_SIG->async_emit(std::move(_channel));
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
