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

#include "notifications_store.hpp"

#include <core/thread/worker.hpp>

#include <algorithm>

namespace sight::ui::qt::widget
{

//------------------------------------------------------------------------------

void notifications_store::watch(
    const notification_t::sptr& _notification,
    bool _own,
    std::weak_ptr<void> _self
)
{
    if(!_notification)
    {
        return;
    }

    {
        std::scoped_lock lock(m_mutex);

        const bool already_tracked = std::ranges::any_of(
            m_entries,
            [&_notification](const auto& _entry)
            {
                return _entry.m_notification.lock() == _notification;
            });

        if(already_tracked)
        {
            return;
        }

        m_entries.push_back({.m_owned = _own ? _notification : nullptr, .m_notification = _notification});
    }

    const notification_t::wptr weak_notification = _notification;

    _notification->add_state_hook(
        [this, _self, weak_notification](notification_t::state_t _state)
        {
            if(_state != notification_t::canceled && _state != notification_t::finished)
            {
                return;
            }

            core::thread::get_default_worker()->post_task<void>(
                [this, _self, weak_notification]
            {
                // Only guard on the store's own liveness. The notification itself may already be
                // expired: e.g. a weakly-tracked monitor whose destructor calls finish() after the
                // caller dropped its last owning sptr - the state hook still fires (synchronously, as
                // part of that same destructor), but by the time this posted task runs the weak_ptr may
                // no longer lock. on_notification_finished() must tolerate that.
                if(const auto self = _self.lock(); self)
                {
                    this->on_notification_finished(weak_notification);
                }
            });
        });

    if(const auto message = std::dynamic_pointer_cast<message_t>(_notification); message)
    {
        const message_t::wptr weak_message = message;

        message->add_change_hook(
            [this, _self, weak_message]
            {
                core::thread::get_default_worker()->post_task<void>(
                    [this, _self, weak_message]
                {
                    if(const auto self = _self.lock(); self)
                    {
                        this->on_notification_changed(weak_message);
                    }
                });
            });
    }
}

//------------------------------------------------------------------------------

void notifications_store::forget(const notification_t::sptr& _notification)
{
    std::scoped_lock lock(m_mutex);

    std::erase_if(
        m_entries,
        [&_notification](const auto& _entry)
        {
            const auto notification = _entry.m_notification.lock();
            return !notification || notification == _notification;
        });
}

//------------------------------------------------------------------------------

std::vector<notifications_store::notification_t::sptr> notifications_store::tracked_notifications() const
{
    std::scoped_lock lock(m_mutex);

    std::erase_if(
        m_entries,
        [](const auto& _entry)
        {
            return _entry.m_notification.expired();
        });

    std::vector<notification_t::sptr> notifications;
    notifications.reserve(m_entries.size());

    for(const auto& entry : m_entries)
    {
        notifications.push_back(entry.m_notification.lock());
    }

    return notifications;
}

//------------------------------------------------------------------------------

bool notifications_store::all_finished() const
{
    const auto notifications = this->tracked_notifications();

    return std::ranges::all_of(
        notifications,
        [](const auto& _notification)
        {
            return _notification->is_finished();
        });
}

//------------------------------------------------------------------------------

void notifications_store::on_notification_changed(const message_t::wptr& /*_notification*/)
{
}

} // namespace sight::ui::qt::widget
