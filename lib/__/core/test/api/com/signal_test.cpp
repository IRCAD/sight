/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "signal_test.hpp"

#include "core/com/exception/already_connected.hpp"
#include "core/com/exception/bad_slot.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/thread/worker.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <string>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::com::ut::signal_test);

namespace sight::core::com::ut
{

//------------------------------------------------------------------------------

void signal_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void signal_test::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void signal_test::build_test()
{
    {
        using signature = void ();
        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();
        CPPUNIT_ASSERT(sig);
    }

    {
        using signature = void (int);
        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();
        CPPUNIT_ASSERT(sig);
    }

    {
        using signature = void (char, float);
        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();
        CPPUNIT_ASSERT(sig);
    }

    {
        using signature = void (std::int16_t, double, std::string);
        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();
        CPPUNIT_ASSERT(sig);
    }
}

//-----------------------------------------------------------------------------

struct signal_test_class
{
    signal_test_class()
    = default;

    //------------------------------------------------------------------------------

    void method0()
    {
        m_method0 = true;
    }

    //------------------------------------------------------------------------------

    void method00()
    {
        m_method00 = true;
    }

    //------------------------------------------------------------------------------

    float method1(float _f)
    {
        m_method1 = true;
        return 2 * _f;
    }

    //------------------------------------------------------------------------------

    float method2(float _f, int /*unused*/)
    {
        m_method2 = true;
        return 2 * _f;
    }

    //------------------------------------------------------------------------------

    float method3(float _f, double /*unused*/, std::string /*unused*/)
    {
        m_method3 = true;
        return 2 * _f;
    }

    bool m_method00 {false};
    bool m_method0 {false};
    bool m_method1 {false};
    bool m_method2 {false};
    bool m_method3 {false};
};

//-----------------------------------------------------------------------------

void signal_test::connect_test()
{
    signal_test_class test_object;
    auto slot0 = core::com::new_slot(&signal_test_class::method0, &test_object);
    auto slot1 = core::com::new_slot(&signal_test_class::method1, &test_object);
    auto slot2 = core::com::new_slot(&signal_test_class::method2, &test_object);
    auto slot3 = core::com::new_slot(&signal_test_class::method3, &test_object);

    core::com::connection connection;
    CPPUNIT_ASSERT(connection.expired());

    {
        using signature = void ();
        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        connection = sig->connect(slot0);
        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        using signature = void (float);
        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        connection = sig->connect(slot1);
        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        using signature = void (float, int);
        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        connection = sig->connect(slot2);
        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        using signature = void (float, double, std::string);
        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        connection = sig->connect(slot3);
        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());
    }

    CPPUNIT_ASSERT(connection.expired());

    // Disconnect
    {
        using signature = void ();
        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        connection = sig->connect(slot0);
        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());
        connection.disconnect();
        CPPUNIT_ASSERT(connection.expired());
        CPPUNIT_ASSERT_EQUAL((std::size_t) 0, sig->num_connections());
    }

    {
        using signature = void ();
        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        sig->connect(slot0);
        connection = sig->get_connection(slot0);

        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());
        connection.disconnect();
        CPPUNIT_ASSERT(connection.expired());
        CPPUNIT_ASSERT_EQUAL((std::size_t) 0, sig->num_connections());

        CPPUNIT_ASSERT_THROW(sig->get_connection(slot3, true), core::com::exception::bad_slot);
    }

    {
        using signature = void ();
        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        connection = sig->connect(slot0);
        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());
        sig->disconnect(slot0);
        CPPUNIT_ASSERT(connection.expired());
        CPPUNIT_ASSERT_EQUAL((std::size_t) 0, sig->num_connections());
    }

    {
        using signature = void ();
        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        core::com::slot<void()>::sptr slot =
            core::com::new_slot(&signal_test_class::method0, &test_object);

        connection = sig->connect(slot0);
        core::com::connection connection2 = sig->connect(slot);

        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT(!connection2.expired());
        CPPUNIT_ASSERT_EQUAL((std::size_t) 2, sig->num_connections());
        sig->disconnect_all();
        CPPUNIT_ASSERT(connection.expired());
        CPPUNIT_ASSERT(connection2.expired());
        CPPUNIT_ASSERT_EQUAL((std::size_t) 0, sig->num_connections());
    }

    {
        using signature = void (std::string);
        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        CPPUNIT_ASSERT_THROW(sig->connect(slot1), core::com::exception::bad_slot);
        CPPUNIT_ASSERT_THROW(sig->connect(slot2), core::com::exception::bad_slot);
        CPPUNIT_ASSERT_THROW(sig->connect(slot3), core::com::exception::bad_slot);
    }

    {
        using signature = void (std::string);
        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        CPPUNIT_ASSERT_THROW(sig->disconnect(slot1), core::com::exception::bad_slot);
        CPPUNIT_ASSERT_THROW(sig->disconnect(slot2), core::com::exception::bad_slot);
        CPPUNIT_ASSERT_THROW(sig->disconnect(slot3), core::com::exception::bad_slot);

        sig->connect(slot0);

        CPPUNIT_ASSERT_THROW(sig->disconnect(slot1), core::com::exception::bad_slot);
        CPPUNIT_ASSERT_THROW(sig->disconnect(slot2), core::com::exception::bad_slot);
        CPPUNIT_ASSERT_THROW(sig->disconnect(slot3), core::com::exception::bad_slot);

        sig->disconnect(slot0);
    }

    // Test for a specific cast that used to cause a stack overflow
    // When we have more arguments than the slot and the type is different we try to remove arguments
    // Until we can cast, but we have to stop somewhere, when the arity is 0
    {
        using signature = void (int, int, int, int);
        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        CPPUNIT_ASSERT_THROW(sig->connect(slot1), core::com::exception::bad_slot);
        CPPUNIT_ASSERT_THROW(sig->connect(slot2), core::com::exception::bad_slot);
        CPPUNIT_ASSERT_THROW(sig->connect(slot3), core::com::exception::bad_slot);
    }

    {
        using signature = void ();
        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        sig->connect(slot0);
        CPPUNIT_ASSERT_THROW(sig->connect(slot0), core::com::exception::already_connected);

        CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());
    }
}

