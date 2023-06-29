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

#include "INotifier.hpp"

#include <core/com/Signal.hxx>

#include <boost/range/iterator_range_core.hpp>

namespace sight::service
{

INotifier::INotifier(core::com::Signals& _signals) noexcept
{
    _signals(signals::s_NOTIFIED, m_notified_sig);
    _signals(signals::s_NOTIFICATION_CLOSED, m_notification_closed_sig);
}

//------------------------------------------------------------------------------

void INotifier::notify(Notification notification) const
{
    SIGHT_FATAL_IF("channel '" + notification.channel + "' not found.", !m_channels.contains(notification.channel));
    notification.channel = m_channels.at(notification.channel);
    m_notified_sig->asyncEmit(std::move(notification));
}

//------------------------------------------------------------------------------

void INotifier::closeNotification(std::string channel) const
{
    SIGHT_FATAL_IF("channel '" + channel + "' not found.", !m_channels.contains(channel));
    channel = m_channels.at(channel);
    m_notification_closed_sig->asyncEmit(std::move(channel));
}

//------------------------------------------------------------------------------

void INotifier::initialize(const IService::config_t& config)
{
    if(const auto& notification = config.get_child_optional("notification"); notification)
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
