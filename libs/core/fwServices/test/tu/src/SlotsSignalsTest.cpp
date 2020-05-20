/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTest/Exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::SlotsSignalsTest );
//
namespace fwServices
{
namespace ut
{

static ::fwTest::Exception fwTestException(""); // force link with fwTest

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
    SLM_TRACE_FUNC();
    using namespace ::fwServices::registry;

    Buffer::sptr buffer  = Buffer::New();
    Buffer::sptr buffer2 = Buffer::New();

    ActiveWorkers::sptr activeWorkers = ActiveWorkers::getDefault();
    activeWorkers->initRegistry();

    ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();
    activeWorkers->addWorker("test", worker);

    SBasicTest::sptr basicTestSrv = ::fwServices::factory::New<SBasicTest>();
    ::fwServices::OSR::registerService(basicTestSrv);
    basicTestSrv->registerInOut(buffer, SBasicTest::s_BUFFER_INOUT);

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

    ::fwServices::OSR::unregisterService(basicTestSrv);

    activeWorkers->clearRegistry();
}

//------------------------------------------------------------------------------

void SlotsSignalsTest::comObjectServiceTest()
{
    SLM_TRACE_FUNC();
    Buffer::sptr buffer = Buffer::New();

    registry::ActiveWorkers::sptr activeWorkers = registry::ActiveWorkers::getDefault();
    activeWorkers->initRegistry();
    ::fwThread::Worker::sptr worker1 = ::fwThread::Worker::New();
    activeWorkers->addWorker("worker1", worker1);
    ::fwThread::Worker::sptr worker2 = ::fwThread::Worker::New();
    activeWorkers->addWorker("worker2", worker2);

    {

        SReaderTest::sptr readerTestSrv = ::fwServices::factory::New<SReaderTest>();
        ::fwServices::OSR::registerService(readerTestSrv);
        readerTestSrv->registerInOut(buffer, IBasicTest::s_BUFFER_INOUT);

        SShowTest::sptr showTestSrv = ::fwServices::factory::New<SShowTest>();
        ::fwServices::OSR::registerService(showTestSrv);
        showTestSrv->registerInOut(buffer, IBasicTest::s_BUFFER_INOUT);
        showTestSrv->setWorker(worker1);

        buffer->signal(::fwData::Object::s_MODIFIED_SIG)->connect(showTestSrv->slot(IService::s_UPDATE_SLOT));

        readerTestSrv->start();
        showTestSrv->start();

        readerTestSrv->update().wait();

        IService::SharedFutureType stopReaderFuture = readerTestSrv->stop();
        IService::SharedFutureType stopShowFuture   = showTestSrv->stop();
        stopReaderFuture.wait();
        stopShowFuture.wait();

        CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_receiveCount);

        buffer->signal(::fwData::Object::s_MODIFIED_SIG)->disconnect(showTestSrv->slot(IService::s_UPDATE_SLOT));

        ::fwServices::OSR::unregisterService(readerTestSrv);
        ::fwServices::OSR::unregisterService(showTestSrv);
    }

    {
        SReaderTest::sptr readerTestSrv = ::fwServices::factory::New<SReaderTest>();
        ::fwServices::OSR::registerService(readerTestSrv);
        readerTestSrv->registerInOut(buffer, IBasicTest::s_BUFFER_INOUT);

        SReaderTest::sptr reader2TestSrv = ::fwServices::factory::New<SReaderTest>();
        ::fwServices::OSR::registerService(reader2TestSrv);
        reader2TestSrv->registerInOut(buffer, IBasicTest::s_BUFFER_INOUT);
        reader2TestSrv->setWorker(worker2);

        SShowTest::sptr showTestSrv = ::fwServices::factory::New<SShowTest>();
        ::fwServices::OSR::registerService(showTestSrv);
        showTestSrv->registerInOut(buffer, IBasicTest::s_BUFFER_INOUT, true);
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

        ::fwServices::OSR::unregisterService(readerTestSrv);
        ::fwServices::OSR::unregisterService(reader2TestSrv);
        ::fwServices::OSR::unregisterService(showTestSrv);
    }
    activeWorkers->clearRegistry();
}

//------------------------------------------------------------------------------

void SlotsSignalsTest::comServiceToServiceTest()
{
    SLM_TRACE_FUNC();
    Buffer::sptr buffer = Buffer::New();

    registry::ActiveWorkers::sptr activeWorkers = registry::ActiveWorkers::getDefault();
    activeWorkers->initRegistry();
    ::fwThread::Worker::sptr worker1 = ::fwThread::Worker::New();
    activeWorkers->addWorker("worker1", worker1);

    SReader2Test::sptr readerTestSrv = ::fwServices::factory::New<SReader2Test>();
    ::fwServices::OSR::registerService(readerTestSrv);
    readerTestSrv->registerInOut(buffer, IBasicTest::s_BUFFER_INOUT);

    SShowTest::sptr showTestSrv = ::fwServices::factory::New<SShowTest>();
    ::fwServices::OSR::registerService(showTestSrv);
    showTestSrv->registerInOut(buffer, IBasicTest::s_BUFFER_INOUT);
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

    ::fwServices::OSR::unregisterService(readerTestSrv);
    ::fwServices::OSR::unregisterService(showTestSrv);

    activeWorkers->clearRegistry();
}

//------------------------------------------------------------------------------

void SlotsSignalsTest::blockConnectionTest()
{
    SLM_TRACE_FUNC();
    Buffer::sptr buffer = Buffer::New();

    registry::ActiveWorkers::sptr activeWorkers = registry::ActiveWorkers::getDefault();
    activeWorkers->initRegistry();
    ::fwThread::Worker::sptr worker1 = ::fwThread::Worker::New();
    activeWorkers->addWorker("worker1", worker1);

    SReaderTest::sptr readerTestSrv = ::fwServices::factory::New<SReaderTest>();
    ::fwServices::OSR::registerService(readerTestSrv);
    readerTestSrv->registerInOut(buffer, IBasicTest::s_BUFFER_INOUT);

    SShow2Test::sptr showTestSrv = ::fwServices::factory::New<SShow2Test>();
    ::fwServices::OSR::registerService(showTestSrv);
    showTestSrv->registerInOut(buffer, IBasicTest::s_BUFFER_INOUT);
    showTestSrv->setWorker(worker1);

    ::fwCom::Connection connection;
    connection = buffer->signal(::fwData::Object::s_MODIFIED_SIG)->
                 connect(showTestSrv->slot(SShow2Test::s_UPDATE_BUFFER_SLOT));

    readerTestSrv->start();
    showTestSrv->start();

    readerTestSrv->update();

    std::this_thread::sleep_for( std::chrono::seconds(8));

    IService::SharedFutureType stopReaderFuture = readerTestSrv->stop();
    IService::SharedFutureType stopShowFuture   = showTestSrv->stop();
    stopReaderFuture.wait();
    stopShowFuture.wait();

    connection.disconnect();

    CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_receiveCount);

    activeWorkers->clearRegistry();
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