//-----------------------------------------------------------------------------

void signal_test::emit_test()
{
    core::com::connection connection;

    {
        using signature = void ();
        signal_test_class test_object;

        core::com::slot<signature>::sptr slot =
            core::com::new_slot(&signal_test_class::method0, &test_object);

        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());
        sig->emit();
        CPPUNIT_ASSERT(test_object.m_method0);
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        using signature = void (float);
        signal_test_class test_object;

        core::com::slot<float(float)>::sptr slot =
            core::com::new_slot(&signal_test_class::method1, &test_object);

        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());

        sig->emit(21.0F);
        CPPUNIT_ASSERT(test_object.m_method1);
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        using signature = void (float, int);
        signal_test_class test_object;

        core::com::slot<float(float, int)>::sptr slot =
            core::com::new_slot(&signal_test_class::method2, &test_object);

        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());

        sig->emit(21.0F, 42);
        CPPUNIT_ASSERT(test_object.m_method2);
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        using signature = void (float, double, std::string);
        signal_test_class test_object;

        core::com::slot<float(float, double, std::string)>::sptr slot =
            core::com::new_slot(&signal_test_class::method3, &test_object);

        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());

        sig->emit(21.0F, 42.0, "emit");
        CPPUNIT_ASSERT(test_object.m_method3);
    }

    CPPUNIT_ASSERT(connection.expired());
}

//-----------------------------------------------------------------------------

void signal_test::auto_slot_disconnect_test()
{
    using signature = void (float);
    signal_test_class test_object;

    core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

    {
        core::com::slot<void()>::sptr slot0 =
            core::com::new_slot(&signal_test_class::method0, &test_object);

        sig->connect(slot0);

        CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());
        {
            core::com::slot<float(float)>::sptr slot1 =
                core::com::new_slot(&signal_test_class::method1, &test_object);

            sig->connect(slot1);
            CPPUNIT_ASSERT_EQUAL((std::size_t) 2, sig->num_connections());
        }

        CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());

        {
            core::com::slot<void()>::sptr slot00 =
                core::com::new_slot(&signal_test_class::method00, &test_object);

            sig->connect(slot00);
            CPPUNIT_ASSERT_EQUAL((std::size_t) 2, sig->num_connections());
        }

        CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());
    }

    CPPUNIT_ASSERT_EQUAL((std::size_t) 0, sig->num_connections());
}

//-----------------------------------------------------------------------------

void signal_test::argument_loss_test()
{
    using signature = void (float, double, std::string);
    signal_test_class test_object;

    core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

    core::com::slot<void()>::sptr slot0 =
        core::com::new_slot(&signal_test_class::method0, &test_object);
    core::com::slot<float(float)>::sptr slot1 =
        core::com::new_slot(&signal_test_class::method1, &test_object);
    core::com::slot<float(float, double, std::string)>::sptr slot3 =
        core::com::new_slot(&signal_test_class::method3, &test_object);

    sig->connect(slot0);
    CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());

    sig->connect(slot1);
    CPPUNIT_ASSERT_EQUAL((std::size_t) 2, sig->num_connections());

    sig->connect(slot3);
    CPPUNIT_ASSERT_EQUAL((std::size_t) 3, sig->num_connections());

    sig->emit(21.0F, 42.0, "emit");

    CPPUNIT_ASSERT(test_object.m_method0);
    CPPUNIT_ASSERT(test_object.m_method1);
    CPPUNIT_ASSERT(test_object.m_method3);

    sig->disconnect_all();
    CPPUNIT_ASSERT_EQUAL((std::size_t) 0, sig->num_connections());
}

//-----------------------------------------------------------------------------

