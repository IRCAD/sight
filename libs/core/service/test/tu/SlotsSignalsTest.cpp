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

#include <utest/Exception.hpp>

#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::SlotsSignalsTest);
//
namespace sight::service::ut
{

static utest::Exception fwTestException(""); // force link with fwTest

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
    Buffer::sptr buffer  = Buffer::New();
    Buffer::sptr buffer2 = Buffer::New();

    core::thread::Worker::sptr worker = core::thread::Worker::New();
    core::thread::addWorker("test", worker);

    SBasicTest::sptr basicTestSrv = service::factory::New<SBasicTest>();
    service::registerService(basicTestSrv);
    basicTestSrv->setInOut(buffer, SBasicTest::s_BUFFER_INOUT);

    basicTestSrv->setWorker(worker);

    IService::SharedFutureType startFuture = basicTestSrv->start();
    CPPUNIT_ASSERT(basicTestSrv->getStatus() != IService::STARTED);
    startFuture.wait();
    CPPUNIT_ASSERT(basicTestSrv->getStatus() == IService::STARTED);

    IService::SharedFutureType updateFuture = basicTestSrv->update();
    CPPUNIT_ASSERT(basicTestSrv->m_updateFinished == false);
    updateFuture.wait();
    CPPUNIT_ASSERT(basicTestSrv->m_updateFinished == true);

    IService::SharedFutureType swapFuture = basicTestSrv->swapKey(SBasicTest::s_BUFFER_INOUT, buffer2);
    CPPUNIT_ASSERT(basicTestSrv->m_swapFinished == false);
    swapFuture.wait();
    CPPUNIT_ASSERT(basicTestSrv->m_swapFinished == true);

    IService::SharedFutureType stopFuture = basicTestSrv->stop();
    CPPUNIT_ASSERT(basicTestSrv->getStatus() != IService::STOPPED);
    stopFuture.wait();
    CPPUNIT_ASSERT(basicTestSrv->getStatus() == IService::STOPPED);

    service::unregisterService(basicTestSrv);

    core::thread::removeWorker("test");
}

//------------------------------------------------------------------------------

void SlotsSignalsTest::comObjectServiceTest()
{
    core::thread::Worker::sptr worker1 = core::thread::Worker::New();
    core::thread::addWorker("worker1", worker1);
    core::thread::Worker::sptr worker2 = core::thread::Worker::New();
    core::thread::addWorker("worker2", worker2);

    Buffer::sptr buffer = Buffer::New();
    {
        SReaderTest::sptr readerTestSrv = service::factory::New<SReaderTest>();
        service::registerService(readerTestSrv);
        readerTestSrv->setInOut(buffer, IBasicTest::s_BUFFER_INOUT);

        SShowTest::sptr showTestSrv = service::factory::New<SShowTest>();
        service::registerService(showTestSrv);
        showTestSrv->setInOut(buffer, IBasicTest::s_BUFFER_INOUT);
        showTestSrv->setWorker(worker1);

        buffer->signal(data::Object::s_MODIFIED_SIG)->connect(showTestSrv->slot(IService::slots::s_UPDATE));

        readerTestSrv->start();
        showTestSrv->start();

        readerTestSrv->update().wait();

        IService::SharedFutureType stopReaderFuture = readerTestSrv->stop();
        IService::SharedFutureType stopShowFuture   = showTestSrv->stop();
        stopReaderFuture.wait();
        stopShowFuture.wait();

        CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_receiveCount);

        buffer->signal(data::Object::s_MODIFIED_SIG)->disconnect(showTestSrv->slot(IService::slots::s_UPDATE));

        service::unregisterService(readerTestSrv);
        service::unregisterService(showTestSrv);
    }

    {
        SReaderTest::sptr readerTestSrv = service::factory::New<SReaderTest>();
        service::registerService(readerTestSrv);
        readerTestSrv->setInOut(buffer, IBasicTest::s_BUFFER_INOUT);

        SReaderTest::sptr reader2TestSrv = service::factory::New<SReaderTest>();
        service::registerService(reader2TestSrv);
        reader2TestSrv->setInOut(buffer, IBasicTest::s_BUFFER_INOUT);
        reader2TestSrv->setWorker(worker2);

        SShowTest::sptr showTestSrv = service::factory::New<SShowTest>();
        service::registerService(showTestSrv);
        showTestSrv->setInOut(buffer, IBasicTest::s_BUFFER_INOUT, true);
        showTestSrv->setWorker(worker1);

        readerTestSrv->start();
        reader2TestSrv->start();
        showTestSrv->start().wait();

        IService::SharedFutureType updateReaderFuture  = readerTestSrv->update();
        IService::SharedFutureType updateReader2Future = reader2TestSrv->update();
        updateReaderFuture.wait();
        updateReader2Future.wait();

        IService::SharedFutureType stopReaderFuture  = readerTestSrv->stop();
        IService::SharedFutureType stopReader2Future = reader2TestSrv->stop();
        IService::SharedFutureType stopShowFuture    = showTestSrv->stop();
        stopReaderFuture.wait();
        stopReader2Future.wait();
        stopShowFuture.wait();

        CPPUNIT_ASSERT_EQUAL(2, showTestSrv->m_receiveCount);

        service::unregisterService(readerTestSrv);
        service::unregisterService(reader2TestSrv);
        service::unregisterService(showTestSrv);
    }

    core::thread::removeWorker("worker1");
    core::thread::removeWorker("worker2");
}

