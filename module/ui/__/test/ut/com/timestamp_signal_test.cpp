/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include <core/com/slot.hpp>
#include <core/thread/worker.hpp>

#include <service/base.hpp>
#include <service/op.hpp>

#include <utest/wait.hpp>

#include <doctest/doctest.h>

#include <thread>
#include <vector>

namespace
{

struct timestamp_signal_fixture
{
    timestamp_signal_fixture()
    {
        m_timestamp_signal = sight::service::add("sight::module::ui::com::timestamp_signal");
        REQUIRE_MESSAGE(
            m_timestamp_signal,
            "Failed to create service 'sight::module::ui::com::timestamp_signal'"
        );
    }

    ~timestamp_signal_fixture()
    {
        if(m_worker)
        {
            m_worker->stop();
        }

        if(!m_timestamp_signal->stopped())
        {
            CHECK_NOTHROW(m_timestamp_signal->stop().get());
        }

        sight::service::remove(m_timestamp_signal);
    }

    timestamp_signal_fixture(const timestamp_signal_fixture&)            = delete;
    timestamp_signal_fixture& operator=(const timestamp_signal_fixture&) = delete;
    timestamp_signal_fixture(timestamp_signal_fixture&&)                 = delete;
    timestamp_signal_fixture& operator=(timestamp_signal_fixture&&)      = delete;

    //------------------------------------------------------------------------------

    void test(bool _use_system_clock)
    {
        using namespace std::literals::chrono_literals;

        boost::property_tree::ptree ptree;
        ptree.put("useSystemClock", _use_system_clock);
        m_timestamp_signal->set_config(ptree);

        CHECK_NOTHROW(m_timestamp_signal->configure());
        CHECK_NOTHROW(m_timestamp_signal->start().get());

        std::vector<double> timestamps;
        auto triggered_slot = sight::core::com::new_slot(
            [&timestamps](double _timestamp)
            {
                timestamps.push_back(_timestamp);
            });

        m_worker = sight::core::thread::worker::make();
        triggered_slot->set_worker(m_worker);
        m_timestamp_signal->signal("triggered")->connect(triggered_slot);

        CHECK_NOTHROW(m_timestamp_signal->update().get());
        SIGHT_TEST_WAIT(1 == timestamps.size());
        CHECK_EQ(std::size_t(1), timestamps.size());

        std::this_thread::sleep_for(1000ms);

        CHECK_NOTHROW(m_timestamp_signal->update().get());
        SIGHT_TEST_WAIT(2 == timestamps.size());
        REQUIRE_EQ(std::size_t(2), timestamps.size());

        CHECK(timestamps[1] - timestamps[0] >= 1);
    }

    sight::service::base::sptr m_timestamp_signal;
    sight::core::thread::worker::sptr m_worker;
};

} // namespace

TEST_SUITE("sight::module::ui::com::timestamp_signal")
{
    TEST_CASE_FIXTURE(timestamp_signal_fixture, "system_clock")
    {
        test(true);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(timestamp_signal_fixture, "high_res_clock")
    {
        test(false);
    }
}
