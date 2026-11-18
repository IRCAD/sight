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

#include <data/string_serializable.hpp>

#include <service/base.hpp>
#include <service/op.hpp>

#include <ui/__/preferences.hpp>

#include <utest/wait.hpp>

#include <doctest/doctest.h>

#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <ostream>
#include <string>
#include <thread>

namespace
{

/// A value whose write can be held while it runs, once armed, for the test to act in the middle of it.
class held_value final : public sight::data::string_serializable
{
public:

    SIGHT_DECLARE_CLASS(held_value, sight::data::string_serializable);

    //------------------------------------------------------------------------------

    [[nodiscard]] std::string to_string() const override
    {
        // What is written is the value when the write reads it, whatever comes during the write.
        std::unique_lock lock(m_mutex);
        std::string value = m_value;
        if(m_armed)
        {
            m_armed   = false;
            m_entered = true;
            m_changed.notify_all();
            m_changed.wait_for(lock, std::chrono::seconds(30), [this]{return m_released;});
        }

        return value;
    }

    //------------------------------------------------------------------------------

    void from_string(const std::string& _value) override
    {
        const std::scoped_lock lock(m_mutex);
        m_value = _value;
    }

    //------------------------------------------------------------------------------

    void reset() override
    {
    }

    //------------------------------------------------------------------------------

    void set_default_value() override
    {
    }

    //------------------------------------------------------------------------------

    void arm()
    {
        const std::scoped_lock lock(m_mutex);
        m_armed = true;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool wait_until_entered()
    {
        std::unique_lock lock(m_mutex);
        return m_changed.wait_for(lock, std::chrono::seconds(30), [this]{return m_entered;});
    }

    //------------------------------------------------------------------------------

    void release()
    {
        const std::scoped_lock lock(m_mutex);
        m_released = true;
        m_changed.notify_all();
    }

private:

    //------------------------------------------------------------------------------

    std::ostream& to_ostream(std::ostream& _os) const override
    {
        return _os << m_value;
    }

    mutable std::mutex m_mutex;
    mutable std::condition_variable m_changed;
    mutable bool m_armed {false};
    mutable bool m_entered {false};
    bool m_released {false};
    std::string m_value;
};

} // namespace

TEST_SUITE("sight::module::ui::preferences")
{
    // What the test checks is the decision whether to write at all. Each update() starts a timer, and only the timer
    // whose id equals m_last_timer_id writes. In the scenario under test:
    // 1. write A is running;
    // 2. the user changes the value, which creates timer B;
    // 3. A finishes and resets m_last_timer_id to -1.
    // Whether "second" ends up written then depends on that bookkeeping and on the order in which A and B take the
    // timers lock.
    TEST_CASE("writes_a_value_modified_during_a_write")
    {
        sight::ui::preferences::set_enabled(true);
        sight::ui::preferences::ignore_filesystem(true);

        const auto value = std::make_shared<held_value>();
        value->from_string("first");

        auto service = sight::service::add("sight::module::ui::preferences");
        REQUIRE(service);
        service->set_inout(value, "keys", true, false, 0);
        service->configure();
        service->start().get();

        // The timer of this update writes after a second, held in the middle.
        value->arm();
        service->update().get();
        REQUIRE(value->wait_until_entered());

        // A modification during the write has a timer of its own, which must write it. Its update waits for the write
        // to end: it has started once its status says so, and the pause lets it reach the lock of the timers, which
        // nothing shows. Either order at the end of the write must write the value.
        value->from_string("second");
        const auto second_update = service->update();
        SIGHT_TEST_WAIT(service->updating_status() == sight::service::base::updating_status::updating, 30000);
        CHECK(service->updating_status() == sight::service::base::updating_status::updating);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        value->release();
        second_update.get();

        const std::string key = value->base_id();
        SIGHT_TEST_WAIT(
            sight::ui::preferences().get_optional<std::string>(key).value_or("") == "second",
            30000
        );
        CHECK_EQ(sight::ui::preferences().get_optional<std::string>(key).value_or(""), "second");

        service->stop().get();
        sight::service::remove(service);
        sight::ui::preferences::ignore_filesystem(false);
        sight::ui::preferences::set_enabled(false);
    }

    //------------------------------------------------------------------------------

    TEST_CASE("writes_at_stop_a_value_modified_during_a_write")
    {
        sight::ui::preferences::set_enabled(true);
        sight::ui::preferences::ignore_filesystem(true);

        const auto value = std::make_shared<held_value>();
        value->from_string("first");

        auto service = sight::service::add("sight::module::ui::preferences");
        REQUIRE(service);
        service->set_inout(value, "keys", true, false, 0);
        service->configure();
        service->start().get();

        value->arm();
        service->update().get();
        REQUIRE(value->wait_until_entered());

        // As above, then the application stops at once: the timer of the modification must still write it.
        value->from_string("second");
        const auto second_update = service->update();
        SIGHT_TEST_WAIT(service->updating_status() == sight::service::base::updating_status::updating, 30000);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        value->release();
        second_update.get();
        service->stop().get();

        const std::string key = value->base_id();
        CHECK_EQ(sight::ui::preferences().get_optional<std::string>(key).value_or(""), "second");

        sight::service::remove(service);
        sight::ui::preferences::ignore_filesystem(false);
        sight::ui::preferences::set_enabled(false);
    }

    //------------------------------------------------------------------------------

    TEST_CASE("writes_at_stop_a_pending_modification")
    {
        sight::ui::preferences::set_enabled(true);
        sight::ui::preferences::ignore_filesystem(true);

        const auto value = std::make_shared<held_value>();
        value->from_string("first");

        auto service = sight::service::add("sight::module::ui::preferences");
        REQUIRE(service);
        service->set_inout(value, "keys", true, false, 0);
        service->configure();
        service->start().get();

        // The timer of this update writes after a second, and the stop comes at once: if the timer has not fired yet,
        // the stop writes the value itself, else the timer has. Either way, the value is written.
        value->from_string("second");
        service->update().get();
        service->stop().get();

        const std::string key = value->base_id();
        CHECK_EQ(sight::ui::preferences().get_optional<std::string>(key).value_or(""), "second");

        sight::service::remove(service);
        sight::ui::preferences::ignore_filesystem(false);
        sight::ui::preferences::set_enabled(false);
    }
} // TEST_SUITE
