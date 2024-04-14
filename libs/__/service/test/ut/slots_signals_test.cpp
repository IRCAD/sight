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

#include "slots_signals_test.hpp"

#include "slots_signals_stuff.hpp"

#include <service/registry.hpp>

#include <utest/exception.hpp>

#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::slots_signals_test);
//
namespace sight::service::ut
{

static utest::exception fw_test_exception(""); // force link with fwTest

//------------------------------------------------------------------------------

void slots_signals_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void slots_signals_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void slots_signals_test::basic_test()
{
    buffer::sptr buffer1 = std::make_shared<buffer>();
    buffer::sptr buffer2 = std::make_shared<buffer>();

    core::thread::worker::sptr worker = core::thread::worker::make();
    core::thread::add_worker("test", worker);

    s_basic_test::sptr basic_test_srv = service::factory::make<s_basic_test>();
    service::register_service(basic_test_srv);
    basic_test_srv->set_inout(buffer1, s_basic_test::BUFFER_INOUT);

    basic_test_srv->set_worker(worker);

    base::shared_future_t start_future = basic_test_srv->start();
    CPPUNIT_ASSERT(basic_test_srv->status() != base::global_status::started);
    start_future.wait();
    CPPUNIT_ASSERT(basic_test_srv->status() == base::global_status::started);

    base::shared_future_t update_future = basic_test_srv->update();
    CPPUNIT_ASSERT(basic_test_srv->m_update_finished == false);
    update_future.wait();
    CPPUNIT_ASSERT(basic_test_srv->m_update_finished == true);

    base::shared_future_t swap_future = basic_test_srv->swap_key(s_basic_test::BUFFER_INOUT, buffer2);
    CPPUNIT_ASSERT(basic_test_srv->m_swap_finished == false);
    swap_future.wait();
    CPPUNIT_ASSERT(basic_test_srv->m_swap_finished == true);

    base::shared_future_t stop_future = basic_test_srv->stop();
    CPPUNIT_ASSERT(basic_test_srv->status() != base::global_status::stopped);
    stop_future.wait();
    CPPUNIT_ASSERT(basic_test_srv->status() == base::global_status::stopped);

    service::unregister_service(basic_test_srv);

    core::thread::remove_worker("test");
}

//------------------------------------------------------------------------------

void slots_signals_test::com_object_service_test()
{
    core::thread::worker::sptr worker1 = core::thread::worker::make();
    core::thread::add_worker("worker1", worker1);
    core::thread::worker::sptr worker2 = core::thread::worker::make();
    core::thread::add_worker("worker2", worker2);

    buffer::sptr buffer1 = std::make_shared<buffer>();
    {
        reader_test::sptr reader_test_srv = service::factory::make<reader_test>();
        service::register_service(reader_test_srv);
        reader_test_srv->set_inout(buffer1, basic_srv::BUFFER_INOUT);

        s_show_test::sptr show_test_srv = service::factory::make<s_show_test>();
        service::register_service(show_test_srv);
        show_test_srv->set_inout(buffer1, basic_srv::BUFFER_INOUT);
        show_test_srv->set_worker(worker1);

        buffer1->signal(data::object::MODIFIED_SIG)->connect(show_test_srv->slot(service::slots::UPDATE));

        reader_test_srv->start();
        show_test_srv->start();

        reader_test_srv->update().wait();

        base::shared_future_t stop_reader_future = reader_test_srv->stop();
        base::shared_future_t stop_show_future   = show_test_srv->stop();
        stop_reader_future.wait();
        stop_show_future.wait();

        CPPUNIT_ASSERT_EQUAL(1, show_test_srv->m_receive_count);

        buffer1->signal(data::object::MODIFIED_SIG)->disconnect(show_test_srv->slot(service::slots::UPDATE));

        service::unregister_service(reader_test_srv);
        service::unregister_service(show_test_srv);
    }

    {
        reader_test::sptr reader_test_srv = service::factory::make<reader_test>();
        service::register_service(reader_test_srv);
        reader_test_srv->set_inout(buffer1, basic_srv::BUFFER_INOUT);

        reader_test::sptr reader2_test_srv = service::factory::make<reader_test>();
        service::register_service(reader2_test_srv);
        reader2_test_srv->set_inout(buffer1, basic_srv::BUFFER_INOUT);
        reader2_test_srv->set_worker(worker2);

        s_show_test::sptr show_test_srv = service::factory::make<s_show_test>();
        service::register_service(show_test_srv);
        show_test_srv->set_inout(buffer1, basic_srv::BUFFER_INOUT, true);
        show_test_srv->set_worker(worker1);

        reader_test_srv->start();
        reader2_test_srv->start();
        show_test_srv->start().wait();

        base::shared_future_t update_reader_future  = reader_test_srv->update();
        base::shared_future_t update_reader2_future = reader2_test_srv->update();
        update_reader_future.wait();
        update_reader2_future.wait();

        base::shared_future_t stop_reader_future  = reader_test_srv->stop();
        base::shared_future_t stop_reader2_future = reader2_test_srv->stop();
        base::shared_future_t stop_show_future    = show_test_srv->stop();
        stop_reader_future.wait();
        stop_reader2_future.wait();
        stop_show_future.wait();

        CPPUNIT_ASSERT_EQUAL(2, show_test_srv->m_receive_count);

        service::unregister_service(reader_test_srv);
        service::unregister_service(reader2_test_srv);
        service::unregister_service(show_test_srv);
    }

    core::thread::remove_worker("worker1");
    core::thread::remove_worker("worker2");
}

//------------------------------------------------------------------------------

void slots_signals_test::com_service_to_service_test()
{
    buffer::sptr buffer1 = std::make_shared<buffer>();

    core::thread::worker::sptr worker1 = core::thread::worker::make();
    core::thread::add_worker("worker1", worker1);

    reader2_test::sptr reader_test_srv = service::factory::make<reader2_test>();
    service::register_service(reader_test_srv);
    reader_test_srv->set_inout(buffer1, basic_srv::BUFFER_INOUT);

    s_show_test::sptr show_test_srv = service::factory::make<s_show_test>();
    service::register_service(show_test_srv);
    show_test_srv->set_inout(buffer1, basic_srv::BUFFER_INOUT);
    show_test_srv->set_worker(worker1);

    reader_test_srv->signal(reader2_test::CHANGED_SIG)->connect(show_test_srv->slot(s_show_test::CHANGE_SLOT));

    reader_test_srv->start();
    show_test_srv->start();

    reader_test_srv->update().wait();

    base::shared_future_t stop_reader_future = reader_test_srv->stop();
    base::shared_future_t stop_show_future   = show_test_srv->stop();
    stop_reader_future.wait();
    stop_show_future.wait();

    reader_test_srv->signal(reader2_test::CHANGED_SIG)->disconnect(show_test_srv->slot(s_show_test::CHANGE_SLOT));

    CPPUNIT_ASSERT_EQUAL(1, show_test_srv->m_change_count);

    service::unregister_service(reader_test_srv);
    service::unregister_service(show_test_srv);

    core::thread::remove_worker("worker1");
}

//------------------------------------------------------------------------------

void slots_signals_test::block_connection_test()
{
    buffer::sptr buffer1 = std::make_shared<buffer>();

    core::thread::worker::sptr worker1 = core::thread::worker::make();
    core::thread::add_worker("worker1", worker1);

    reader_test::sptr reader_test_srv = service::factory::make<reader_test>();
    service::register_service(reader_test_srv);
    reader_test_srv->set_inout(buffer1, basic_srv::BUFFER_INOUT);

    s_show2_test::sptr show_test_srv = service::factory::make<s_show2_test>();
    service::register_service(show_test_srv);
    show_test_srv->set_inout(buffer1, basic_srv::BUFFER_INOUT);
    show_test_srv->set_worker(worker1);

    core::com::connection connection;
    connection = buffer1->signal(data::object::MODIFIED_SIG)->
                 connect(show_test_srv->slot(s_show2_test::UPDATE_BUFFER_SLOT));

    reader_test_srv->start();
    show_test_srv->start();

    reader_test_srv->update();

    std::this_thread::sleep_for(std::chrono::seconds(8));

    base::shared_future_t stop_reader_future = reader_test_srv->stop();
    base::shared_future_t stop_show_future   = show_test_srv->stop();
    stop_reader_future.wait();
    stop_show_future.wait();

    connection.disconnect();

    CPPUNIT_ASSERT_EQUAL(1, show_test_srv->m_receive_count);

    core::thread::remove_worker("worker1");
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
