/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "timestamp_signal_test.hpp"

#include <core/com/slot.hxx>

#include <service/op.hpp>

#include <utest/wait.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::com::ut::timestamp_signal_test);

namespace sight::module::ui::com::ut
{

//------------------------------------------------------------------------------

void timestamp_signal_test::setUp()
{
    m_timestampSignal = service::add("sight::module::ui::com::timestamp_signal");
    CPPUNIT_ASSERT_MESSAGE(
        "Failed to create service 'sight::module::ui::com::timestamp_signal'",
        m_timestampSignal
    );
}

//------------------------------------------------------------------------------

void timestamp_signal_test::tearDown()
{
    m_worker->stop();
    if(!m_timestampSignal->stopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_timestampSignal->stop().get());
    }

    service::remove(m_timestampSignal);
}

//------------------------------------------------------------------------------

void timestamp_signal_test::test(bool _use_system_clock)
{
    using namespace std::literals::chrono_literals;

    boost::property_tree::ptree ptree;
    ptree.put("useSystemClock", _use_system_clock);
    m_timestampSignal->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_timestampSignal->configure());
    CPPUNIT_ASSERT_NO_THROW(m_timestampSignal->start().get());

    std::vector<double> timestamps;
    auto triggered_slot = core::com::new_slot(
        [&timestamps](double _timestamp)
        {
            timestamps.push_back(_timestamp);
        });
    m_worker = core::thread::worker::make();
    triggered_slot->set_worker(m_worker);
    m_timestampSignal->signal("triggered")->connect(triggered_slot);

    CPPUNIT_ASSERT_NO_THROW(m_timestampSignal->update().get());
    SIGHT_TEST_WAIT(1 == timestamps.size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), timestamps.size());

    std::this_thread::sleep_for(1000ms);

    CPPUNIT_ASSERT_NO_THROW(m_timestampSignal->update().get());
    SIGHT_TEST_WAIT(2 == timestamps.size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), timestamps.size());

    CPPUNIT_ASSERT(timestamps[1] - timestamps[0] >= 1);
}

//------------------------------------------------------------------------------

void timestamp_signal_test::systemClockTest()
{
    test(true);
}

//------------------------------------------------------------------------------

void timestamp_signal_test::highResClockTest()
{
    test(false);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::com::ut
