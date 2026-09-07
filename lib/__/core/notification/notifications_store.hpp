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

#include <sight/core/config.hpp>

#include "message.hpp"

#include <memory>
#include <mutex>

namespace sight::core::notification
{

/**
 * @brief Tracks notifications and dispatches their lifecycle and content changes on the default worker.
 *
 * Notifications may be watched with or without ownership. State changes and message updates are forwarded to
 * the derived store on sight::core::thread::get_default_worker().
 */
class SIGHT_CORE_CLASS_API notifications_store
{
public:

    SIGHT_CORE_API virtual ~notifications_store() = default;

protected:

    using notification_t = base;
    using message_t      = message;

    /**
     * @brief Start observing a notification.
     *
     * Installs hooks for state changes and, for messages, content changes.
     *
     * @param _notification the notification to observe.
     * @param _own if true, the store keeps _notification alive with a shared_ptr. If false, the store only
     *             observes it weakly; the caller is responsible for keeping it alive.
     * @param _self weak handle to the owning object, used to guard asynchronous callbacks.
     */
    SIGHT_CORE_API void watch(
        const notification_t::sptr& _notification,
        bool _own,
        std::weak_ptr<void> _self
    );

    /**
     * @brief Stop observing and drop ownership of a notification.
     *
     * Existing hooks remain installed but become inactive until the notification is watched again.
     */
    SIGHT_CORE_API void forget(const notification_t::sptr& _notification);

    /// Snapshot of currently tracked, still-alive notifications. Opportunistically prunes expired entries.
    [[nodiscard]] SIGHT_CORE_API std::vector<notification_t::sptr> tracked_notifications() const;

    /// True if there are no tracked notifications, or every tracked notification is finished/canceled.
    [[nodiscard]] SIGHT_CORE_API bool all_finished() const;

    /**
     * @brief Called on the default worker when a tracked notification is canceled or finished.
     *
     * @param _notification The notification may already be expired when this is called.
     */
    virtual void on_notification_finished(const notification_t::wptr& _notification) = 0;

    /**
     * @brief Called on the default worker when a tracked message changes.
     */
    SIGHT_CORE_API virtual void on_notification_changed(const message_t::wptr& _notification);

private:

    /// True while the given notification is one of the entries this store observes.
    [[nodiscard]] bool is_tracked(const notification_t::sptr& _notification) const;

    struct entry final
    {
        /// Non-null if this store keeps the notification alive.
        notification_t::sptr m_owned;
        notification_t::wptr m_notification;
    };

    // m_mutex and m_entries are private implementation details, never exposed by reference/value across the
    // DLL boundary (tracked_notifications() returns a fresh copy), so it is safe to silence the MSVC
    // C4251 "needs to have dll-interface" warning here instead of trying to export std:: types.
#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif

    mutable std::recursive_mutex m_mutex;

    /// Mutable: tracked_notifications()/all_finished() opportunistically prune expired entries even though
    /// they are logically read-only operations from the caller's point of view.
    mutable std::vector<entry> m_entries;

    /// Notifications that already have hooks, including forgotten ones. Pruned by watch().
    mutable std::vector<notification_t::wptr> m_hooked;

#ifdef _MSC_VER
    #pragma warning(pop)
#endif
};

} // namespace sight::core::notification
