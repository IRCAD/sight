/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "proxy_test.hpp"

#include <core/com/proxy.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hpp>
#include <core/com/slot.hxx>

#include <utest/wait.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::ut::proxy_test);

namespace sight::core::ut
{

//------------------------------------------------------------------------------

void proxy_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void proxy_test::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

struct proxy_test_class
{
    proxy_test_class()
    = default;

    //------------------------------------------------------------------------------

    int sum(int _a, int _b)
    {
        SIGHT_INFO("SUM " << _a << " + " << _b);
        ++m_method_sum;
        return _a + _b;
    }

    //------------------------------------------------------------------------------

    int square(int _a)
    {
        SIGHT_INFO("SQUARE " << _a);
        ++m_method_square;
        return _a * _a;
    }

    //------------------------------------------------------------------------------

    void do_nothing()
    {
        SIGHT_INFO("DO NOTHING");
        ++m_method_do_nothing;
    }

    int m_method_sum {0};
    int m_method_square {0};
    int m_method_do_nothing {0};
};

//------------------------------------------------------------------------------

void proxy_test::basic_test()
{
    const std::string channel = "channel";

    core::com::proxy::sptr proxy = core::com::proxy::get();

    core::com::signal<void(int, int)>::sptr sig        = std::make_shared<core::com::signal<void(int, int)> >();
    core::com::signal<void(int, int, char)>::sptr sig2 = std::make_shared<core::com::signal<void(int, int, char)> >();

    proxy_test_class test_object;
    core::com::slot<int(int, int)>::sptr slot = core::com::new_slot(&proxy_test_class::sum, &test_object);
    core::com::slot<int(int)>::sptr slot2     = core::com::new_slot(&proxy_test_class::square, &test_object);
    core::com::slot<void()>::sptr slot3       = core::com::new_slot(&proxy_test_class::do_nothing, &test_object);
    core::thread::worker::sptr worker         = core::thread::worker::make();
    slot->set_worker(worker);
    slot2->set_worker(worker);
    slot3->set_worker(worker);

    proxy->connect(channel, sig);
    proxy->connect(channel, sig2);
    proxy->connect(channel, slot);
    proxy->connect(channel, slot2);
    proxy->connect(channel, slot3);

    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(3), sig->num_connections());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(3), sig2->num_connections());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), slot->num_connections());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), slot2->num_connections());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), slot3->num_connections());
    sig->async_emit(3, 5);

    SIGHT_TEST_WAIT(
        test_object.m_method_sum == 1 && test_object.m_method_square == 1 && test_object.m_method_do_nothing == 1
    );

    CPPUNIT_ASSERT_EQUAL(1, test_object.m_method_sum);
    CPPUNIT_ASSERT_EQUAL(1, test_object.m_method_square);
    CPPUNIT_ASSERT_EQUAL(1, test_object.m_method_do_nothing);

    sig2->async_emit(8, 2, 'x');

    SIGHT_TEST_WAIT(
        test_object.m_method_sum == 2 && test_object.m_method_square == 2 && test_object.m_method_do_nothing == 2
    );

    CPPUNIT_ASSERT_EQUAL(2, test_object.m_method_sum);
    CPPUNIT_ASSERT_EQUAL(2, test_object.m_method_square);
    CPPUNIT_ASSERT_EQUAL(2, test_object.m_method_do_nothing);

    proxy->disconnect(channel, sig);
    proxy->disconnect(channel, sig2);
    proxy->disconnect(channel, slot);
    proxy->disconnect(channel, slot2);
    proxy->disconnect(channel, slot3);

    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), sig->num_connections());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), sig2->num_connections());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), slot->num_connections());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), slot2->num_connections());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), slot3->num_connections());
    worker->stop();
}

//------------------------------------------------------------------------------

} // namespace sight::core::ut
