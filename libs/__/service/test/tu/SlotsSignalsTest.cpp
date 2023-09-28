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

#include "SlotsSignalsTest.hpp"

#include "SlotsSignalsStuff.hpp"

#include <service/registry.hpp>

#include <utest/exception.hpp>

#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::SlotsSignalsTest);
//
namespace sight::service::ut
{

static utest::exception fwTestException(""); // force link with fwTest

//------------------------------------------------------------------------------

void SlotsSignalsTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void SlotsSignalsTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SlotsSignalsTest::basicTest()
{
    Buffer::sptr buffer  = std::make_shared<Buffer>();
    Buffer::sptr buffer2 = std::make_shared<Buffer>();

    core::thread::worker::sptr worker = core::thread::worker::make();
    core::thread::add_worker("test", worker);

    SBasicTest::sptr basicTestSrv = service::factory::make<SBasicTest>();
    service::registerService(basicTestSrv);
    basicTestSrv->setInOut(buffer, SBasicTest::s_BUFFER_INOUT);

    basicTestSrv->set_worker(worker);

    base::SharedFutureType startFuture = basicTestSrv->start();
    CPPUNIT_ASSERT(basicTestSrv->getStatus() != base::STARTED);
    startFuture.wait();
    CPPUNIT_ASSERT(basicTestSrv->getStatus() == base::STARTED);

    base::SharedFutureType updateFuture = basicTestSrv->update();
    CPPUNIT_ASSERT(basicTestSrv->m_updateFinished == false);
    updateFuture.wait();
    CPPUNIT_ASSERT(basicTestSrv->m_updateFinished == true);

    base::SharedFutureType swapFuture = basicTestSrv->swapKey(SBasicTest::s_BUFFER_INOUT, buffer2);
    CPPUNIT_ASSERT(basicTestSrv->m_swapFinished == false);
    swapFuture.wait();
    CPPUNIT_ASSERT(basicTestSrv->m_swapFinished == true);

    base::SharedFutureType stopFuture = basicTestSrv->stop();
    CPPUNIT_ASSERT(basicTestSrv->getStatus() != base::STOPPED);
    stopFuture.wait();
    CPPUNIT_ASSERT(basicTestSrv->getStatus() == base::STOPPED);

    service::unregisterService(basicTestSrv);

    core::thread::remove_worker("test");
}

//------------------------------------------------------------------------------

void SlotsSignalsTest::comObjectServiceTest()
{
    core::thread::worker::sptr worker1 = core::thread::worker::make();
    core::thread::add_worker("worker1", worker1);
    core::thread::worker::sptr worker2 = core::thread::worker::make();
    core::thread::add_worker("worker2", worker2);

    Buffer::sptr buffer = std::make_shared<Buffer>();
    {
        SReaderTest::sptr readerTestSrv = service::factory::make<SReaderTest>();
        service::registerService(readerTestSrv);
        readerTestSrv->setInOut(buffer, basic_srv::s_BUFFER_INOUT);

        SShowTest::sptr showTestSrv = service::factory::make<SShowTest>();
        service::registerService(showTestSrv);
        showTestSrv->setInOut(buffer, basic_srv::s_BUFFER_INOUT);
        showTestSrv->set_worker(worker1);

        buffer->signal(data::Object::MODIFIED_SIG)->connect(showTestSrv->slot(service::slots::UPDATE));

        readerTestSrv->start();
        showTestSrv->start();

        readerTestSrv->update().wait();

        base::SharedFutureType stopReaderFuture = readerTestSrv->stop();
        base::SharedFutureType stopShowFuture   = showTestSrv->stop();
        stopReaderFuture.wait();
        stopShowFuture.wait();

        CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_receiveCount);

        buffer->signal(data::Object::MODIFIED_SIG)->disconnect(showTestSrv->slot(service::slots::UPDATE));

        service::unregisterService(readerTestSrv);
        service::unregisterService(showTestSrv);
    }

