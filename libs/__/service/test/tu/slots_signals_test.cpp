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

void slots_signals_test::basicTest()
{
    buffer::sptr buffer1 = std::make_shared<buffer>();
    buffer::sptr buffer2 = std::make_shared<buffer>();

    core::thread::worker::sptr worker = core::thread::worker::make();
    core::thread::add_worker("test", worker);

    SBasicTest::sptr basic_test_srv = service::factory::make<SBasicTest>();
    service::register_service(basic_test_srv);
    basic_test_srv->set_inout(buffer1, SBasicTest::s_BUFFER_INOUT);

    basic_test_srv->set_worker(worker);

    base::shared_future_t start_future = basic_test_srv->start();
    CPPUNIT_ASSERT(basic_test_srv->status() != base::STARTED);
    start_future.wait();
    CPPUNIT_ASSERT(basic_test_srv->status() == base::STARTED);

    base::shared_future_t update_future = basic_test_srv->update();
    CPPUNIT_ASSERT(basic_test_srv->m_updateFinished == false);
    update_future.wait();
    CPPUNIT_ASSERT(basic_test_srv->m_updateFinished == true);

    base::shared_future_t swap_future = basic_test_srv->swap_key(SBasicTest::s_BUFFER_INOUT, buffer2);
    CPPUNIT_ASSERT(basic_test_srv->m_swapFinished == false);
    swap_future.wait();
    CPPUNIT_ASSERT(basic_test_srv->m_swapFinished == true);

    base::shared_future_t stop_future = basic_test_srv->stop();
    CPPUNIT_ASSERT(basic_test_srv->status() != base::STOPPED);
    stop_future.wait();
    CPPUNIT_ASSERT(basic_test_srv->status() == base::STOPPED);

    service::unregister_service(basic_test_srv);

    core::thread::remove_worker("test");
}

//------------------------------------------------------------------------------

void slots_signals_test::comObjectServiceTest()
{
    core::thread::worker::sptr worker1 = core::thread::worker::make();
    core::thread::add_worker("worker1", worker1);
    core::thread::worker::sptr worker2 = core::thread::worker::make();
    core::thread::add_worker("worker2", worker2);

    buffer::sptr buffer1 = std::make_shared<buffer>();
    {
        readerTest::sptr reader_test_srv = service::factory::make<readerTest>();
        service::register_service(reader_test_srv);
        reader_test_srv->set_inout(buffer1, basic_srv::s_BUFFER_INOUT);

        SShowTest::sptr show_test_srv = service::factory::make<SShowTest>();
        service::register_service(show_test_srv);
        show_test_srv->set_inout(buffer1, basic_srv::s_BUFFER_INOUT);
        show_test_srv->set_worker(worker1);

        buffer1->signal(data::object::MODIFIED_SIG)->connect(show_test_srv->slot(service::slots::UPDATE));

        reader_test_srv->start();
        show_test_srv->start();

        reader_test_srv->update().wait();

        base::shared_future_t stop_reader_future = reader_test_srv->stop();
        base::shared_future_t stop_show_future   = show_test_srv->stop();
        stop_reader_future.wait();
        stop_show_future.wait();

        CPPUNIT_ASSERT_EQUAL(1, show_test_srv->m_receiveCount);

        buffer1->signal(data::object::MODIFIED_SIG)->disconnect(show_test_srv->slot(service::slots::UPDATE));

        service::unregister_service(reader_test_srv);
        service::unregister_service(show_test_srv);
    }

    {
        readerTest::sptr reader_test_srv = service::factory::make<readerTest>();
        service::register_service(reader_test_srv);
        reader_test_srv->set_inout(buffer1, basic_srv::s_BUFFER_INOUT);

        readerTest::sptr reader2_test_srv = service::factory::make<readerTest>();
        service::register_service(reader2_test_srv);
        reader2_test_srv->set_inout(buffer1, basic_srv::s_BUFFER_INOUT);
        reader2_test_srv->set_worker(worker2);

        SShowTest::sptr show_test_srv = service::factory::make<SShowTest>();
        service::register_service(show_test_srv);
        show_test_srv->set_inout(buffer1, basic_srv::s_BUFFER_INOUT, true);
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

        CPPUNIT_ASSERT_EQUAL(2, show_test_srv->m_receiveCount);

        service::unregister_service(reader_test_srv);
        service::unregister_service(reader2_test_srv);
        service::unregister_service(show_test_srv);
    }

    core::thread::remove_worker("worker1");
    core::thread::remove_worker("worker2");
}

//------------------------------------------------------------------------------

void slots_signals_test::comServiceToServiceTest()
{
    buffer::sptr buffer1 = std::make_shared<buffer>();

    core::thread::worker::sptr worker1 = core::thread::worker::make();
    core::thread::add_worker("worker1", worker1);

    reader2Test::sptr reader_test_srv = service::factory::make<reader2Test>();
    service::register_service(reader_test_srv);
    reader_test_srv->set_inout(buffer1, basic_srv::s_BUFFER_INOUT);

    SShowTest::sptr show_test_srv = service::factory::make<SShowTest>();
    service::register_service(show_test_srv);
    show_test_srv->set_inout(buffer1, basic_srv::s_BUFFER_INOUT);
    show_test_srv->set_worker(worker1);

    reader_test_srv->signal(reader2Test::CHANGED_SIG)->connect(show_test_srv->slot(SShowTest::CHANGE_SLOT));

    reader_test_srv->start();
    show_test_srv->start();

    reader_test_srv->update().wait();

    base::shared_future_t stop_reader_future = reader_test_srv->stop();
    base::shared_future_t stop_show_future   = show_test_srv->stop();
    stop_reader_future.wait();
    stop_show_future.wait();

    reader_test_srv->signal(reader2Test::CHANGED_SIG)->disconnect(show_test_srv->slot(SShowTest::CHANGE_SLOT));

    CPPUNIT_ASSERT_EQUAL(1, show_test_srv->m_changeCount);

    service::unregister_service(reader_test_srv);
    service::unregister_service(show_test_srv);

    core::thread::remove_worker("worker1");
}

//------------------------------------------------------------------------------

void slots_signals_test::blockConnectionTest()
{
    buffer::sptr buffer1 = std::make_shared<buffer>();

    core::thread::worker::sptr worker1 = core::thread::worker::make();
    core::thread::add_worker("worker1", worker1);

    readerTest::sptr reader_test_srv = service::factory::make<readerTest>();
    service::register_service(reader_test_srv);
    reader_test_srv->set_inout(buffer1, basic_srv::s_BUFFER_INOUT);

    SShow2Test::sptr show_test_srv = service::factory::make<SShow2Test>();
    service::register_service(show_test_srv);
    show_test_srv->set_inout(buffer1, basic_srv::s_BUFFER_INOUT);
    show_test_srv->set_worker(worker1);

    core::com::connection connection;
    connection = buffer1->signal(data::object::MODIFIED_SIG)->
                 connect(show_test_srv->slot(SShow2Test::UPDATE_BUFFER_SLOT));

    reader_test_srv->start();
    show_test_srv->start();

    reader_test_srv->update();

    std::this_thread::sleep_for(std::chrono::seconds(8));

    base::shared_future_t stop_reader_future = reader_test_srv->stop();
    base::shared_future_t stop_show_future   = show_test_srv->stop();
    stop_reader_future.wait();
    stop_show_future.wait();

    connection.disconnect();

    CPPUNIT_ASSERT_EQUAL(1, show_test_srv->m_receiveCount);

    core::thread::remove_worker("worker1");
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
