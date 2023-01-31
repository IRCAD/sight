/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "STimestampSignalTest.hpp"

#include <core/com/Slot.hxx>

#include <service/base.hpp>

#include <utest/wait.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::base::com::ut::STimestampSignalTest);

namespace sight::module::ui::base::com::ut
{

//------------------------------------------------------------------------------

void STimestampSignalTest::setUp()
{
    m_timestampSignal = service::add("sight::module::ui::base::com::STimestampSignal");
    CPPUNIT_ASSERT_MESSAGE(
        "Failed to create service 'sight::module::ui::base::com::STimestampSignal'",
        m_timestampSignal
    );
}

//------------------------------------------------------------------------------

void STimestampSignalTest::tearDown()
{
    m_worker->stop();
    if(!m_timestampSignal->isStopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_timestampSignal->stop().get());
    }

    service::remove(m_timestampSignal);
}

//------------------------------------------------------------------------------

void STimestampSignalTest::test(bool useSystemClock)
{
    using namespace std::literals::chrono_literals;

    boost::property_tree::ptree ptree;
    ptree.put("useSystemClock", useSystemClock);
    m_timestampSignal->setConfiguration(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_timestampSignal->configure());
    CPPUNIT_ASSERT_NO_THROW(m_timestampSignal->start().get());

    std::vector<double> timestamps;
    auto triggeredSlot = core::com::newSlot(
        [&timestamps](double timestamp)
        {
            timestamps.push_back(timestamp);
        });
    m_worker = core::thread::Worker::New();
    triggeredSlot->setWorker(m_worker);
    m_timestampSignal->signal("triggered")->connect(triggeredSlot);

    CPPUNIT_ASSERT_NO_THROW(m_timestampSignal->update().get());
    fwTestWaitMacro(1 == timestamps.size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), timestamps.size());

    std::this_thread::sleep_for(1000ms);

    CPPUNIT_ASSERT_NO_THROW(m_timestampSignal->update().get());
    fwTestWaitMacro(2 == timestamps.size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), timestamps.size());

    CPPUNIT_ASSERT(timestamps[1] - timestamps[0] >= 1);
}

//------------------------------------------------------------------------------

void STimestampSignalTest::systemClockTest()
{
    test(true);
}

//------------------------------------------------------------------------------

void STimestampSignalTest::highResClockTest()
{
    test(false);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::base::com::ut