    {
        SReaderTest::sptr readerTestSrv = service::factory::make<SReaderTest>();
        service::registerService(readerTestSrv);
        readerTestSrv->setInOut(buffer, basic_srv::s_BUFFER_INOUT);

        SReaderTest::sptr reader2TestSrv = service::factory::make<SReaderTest>();
        service::registerService(reader2TestSrv);
        reader2TestSrv->setInOut(buffer, basic_srv::s_BUFFER_INOUT);
        reader2TestSrv->set_worker(worker2);

        SShowTest::sptr showTestSrv = service::factory::make<SShowTest>();
        service::registerService(showTestSrv);
        showTestSrv->setInOut(buffer, basic_srv::s_BUFFER_INOUT, true);
        showTestSrv->set_worker(worker1);

        readerTestSrv->start();
        reader2TestSrv->start();
        showTestSrv->start().wait();

        base::SharedFutureType updateReaderFuture  = readerTestSrv->update();
        base::SharedFutureType updateReader2Future = reader2TestSrv->update();
        updateReaderFuture.wait();
        updateReader2Future.wait();

        base::SharedFutureType stopReaderFuture  = readerTestSrv->stop();
        base::SharedFutureType stopReader2Future = reader2TestSrv->stop();
        base::SharedFutureType stopShowFuture    = showTestSrv->stop();
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

void SlotsSignalsTest::comServiceToServiceTest()
{
    Buffer::sptr buffer = std::make_shared<Buffer>();

    core::thread::worker::sptr worker1 = core::thread::worker::make();
    core::thread::add_worker("worker1", worker1);

    SReader2Test::sptr readerTestSrv = service::factory::make<SReader2Test>();
    service::registerService(readerTestSrv);
    readerTestSrv->setInOut(buffer, basic_srv::s_BUFFER_INOUT);

    SShowTest::sptr showTestSrv = service::factory::make<SShowTest>();
    service::registerService(showTestSrv);
    showTestSrv->setInOut(buffer, basic_srv::s_BUFFER_INOUT);
    showTestSrv->set_worker(worker1);

    readerTestSrv->signal(SReader2Test::CHANGED_SIG)->connect(showTestSrv->slot(SShowTest::CHANGE_SLOT));

    readerTestSrv->start();
    showTestSrv->start();

    readerTestSrv->update().wait();

    base::SharedFutureType stopReaderFuture = readerTestSrv->stop();
    base::SharedFutureType stopShowFuture   = showTestSrv->stop();
    stopReaderFuture.wait();
    stopShowFuture.wait();

    readerTestSrv->signal(SReader2Test::CHANGED_SIG)->disconnect(showTestSrv->slot(SShowTest::CHANGE_SLOT));

    CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_changeCount);

    service::unregisterService(readerTestSrv);
    service::unregisterService(showTestSrv);

    core::thread::remove_worker("worker1");
}

//------------------------------------------------------------------------------

void SlotsSignalsTest::blockConnectionTest()
{
    Buffer::sptr buffer = std::make_shared<Buffer>();

    core::thread::worker::sptr worker1 = core::thread::worker::make();
    core::thread::add_worker("worker1", worker1);

    SReaderTest::sptr readerTestSrv = service::factory::make<SReaderTest>();
    service::registerService(readerTestSrv);
    readerTestSrv->setInOut(buffer, basic_srv::s_BUFFER_INOUT);

    SShow2Test::sptr showTestSrv = service::factory::make<SShow2Test>();
    service::registerService(showTestSrv);
    showTestSrv->setInOut(buffer, basic_srv::s_BUFFER_INOUT);
    showTestSrv->set_worker(worker1);

    core::com::connection connection;
    connection = buffer->signal(data::Object::MODIFIED_SIG)->
                 connect(showTestSrv->slot(SShow2Test::UPDATE_BUFFER_SLOT));

    readerTestSrv->start();
    showTestSrv->start();

    readerTestSrv->update();

    std::this_thread::sleep_for(std::chrono::seconds(8));

    base::SharedFutureType stopReaderFuture = readerTestSrv->stop();
    base::SharedFutureType stopShowFuture   = showTestSrv->stop();
    stopReaderFuture.wait();
    stopShowFuture.wait();

    connection.disconnect();

    CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_receiveCount);

    core::thread::remove_worker("worker1");
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