//------------------------------------------------------------------------------

void SlotsSignalsTest::comServiceToServiceTest()
{
    Buffer::sptr buffer = Buffer::New();

    core::thread::Worker::sptr worker1 = core::thread::Worker::New();
    core::thread::addWorker("worker1", worker1);

    SReader2Test::sptr readerTestSrv = service::factory::New<SReader2Test>();
    service::registerService(readerTestSrv);
    readerTestSrv->setInOut(buffer, IBasicTest::s_BUFFER_INOUT);

    SShowTest::sptr showTestSrv = service::factory::New<SShowTest>();
    service::registerService(showTestSrv);
    showTestSrv->setInOut(buffer, IBasicTest::s_BUFFER_INOUT);
    showTestSrv->setWorker(worker1);

    readerTestSrv->signal(SReader2Test::s_CHANGED_SIG)->connect(showTestSrv->slot(SShowTest::s_CHANGE_SLOT));

    readerTestSrv->start();
    showTestSrv->start();

    readerTestSrv->update().wait();

    IService::SharedFutureType stopReaderFuture = readerTestSrv->stop();
    IService::SharedFutureType stopShowFuture   = showTestSrv->stop();
    stopReaderFuture.wait();
    stopShowFuture.wait();

    readerTestSrv->signal(SReader2Test::s_CHANGED_SIG)->disconnect(showTestSrv->slot(SShowTest::s_CHANGE_SLOT));

    CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_changeCount);

    service::unregisterService(readerTestSrv);
    service::unregisterService(showTestSrv);

    core::thread::removeWorker("worker1");
}

//------------------------------------------------------------------------------

void SlotsSignalsTest::blockConnectionTest()
{
    Buffer::sptr buffer = Buffer::New();

    core::thread::Worker::sptr worker1 = core::thread::Worker::New();
    core::thread::addWorker("worker1", worker1);

    SReaderTest::sptr readerTestSrv = service::factory::New<SReaderTest>();
    service::registerService(readerTestSrv);
    readerTestSrv->setInOut(buffer, IBasicTest::s_BUFFER_INOUT);

    SShow2Test::sptr showTestSrv = service::factory::New<SShow2Test>();
    service::registerService(showTestSrv);
    showTestSrv->setInOut(buffer, IBasicTest::s_BUFFER_INOUT);
    showTestSrv->setWorker(worker1);

    core::com::Connection connection;
    connection = buffer->signal(data::Object::s_MODIFIED_SIG)->
                 connect(showTestSrv->slot(SShow2Test::s_UPDATE_BUFFER_SLOT));

    readerTestSrv->start();
    showTestSrv->start();

    readerTestSrv->update();

    std::this_thread::sleep_for(std::chrono::seconds(8));

    IService::SharedFutureType stopReaderFuture = readerTestSrv->stop();
    IService::SharedFutureType stopShowFuture   = showTestSrv->stop();
    stopReaderFuture.wait();
    stopShowFuture.wait();

    connection.disconnect();

    CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_receiveCount);

    core::thread::removeWorker("worker1");
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
