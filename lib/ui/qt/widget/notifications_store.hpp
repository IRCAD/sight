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

#include <sight/ui/qt/config.hpp>

#include <core/notification/message.hpp>

#include <memory>
#include <mutex>

namespace sight::ui::qt::widget
{

/**
 * @brief Base class tracking a set of sight::core::notification::base objects and marshalling their
 * lifecycle/content-change hooks onto the UI thread, so that subclasses can safely update the Qt widgets
 * associated with them.
 *
 * A notification can be watched either "owned" (the store keeps it alive with a shared_ptr, used when
 * nothing else is expected to hold a reference, e.g. plain text notifications) or weakly (the store only
 * observes it, relying on the emitter to keep it alive, e.g. progress monitors). In both cases, once the
 * notification reaches the "canceled" or "finished" state, on_notification_finished() is invoked, already
 * marshalled onto sight::core::thread::get_default_worker(). Message-derived notifications additionally
 * fire on_notification_changed() whenever their content is updated via
 * sight::core::notification::message::set_title()/set_text().
 *
 * This lets a service that kept the sptr returned by e.g. has_notifications::warn()/fail() or
 * has_monitors::observe() manipulate its own notification's widget later on (close it early, update its
 * text) simply by calling finish()/cancel()/set_text()/set_title() on the notification object, from
 * whatever thread it runs on.
 */
class SIGHT_UI_QT_CLASS_API_QT notifications_store
{
public:

    SIGHT_UI_QT_API_QT virtual ~notifications_store() = default;

protected:

    using notification_t = sight::core::notification::base;
    using message_t      = sight::core::notification::message;

    /**
     * @brief Start observing a notification.
     *
     * Installs a state hook (marshalled onto the default worker) that calls on_notification_finished() once
     * the notification reaches the canceled/finished state, and, for message-derived notifications, a
     * change hook that calls on_notification_changed().
     *
     * @param _notification the notification to observe.
     * @param _own if true, the store keeps _notification alive with a shared_ptr. If false, the store only
     *             observes it weakly; the caller is responsible for keeping it alive.
     * @param _self a type-erased weak handle to the owning (derived) object - typically
     *              std::weak_ptr<void>(this->weak_from_this()) - used to guard the marshalled callbacks
     *              against the owner having been destroyed in the meantime. Must be provided fresh at each
     *              call (it cannot be cached, since it is derived from a shared_ptr that may not exist yet
     *              at construction time).
     */
    SIGHT_UI_QT_API_QT void watch(
        const notification_t::sptr& _notification,
        bool _own,
        std::weak_ptr<void> _self
    );

    /// Stop observing / drop ownership of a notification, e.g. when its associated widget is torn down.
    SIGHT_UI_QT_API_QT void forget(const notification_t::sptr& _notification);

    /// Snapshot of currently tracked, still-alive notifications. Opportunistically prunes expired entries.
    [[nodiscard]] SIGHT_UI_QT_API_QT std::vector<notification_t::sptr> tracked_notifications() const;

    /// True if there are no tracked notifications, or every tracked notification is finished/canceled.
    [[nodiscard]] SIGHT_UI_QT_API_QT bool all_finished() const;

    /**
     * @brief Called, already marshalled onto the UI thread, when a tracked notification becomes canceled/
     * finished.
     *
     * @param _notification the notification that finished. May already be expired (e.g. a weakly-tracked
     * monitor whose last owning sptr was dropped as part of the very same finish()/cancel() call that
     * triggered this callback) - implementations that need the instance itself must lock() it and handle
     * that case; implementations that only need to know "something finished, refresh your own state" (e.g.
     * by calling tracked_notifications() again) can ignore it entirely.
     */
    virtual void on_notification_finished(const notification_t::wptr& _notification) = 0;

    /**
     * @brief Called, already marshalled onto the UI thread, when a tracked message's displayed content
     * changes. Default implementation does nothing. @see on_notification_finished for why this is a wptr.
     */
    SIGHT_UI_QT_API_QT virtual void on_notification_changed(const message_t::wptr& _notification);

private:

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

#ifdef _MSC_VER
    #pragma warning(pop)
#endif
};

} // namespace sight::ui::qt::widget
