/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "core/com/exception/already_connected.hpp"
#include "core/com/exception/bad_slot.hpp"

#include <core/com/signal.hpp>

#include <core/com/slot.hpp>

#include <core/thread/worker.hpp>

#include <doctest/doctest.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>

namespace
{

using signal_test_signal = sight::core::com::signal<void ()>;

namespace
{

/// Ends the process when a test is stuck: a deadlock of the test's own threads cannot be unwound.
class hang_watch final
{
public:

    explicit hang_watch(std::string _what) :
        m_what(std::move(_what)),
        m_watch(
            [this]
            {
                std::unique_lock lock(m_mutex);
                if(!m_done.wait_for(lock, std::chrono::minutes(5), [this]{return m_finished;}))
                {
                    std::cerr << "deadlock: " << m_what << std::endl;
                    std::_Exit(EXIT_FAILURE);
                }
            })
    {
    }

    hang_watch(const hang_watch&)            = delete;
    hang_watch(hang_watch&&)                 = delete;
    hang_watch& operator=(const hang_watch&) = delete;
    hang_watch& operator=(hang_watch&&)      = delete;

    ~hang_watch()
    {
        {
            const std::scoped_lock lock(m_mutex);
            m_finished = true;
        }

        m_done.notify_all();
        m_watch.join();
    }

private:

