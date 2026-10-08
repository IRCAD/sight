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

#include <core/notification/information.hpp>
#include <core/notification/notifications_store.hpp>

#include <utest/wait.hpp>

#include <doctest/doctest.h>

#include <atomic>
#include <memory>
#include <string>

namespace
{

/// Minimal store used to count forwarded callbacks.
class counting_store final : public sight::core::notification::notifications_store,
                             public std::enable_shared_from_this<counting_store>
{
public:

    //------------------------------------------------------------------------------

    void observe(const sight::core::notification::base::sptr& _notification)
    {
        this->watch(_notification, /*_own=*/ true, this->weak_from_this());
    }

    //------------------------------------------------------------------------------

    void drop(const sight::core::notification::base::sptr& _notification)
    {
        this->forget(_notification);
    }

    std::atomic_int m_changed {0};
    std::atomic_int m_finished {0};

protected:

    //------------------------------------------------------------------------------

    void on_notification_finished(const sight::core::notification::base::wptr& /*_notification*/) final
    {
        ++m_finished;
    }

    //------------------------------------------------------------------------------

    void on_notification_changed(const sight::core::notification::message::wptr& /*_notification*/) final
    {
        ++m_changed;
    }
};

} // namespace

TEST_SUITE("sight::core::notification::notifications_store")
{
    TEST_CASE("a_watched_notification_reports_its_changes")
    {
        const auto store        = std::make_shared<counting_store>();
        const auto notification = std::make_shared<sight::core::notification::information>("", "before");

        store->observe(notification);

        notification->set_text("after");

        SIGHT_TEST_WAIT(store->m_changed == 1);
        CHECK_EQ(store->m_changed, 1);
    }

//------------------------------------------------------------------------------

    TEST_CASE("a_forgotten_notification_stops_reporting")
    {
        const auto store        = std::make_shared<counting_store>();
        const auto notification = std::make_shared<sight::core::notification::information>("", "before");

        store->observe(notification);
        store->drop(notification);

        // Hooks remain installed but must be inactive after forget().
        notification->set_text("after");
        notification->finish();

        SIGHT_TEST_WAIT(store->m_changed > 0 || store->m_finished > 0);
        CHECK_EQ(store->m_changed, 0);
        CHECK_EQ(store->m_finished, 0);
    }

//------------------------------------------------------------------------------

    TEST_CASE("watching_again_after_a_forget_does_not_double_the_hooks")
    {
        const auto store        = std::make_shared<counting_store>();
        const auto notification = std::make_shared<sight::core::notification::information>("", "before");

        // Rewatching a message after its page was forgotten must reuse its hooks.
        store->observe(notification);
        store->drop(notification);
        store->observe(notification);

        notification->set_text("after");

        // The hooks must fire once, not once per watch(): wait for a second call that must never come.
        SIGHT_TEST_WAIT(store->m_changed > 1);
        CHECK_EQ(store->m_changed, 1);

        notification->finish();

        SIGHT_TEST_WAIT(store->m_finished > 1);
        CHECK_EQ(store->m_finished, 1);
    }
}