void signal_test::async_argument_loss_test()
{
    using signature = void (float, double, std::string);
    signal_test_class test_object;

    core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

    core::com::slot<void()>::sptr slot0 =
        core::com::new_slot(&signal_test_class::method0, &test_object);
    core::com::slot<float(float)>::sptr slot1 =
        core::com::new_slot(&signal_test_class::method1, &test_object);
    core::com::slot<float(float, double, std::string)>::sptr slot3 =
        core::com::new_slot(&signal_test_class::method3, &test_object);

    core::thread::worker::sptr worker = core::thread::worker::make();
    slot0->set_worker(worker);
    slot1->set_worker(worker);
    slot3->set_worker(worker);

    sig->connect(slot0);
    CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());

    sig->connect(slot1);
    CPPUNIT_ASSERT_EQUAL((std::size_t) 2, sig->num_connections());

    sig->connect(slot3);
    CPPUNIT_ASSERT_EQUAL((std::size_t) 3, sig->num_connections());

    sig->async_emit(21.0F, 42.0, "async_emit");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    CPPUNIT_ASSERT(test_object.m_method0);
    CPPUNIT_ASSERT(test_object.m_method1);
    CPPUNIT_ASSERT(test_object.m_method3);

    sig->disconnect_all();
    CPPUNIT_ASSERT_EQUAL((std::size_t) 0, sig->num_connections());
    worker->stop();
}

//-----------------------------------------------------------------------------

void signal_test::block_test()
{
    using signature = void (float, double, std::string);
    signal_test_class test_object;

    core::com::connection connection;

    core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

    core::com::slot<void()>::sptr slot0 =
        core::com::new_slot(&signal_test_class::method0, &test_object);
    core::com::slot<float(float)>::sptr slot1 =
        core::com::new_slot(&signal_test_class::method1, &test_object);
    core::com::slot<float(float, double, std::string)>::sptr slot3 =
        core::com::new_slot(&signal_test_class::method3, &test_object);

    sig->connect(slot0);
    connection = sig->connect(slot1);
    sig->connect(slot3);

    CPPUNIT_ASSERT_EQUAL((std::size_t) 3, sig->num_connections());

    sig->emit(21.0F, 42.0, "emit");

    CPPUNIT_ASSERT(test_object.m_method0);
    CPPUNIT_ASSERT(test_object.m_method1);
    CPPUNIT_ASSERT(test_object.m_method3);

    test_object.m_method0 = false;
    test_object.m_method1 = false;
    test_object.m_method3 = false;

    {
        core::com::connection::blocker block(connection);
        sig->emit(21.0F, 42.0, "emit");
    }

    CPPUNIT_ASSERT(test_object.m_method0);
    CPPUNIT_ASSERT(!test_object.m_method1);
    CPPUNIT_ASSERT(test_object.m_method3);

    test_object.m_method0 = false;
    test_object.m_method1 = false;
    test_object.m_method3 = false;

    sig->emit(21.0F, 42.0, "emit");

    CPPUNIT_ASSERT(test_object.m_method0);
    CPPUNIT_ASSERT(test_object.m_method1);
    CPPUNIT_ASSERT(test_object.m_method3);

    test_object.m_method0 = false;
    test_object.m_method1 = false;
    test_object.m_method3 = false;

    {
        core::com::connection::blocker block(connection);
        block.reset();
        sig->emit(21.0F, 42.0, "emit");
    }

    CPPUNIT_ASSERT(test_object.m_method0);
    CPPUNIT_ASSERT(test_object.m_method1);
    CPPUNIT_ASSERT(test_object.m_method3);
}

//-----------------------------------------------------------------------------

void signal_test::async_emit_test()
{
    core::com::connection connection;
    core::thread::worker::sptr worker = core::thread::worker::make();

    {
        using signature = void ();
        signal_test_class test_object;

        core::com::slot<signature>::sptr slot =
            core::com::new_slot(&signal_test_class::method0, &test_object);
        slot->set_worker(worker);

        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());
        sig->async_emit();
        std::this_thread::sleep_for(std::chrono::seconds(1));

        CPPUNIT_ASSERT(test_object.m_method0);
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        using signature = void (float);
        signal_test_class test_object;

        core::com::slot<float(float)>::sptr slot =
            core::com::new_slot(&signal_test_class::method1, &test_object);
        slot->set_worker(worker);

        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());

        sig->async_emit(21.0F);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        CPPUNIT_ASSERT(test_object.m_method1);
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        using signature = void (float, int);
        signal_test_class test_object;

        core::com::slot<float(float, int)>::sptr slot =
            core::com::new_slot(&signal_test_class::method2, &test_object);
        slot->set_worker(worker);

        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());

        sig->async_emit(21.0F, 42);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        CPPUNIT_ASSERT(test_object.m_method2);
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        using signature = void (float, double, std::string);
        signal_test_class test_object;

        core::com::slot<float(float, double, std::string)>::sptr slot =
            core::com::new_slot(&signal_test_class::method3, &test_object);
        slot->set_worker(worker);

        core::com::signal<signature>::sptr sig = std::make_shared<core::com::signal<signature> >();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((std::size_t) 1, sig->num_connections());

        sig->async_emit(21.0F, 42.0, "emit");
        std::this_thread::sleep_for(std::chrono::seconds(1));

        CPPUNIT_ASSERT(test_object.m_method3);
    }

    CPPUNIT_ASSERT(connection.expired());
    worker->stop();
}

} // namespace sight::core::com::ut
