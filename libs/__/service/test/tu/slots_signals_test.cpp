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

static utest::exception fwTestException(""); // force link with fwTest

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

    SBasicTest::sptr basicTestSrv = service::factory::make<SBasicTest>();
    service::registerService(basicTestSrv);
    basicTestSrv->set_inout(buffer1, SBasicTest::s_BUFFER_INOUT);

    basicTestSrv->set_worker(worker);

    base::shared_future_t startFuture = basicTestSrv->start();
    CPPUNIT_ASSERT(basicTestSrv->status() != base::STARTED);
    startFuture.wait();
    CPPUNIT_ASSERT(basicTestSrv->status() == base::STARTED);

    base::shared_future_t updateFuture = basicTestSrv->update();
    CPPUNIT_ASSERT(basicTestSrv->m_updateFinished == false);
    updateFuture.wait();
    CPPUNIT_ASSERT(basicTestSrv->m_updateFinished == true);

    base::shared_future_t swapFuture = basicTestSrv->swap_key(SBasicTest::s_BUFFER_INOUT, buffer2);
    CPPUNIT_ASSERT(basicTestSrv->m_swapFinished == false);
    swapFuture.wait();
    CPPUNIT_ASSERT(basicTestSrv->m_swapFinished == true);

    base::shared_future_t stopFuture = basicTestSrv->stop();
    CPPUNIT_ASSERT(basicTestSrv->status() != base::STOPPED);
    stopFuture.wait();
    CPPUNIT_ASSERT(basicTestSrv->status() == base::STOPPED);

    service::unregisterService(basicTestSrv);

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
        readerTest::sptr readerTestSrv = service::factory::make<readerTest>();
        service::registerService(readerTestSrv);
        readerTestSrv->set_inout(buffer1, basic_srv::s_BUFFER_INOUT);

        SShowTest::sptr showTestSrv = service::factory::make<SShowTest>();
        service::registerService(showTestSrv);
        showTestSrv->set_inout(buffer1, basic_srv::s_BUFFER_INOUT);
        showTestSrv->set_worker(worker1);

        buffer1->signal(data::object::MODIFIED_SIG)->connect(showTestSrv->slot(service::slots::UPDATE));

        readerTestSrv->start();
        showTestSrv->start();

        readerTestSrv->update().wait();

        base::shared_future_t stopReaderFuture = readerTestSrv->stop();
        base::shared_future_t stopShowFuture   = showTestSrv->stop();
        stopReaderFuture.wait();
        stopShowFuture.wait();

        CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_receiveCount);

        buffer1->signal(data::object::MODIFIED_SIG)->disconnect(showTestSrv->slot(service::slots::UPDATE));

        service::unregisterService(readerTestSrv);
        service::unregisterService(showTestSrv);
    }

    {
        readerTest::sptr readerTestSrv = service::factory::make<readerTest>();
        service::registerService(readerTestSrv);
        readerTestSrv->set_inout(buffer1, basic_srv::s_BUFFER_INOUT);

        readerTest::sptr reader2TestSrv = service::factory::make<readerTest>();
        service::registerService(reader2TestSrv);
        reader2TestSrv->set_inout(buffer1, basic_srv::s_BUFFER_INOUT);
        reader2TestSrv->set_worker(worker2);

        SShowTest::sptr showTestSrv = service::factory::make<SShowTest>();
        service::registerService(showTestSrv);
        showTestSrv->set_inout(buffer1, basic_srv::s_BUFFER_INOUT, true);
        showTestSrv->set_worker(worker1);

        readerTestSrv->start();
        reader2TestSrv->start();
        showTestSrv->start().wait();

        base::shared_future_t updateReaderFuture  = readerTestSrv->update();
        base::shared_future_t updateReader2Future = reader2TestSrv->update();
        updateReaderFuture.wait();
        updateReader2Future.wait();

        base::shared_future_t stopReaderFuture  = readerTestSrv->stop();
        base::shared_future_t stopReader2Future = reader2TestSrv->stop();
        base::shared_future_t stopShowFuture    = showTestSrv->stop();
        stopReaderFuture.wait();
        stopReader2Future.wait();
        stopShowFuture.wait();

        CPPUNIT_ASSERT_EQUAL(2, showTestSrv->m_receiveCount);

        service::unregisterService(readerTestSrv);
        service::unregisterService(reader2TestSrv);
        service::unregisterService(showTestSrv);
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

    reader2Test::sptr readerTestSrv = service::factory::make<reader2Test>();
    service::registerService(readerTestSrv);
    readerTestSrv->set_inout(buffer1, basic_srv::s_BUFFER_INOUT);

    SShowTest::sptr showTestSrv = service::factory::make<SShowTest>();
    service::registerService(showTestSrv);
    showTestSrv->set_inout(buffer1, basic_srv::s_BUFFER_INOUT);
    showTestSrv->set_worker(worker1);

    readerTestSrv->signal(reader2Test::CHANGED_SIG)->connect(showTestSrv->slot(SShowTest::CHANGE_SLOT));

    readerTestSrv->start();
    showTestSrv->start();

    readerTestSrv->update().wait();

    base::shared_future_t stopReaderFuture = readerTestSrv->stop();
    base::shared_future_t stopShowFuture   = showTestSrv->stop();
    stopReaderFuture.wait();
    stopShowFuture.wait();

    readerTestSrv->signal(reader2Test::CHANGED_SIG)->disconnect(showTestSrv->slot(SShowTest::CHANGE_SLOT));

    CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_changeCount);

    service::unregisterService(readerTestSrv);
    service::unregisterService(showTestSrv);

    core::thread::remove_worker("worker1");
}

//------------------------------------------------------------------------------

void slots_signals_test::blockConnectionTest()
{
    buffer::sptr buffer1 = std::make_shared<buffer>();

    core::thread::worker::sptr worker1 = core::thread::worker::make();
    core::thread::add_worker("worker1", worker1);

    readerTest::sptr readerTestSrv = service::factory::make<readerTest>();
    service::registerService(readerTestSrv);
    readerTestSrv->set_inout(buffer1, basic_srv::s_BUFFER_INOUT);

    SShow2Test::sptr showTestSrv = service::factory::make<SShow2Test>();
    service::registerService(showTestSrv);
    showTestSrv->set_inout(buffer1, basic_srv::s_BUFFER_INOUT);
    showTestSrv->set_worker(worker1);

    core::com::connection connection;
    connection = buffer1->signal(data::object::MODIFIED_SIG)->
                 connect(showTestSrv->slot(SShow2Test::UPDATE_BUFFER_SLOT));

    readerTestSrv->start();
    showTestSrv->start();

    readerTestSrv->update();

    std::this_thread::sleep_for(std::chrono::seconds(8));

    base::shared_future_t stopReaderFuture = readerTestSrv->stop();
    base::shared_future_t stopShowFuture   = showTestSrv->stop();
    stopReaderFuture.wait();
    stopShowFuture.wait();

    connection.disconnect();

    CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_receiveCount);

    core::thread::remove_worker("worker1");
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