    std::string m_what;
    std::mutex m_mutex;
    std::condition_variable m_done;
    bool m_finished {false};
    std::thread m_watch;
};

} // namespace

struct signal_test_class
{
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

} // namespace

/// While a thread emits, slots keep being connected and destroyed: a destroyed slot stays in the signal until the end
/// of its destruction, which disconnects it.
static void destroy_slots_during_emissions(
    int _slots,
    const sight::core::thread::worker::sptr& _worker,
    const std::function<void(const signal_test_signal&)>& _emit
)
{
    const auto signal = std::make_shared<signal_test_signal>();
    std::atomic_bool stopped {false};
    std::thread emitter(
        [&signal, &stopped, &_emit]
        {
        while(!stopped)
        {
            _emit(*signal);

            // Holding the read lock in a tight loop would starve the connections.
            std::this_thread::yield();
        }
        });

    for(int i = 0 ; i < _slots ; ++i)
    {
        const auto slot = sight::core::com::new_slot([]{});
        slot->set_worker(_worker);
        signal->connect(slot);
    }

    stopped = true;
    emitter.join();
    CHECK(signal->num_connections() == 0);
}

//------------------------------------------------------------------------------

TEST_SUITE("sight::core::com::signal")
{
//-----------------------------------------------------------------------------

    TEST_CASE("build")
    {
        {
            using signature = void ();
            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();
            CHECK(sig);
        }

        {
            using signature = void (int);
            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();
            CHECK(sig);
        }

        {
            using signature = void (char, float);
            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();
            CHECK(sig);
        }

        {
            using signature = void (std::int16_t, double, std::string);
            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();
            CHECK(sig);
        }
    }

//-----------------------------------------------------------------------------

    TEST_CASE("connect")
    {
        signal_test_class test_object;
        auto slot0 = sight::core::com::new_slot(&signal_test_class::method0, &test_object);
        auto slot1 = sight::core::com::new_slot(&signal_test_class::method1, &test_object);
        auto slot2 = sight::core::com::new_slot(&signal_test_class::method2, &test_object);
        auto slot3 = sight::core::com::new_slot(&signal_test_class::method3, &test_object);

        sight::core::com::connection connection;
        CHECK(connection.expired());

        {
            using signature = void ();
            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            connection = sig->connect(slot0);
            CHECK(!connection.expired());
            CHECK_EQ((std::size_t) 1, sig->num_connections());
        }

        CHECK(connection.expired());

        {
            using signature = void (float);
            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            connection = sig->connect(slot1);
            CHECK(!connection.expired());
            CHECK_EQ((std::size_t) 1, sig->num_connections());
        }

        CHECK(connection.expired());

        {
            using signature = void (float, int);
            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            connection = sig->connect(slot2);
            CHECK(!connection.expired());
            CHECK_EQ((std::size_t) 1, sig->num_connections());
        }

        CHECK(connection.expired());

        {
            using signature = void (float, double, std::string);
            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            connection = sig->connect(slot3);
            CHECK(!connection.expired());
            CHECK_EQ((std::size_t) 1, sig->num_connections());
        }

        CHECK(connection.expired());

        // Disconnect
        {
            using signature = void ();
            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            connection = sig->connect(slot0);
            CHECK(!connection.expired());
            CHECK_EQ((std::size_t) 1, sig->num_connections());
            connection.disconnect();
            CHECK(connection.expired());
            CHECK_EQ((std::size_t) 0, sig->num_connections());
        }

        {
            using signature = void ();
            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            sig->connect(slot0);
            connection = sig->get_connection(slot0);

            CHECK(!connection.expired());
            CHECK_EQ((std::size_t) 1, sig->num_connections());
            connection.disconnect();
            CHECK(connection.expired());
            CHECK_EQ((std::size_t) 0, sig->num_connections());

            CHECK_THROWS_AS(sig->get_connection(slot3, true), sight::core::com::exception::bad_slot);
        }

        {
            using signature = void ();
            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            connection = sig->connect(slot0);
            CHECK(!connection.expired());
            CHECK_EQ((std::size_t) 1, sig->num_connections());
            sig->disconnect(slot0);
            CHECK(connection.expired());
            CHECK_EQ((std::size_t) 0, sig->num_connections());
        }

        {
            using signature = void ();
            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            sight::core::com::slot<void()>::sptr slot =
                sight::core::com::new_slot(&signal_test_class::method0, &test_object);

            connection = sig->connect(slot0);
            sight::core::com::connection connection2 = sig->connect(slot);

            CHECK(!connection.expired());
            CHECK(!connection2.expired());
            CHECK_EQ((std::size_t) 2, sig->num_connections());
            sig->disconnect_all();
            CHECK(connection.expired());
            CHECK(connection2.expired());
            CHECK_EQ((std::size_t) 0, sig->num_connections());
        }

        {
            using signature = void (std::string);
            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            CHECK_THROWS_AS(sig->connect(slot1), sight::core::com::exception::bad_slot);
            CHECK_THROWS_AS(sig->connect(slot2), sight::core::com::exception::bad_slot);
            CHECK_THROWS_AS(sig->connect(slot3), sight::core::com::exception::bad_slot);
        }

        {
            using signature = void (std::string);
            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            CHECK_THROWS_AS(sig->disconnect(slot1), sight::core::com::exception::bad_slot);
            CHECK_THROWS_AS(sig->disconnect(slot2), sight::core::com::exception::bad_slot);
            CHECK_THROWS_AS(sig->disconnect(slot3), sight::core::com::exception::bad_slot);

            sig->connect(slot0);

            CHECK_THROWS_AS(sig->disconnect(slot1), sight::core::com::exception::bad_slot);
            CHECK_THROWS_AS(sig->disconnect(slot2), sight::core::com::exception::bad_slot);
            CHECK_THROWS_AS(sig->disconnect(slot3), sight::core::com::exception::bad_slot);

            sig->disconnect(slot0);
        }

        // Test for a specific cast that used to cause a stack overflow
        // When we have more arguments than the slot and the type is different we try to remove arguments
        // Until we can cast, but we have to stop somewhere, when the arity is 0
        {
            using signature = void (int, int, int, int);
            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            CHECK_THROWS_AS(sig->connect(slot1), sight::core::com::exception::bad_slot);
            CHECK_THROWS_AS(sig->connect(slot2), sight::core::com::exception::bad_slot);
            CHECK_THROWS_AS(sig->connect(slot3), sight::core::com::exception::bad_slot);
        }

        {
            using signature = void ();
            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            sig->connect(slot0);
            CHECK_THROWS_AS(sig->connect(slot0), sight::core::com::exception::already_connected);

            CHECK_EQ((std::size_t) 1, sig->num_connections());
        }
    }

//-----------------------------------------------------------------------------

    TEST_CASE("emit")
    {
        sight::core::com::connection connection;

        {
            using signature = void ();
            signal_test_class test_object;

            sight::core::com::slot<signature>::sptr slot =
                sight::core::com::new_slot(&signal_test_class::method0, &test_object);

            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            connection = sig->connect(slot);
            CHECK(!connection.expired());

            CHECK_EQ((std::size_t) 1, sig->num_connections());
            sig->emit();
            CHECK(test_object.m_method0);
        }

        CHECK(connection.expired());

        {
            using signature = void (float);
            signal_test_class test_object;

            sight::core::com::slot<float(float)>::sptr slot =
                sight::core::com::new_slot(&signal_test_class::method1, &test_object);

            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            connection = sig->connect(slot);
            CHECK(!connection.expired());

            CHECK_EQ((std::size_t) 1, sig->num_connections());

            sig->emit(21.0F);
            CHECK(test_object.m_method1);
        }

        CHECK(connection.expired());

        {
            using signature = void (float, int);
            signal_test_class test_object;

            sight::core::com::slot<float(float, int)>::sptr slot =
                sight::core::com::new_slot(&signal_test_class::method2, &test_object);

            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            connection = sig->connect(slot);
            CHECK(!connection.expired());

            CHECK_EQ((std::size_t) 1, sig->num_connections());

            sig->emit(21.0F, 42);
            CHECK(test_object.m_method2);
        }

        CHECK(connection.expired());

        {
            using signature = void (float, double, std::string);
            signal_test_class test_object;

            sight::core::com::slot<float(float, double, std::string)>::sptr slot =
                sight::core::com::new_slot(&signal_test_class::method3, &test_object);

            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            connection = sig->connect(slot);
            CHECK(!connection.expired());

            CHECK_EQ((std::size_t) 1, sig->num_connections());

            sig->emit(21.0F, 42.0, "emit");
            CHECK(test_object.m_method3);
        }

        CHECK(connection.expired());
    }

//-----------------------------------------------------------------------------

    TEST_CASE("auto_slot_disconnect")
    {
        using signature = void (float);
        signal_test_class test_object;

        sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

        {
            sight::core::com::slot<void()>::sptr slot0 =
                sight::core::com::new_slot(&signal_test_class::method0, &test_object);

            sig->connect(slot0);

            CHECK_EQ((std::size_t) 1, sig->num_connections());
            {
                sight::core::com::slot<float(float)>::sptr slot1 =
                    sight::core::com::new_slot(&signal_test_class::method1, &test_object);

                sig->connect(slot1);
                CHECK_EQ((std::size_t) 2, sig->num_connections());
            }

            CHECK_EQ((std::size_t) 1, sig->num_connections());

            {
                sight::core::com::slot<void()>::sptr slot00 =
                    sight::core::com::new_slot(&signal_test_class::method00, &test_object);

                sig->connect(slot00);
                CHECK_EQ((std::size_t) 2, sig->num_connections());
            }

            CHECK_EQ((std::size_t) 1, sig->num_connections());
        }

        CHECK_EQ((std::size_t) 0, sig->num_connections());
    }

//-----------------------------------------------------------------------------

    TEST_CASE("argument_loss")
    {
        using signature = void (float, double, std::string);
        signal_test_class test_object;

        sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

        sight::core::com::slot<void()>::sptr slot0 =
            sight::core::com::new_slot(&signal_test_class::method0, &test_object);
        sight::core::com::slot<float(float)>::sptr slot1 =
            sight::core::com::new_slot(&signal_test_class::method1, &test_object);
        sight::core::com::slot<float(float, double, std::string)>::sptr slot3 =
            sight::core::com::new_slot(&signal_test_class::method3, &test_object);

        sig->connect(slot0);
        CHECK_EQ((std::size_t) 1, sig->num_connections());

        sig->connect(slot1);
        CHECK_EQ((std::size_t) 2, sig->num_connections());

        sig->connect(slot3);
        CHECK_EQ((std::size_t) 3, sig->num_connections());

        sig->emit(21.0F, 42.0, "emit");

        CHECK(test_object.m_method0);
        CHECK(test_object.m_method1);
        CHECK(test_object.m_method3);

        sig->disconnect_all();
        CHECK_EQ((std::size_t) 0, sig->num_connections());
    }

//-----------------------------------------------------------------------------

    TEST_CASE("async_argument_loss")
    {
        using signature = void (float, double, std::string);
        signal_test_class test_object;

        sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

        sight::core::com::slot<void()>::sptr slot0 =
            sight::core::com::new_slot(&signal_test_class::method0, &test_object);
        sight::core::com::slot<float(float)>::sptr slot1 =
            sight::core::com::new_slot(&signal_test_class::method1, &test_object);
        sight::core::com::slot<float(float, double, std::string)>::sptr slot3 =
            sight::core::com::new_slot(&signal_test_class::method3, &test_object);

        sight::core::thread::worker::sptr worker = sight::core::thread::worker::make();
        slot0->set_worker(worker);
        slot1->set_worker(worker);
        slot3->set_worker(worker);

        sig->connect(slot0);
        CHECK_EQ((std::size_t) 1, sig->num_connections());

        sig->connect(slot1);
        CHECK_EQ((std::size_t) 2, sig->num_connections());

        sig->connect(slot3);
        CHECK_EQ((std::size_t) 3, sig->num_connections());

        sig->async_emit(21.0F, 42.0, "async_emit");

        worker->wait_barrier();

        CHECK(test_object.m_method0);
        CHECK(test_object.m_method1);
        CHECK(test_object.m_method3);

        sig->disconnect_all();
        CHECK_EQ((std::size_t) 0, sig->num_connections());
        worker->stop();
    }

//-----------------------------------------------------------------------------

    TEST_CASE("block")
    {
        using signature = void (float, double, std::string);
        signal_test_class test_object;

        sight::core::com::connection connection;

        sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

        sight::core::com::slot<void()>::sptr slot0 =
            sight::core::com::new_slot(&signal_test_class::method0, &test_object);
        sight::core::com::slot<float(float)>::sptr slot1 =
            sight::core::com::new_slot(&signal_test_class::method1, &test_object);
        sight::core::com::slot<float(float, double, std::string)>::sptr slot3 =
            sight::core::com::new_slot(&signal_test_class::method3, &test_object);

        sig->connect(slot0);
        connection = sig->connect(slot1);
        sig->connect(slot3);

        CHECK_EQ((std::size_t) 3, sig->num_connections());

        sig->emit(21.0F, 42.0, "emit");

        CHECK(test_object.m_method0);
        CHECK(test_object.m_method1);
        CHECK(test_object.m_method3);

        test_object.m_method0 = false;
        test_object.m_method1 = false;
        test_object.m_method3 = false;

        {
            sight::core::com::connection::blocker block(connection);
            sig->emit(21.0F, 42.0, "emit");
        }

        CHECK(test_object.m_method0);
        CHECK(!test_object.m_method1);
        CHECK(test_object.m_method3);

        test_object.m_method0 = false;
        test_object.m_method1 = false;
        test_object.m_method3 = false;

        sig->emit(21.0F, 42.0, "emit");

        CHECK(test_object.m_method0);
        CHECK(test_object.m_method1);
        CHECK(test_object.m_method3);

        test_object.m_method0 = false;
        test_object.m_method1 = false;
        test_object.m_method3 = false;

        {
            sight::core::com::connection::blocker block(connection);
            block.reset();
            sig->emit(21.0F, 42.0, "emit");
        }

        CHECK(test_object.m_method0);
        CHECK(test_object.m_method1);
        CHECK(test_object.m_method3);
    }

//-----------------------------------------------------------------------------

    TEST_CASE("does_not_deadlock_when_a_failing_slot_loses_its_last_reference")
    {
        // A deadlock here would hold the test until its timeout: it fails at once, with its name.
        const hang_watch watch("a slot destroyed under the lock of its signal");

        const auto signal = std::make_shared<signal_test_signal>();
        std::shared_ptr<sight::core::com::slot<void()> > slot;
        slot = sight::core::com::new_slot(
            std::function<void()>(
                [&slot]
        {
            // The emission holds the last reference when it unwinds.
            slot.reset();
            throw std::runtime_error("the slot failed");
        })
        );
        const auto connection = signal->connect(slot);

        CHECK_THROWS_AS(signal->emit(), std::runtime_error);
        CHECK(signal->num_connections() == 0);
    }

    //------------------------------------------------------------------------------

    TEST_CASE("does_not_deadlock_when_a_blocked_slot_loses_its_last_reference")
    {
        const hang_watch watch("a blocked slot destroyed under the lock of its signal");

        const auto signal = std::make_shared<signal_test_signal>();
        std::atomic_bool stopped {false};
        std::thread emitter(
            [&signal, &stopped]
            {
            while(!stopped)
            {
                signal->emit();
                std::this_thread::yield();
            }
            });

        // The emission looks at a blocked slot too, and may be the last to release it.
        for(int i = 0 ; i < 20000 ; ++i)
        {
            const auto slot = sight::core::com::new_slot([]{});
            {
                // A blocker must end before its connection, which ends with the slot.
                const sight::core::com::connection::blocker blocker(signal->connect(slot));
                std::this_thread::yield();
            }
        }

        stopped = true;
        emitter.join();
        CHECK(signal->num_connections() == 0);
    }

    //------------------------------------------------------------------------------

    TEST_CASE("emit_while_a_slot_is_destroyed")
    {
        const auto worker = sight::core::thread::worker::make();
        destroy_slots_during_emissions(5000, worker, [](const signal_test_signal& _signal){_signal.emit();});
        worker->stop();
    }

    //------------------------------------------------------------------------------

    TEST_CASE("async_emit_while_a_slot_is_destroyed")
    {
        const auto worker = sight::core::thread::worker::make();
        destroy_slots_during_emissions(
            5000,
            worker,
            [&worker](const signal_test_signal& _signal)
        {
            _signal.async_emit();
            worker->wait_barrier();
        });
        worker->stop();
    }

    //------------------------------------------------------------------------------

    TEST_CASE("async_emit")
    {
        sight::core::com::connection connection;
        sight::core::thread::worker::sptr worker = sight::core::thread::worker::make();

        {
            using signature = void ();
            signal_test_class test_object;

            sight::core::com::slot<signature>::sptr slot =
                sight::core::com::new_slot(&signal_test_class::method0, &test_object);
            slot->set_worker(worker);

            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            connection = sig->connect(slot);
            CHECK(!connection.expired());

            CHECK_EQ((std::size_t) 1, sig->num_connections());
            sig->async_emit();
            worker->wait_barrier();

            CHECK(test_object.m_method0);
        }

        CHECK(connection.expired());

        {
            using signature = void (float);
            signal_test_class test_object;

            sight::core::com::slot<float(float)>::sptr slot =
                sight::core::com::new_slot(&signal_test_class::method1, &test_object);
            slot->set_worker(worker);

            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            connection = sig->connect(slot);
            CHECK(!connection.expired());

            CHECK_EQ((std::size_t) 1, sig->num_connections());

            sig->async_emit(21.0F);
            worker->wait_barrier();

            CHECK(test_object.m_method1);
        }

        CHECK(connection.expired());

        {
            using signature = void (float, int);
            signal_test_class test_object;

            sight::core::com::slot<float(float, int)>::sptr slot =
                sight::core::com::new_slot(&signal_test_class::method2, &test_object);
            slot->set_worker(worker);

            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            connection = sig->connect(slot);
            CHECK(!connection.expired());

            CHECK_EQ((std::size_t) 1, sig->num_connections());

            sig->async_emit(21.0F, 42);
            worker->wait_barrier();

            CHECK(test_object.m_method2);
        }

        CHECK(connection.expired());

        {
            using signature = void (float, double, std::string);
            signal_test_class test_object;

            sight::core::com::slot<float(float, double, std::string)>::sptr slot =
                sight::core::com::new_slot(&signal_test_class::method3, &test_object);
            slot->set_worker(worker);

            sight::core::com::signal<signature>::sptr sig = std::make_shared<sight::core::com::signal<signature> >();

            connection = sig->connect(slot);
            CHECK(!connection.expired());

            CHECK_EQ((std::size_t) 1, sig->num_connections());

            sig->async_emit(21.0F, 42.0, "emit");
            worker->wait_barrier();

            CHECK(test_object.m_method3);
        }

        CHECK(connection.expired());
        worker->stop();
    }
} // TEST_SUITE
