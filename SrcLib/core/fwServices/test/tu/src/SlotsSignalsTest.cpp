/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/chrono/duration.hpp>

#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTest/Exception.hpp>

#include "SlotsSignalsTest.hpp"
#include "SlotsSignalsStuff.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::SlotsSignalsTest );

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

    Buffer::sptr buffer = Buffer::New();
    Buffer::sptr buffer2 = Buffer::New();

    ActiveWorkers::sptr activeWorkers = ActiveWorkers::getDefault();
    activeWorkers->initRegistry();

    SBasicTest::sptr basicTestSrv = ::fwServices::factory::New<SBasicTest>();
    ::fwServices::OSR::registerService(buffer, basicTestSrv);

    IService::SharedFutureType startFuture = basicTestSrv->start();
    CPPUNIT_ASSERT(basicTestSrv->getStatus() != IService::STARTED);
    startFuture.wait();
    CPPUNIT_ASSERT(basicTestSrv->getStatus() == IService::STARTED);

    IService::SharedFutureType updateFuture = basicTestSrv->update();
    CPPUNIT_ASSERT(basicTestSrv->m_updateFinished == false);
    updateFuture.wait();
    CPPUNIT_ASSERT(basicTestSrv->m_updateFinished == true);

    IService::SharedFutureType swapFuture = basicTestSrv->swap(buffer2);
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
    activeWorkers->addWorker("worker1",worker1);
    ::fwThread::Worker::sptr worker2 = ::fwThread::Worker::New();
    activeWorkers->addWorker("worker2",worker2);

    {

        SReaderTest::sptr readerTestSrv = ::fwServices::factory::New<SReaderTest>();
        ::fwServices::OSR::registerService(buffer, readerTestSrv);

        SShowTest::sptr showTestSrv = ::fwServices::factory::New<SShowTest>();
        ::fwServices::OSR::registerService(buffer, showTestSrv);
        showTestSrv->setWorker(worker1);

        buffer->signal(::fwData::Object::s_OBJECT_MODIFIED_SIG)->connect(showTestSrv->slot(IService::s_RECEIVE_SLOT));

        readerTestSrv->start();
        showTestSrv->start();

        readerTestSrv->update().wait();

        IService::SharedFutureType stopReaderFuture = readerTestSrv->stop();
        IService::SharedFutureType stopShowFuture = showTestSrv->stop();
        stopReaderFuture.wait();
        stopShowFuture.wait();

        CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_receiveCount);

        buffer->signal(::fwData::Object::s_OBJECT_MODIFIED_SIG)->disconnect(showTestSrv->slot(IService::s_RECEIVE_SLOT));

        ::fwServices::OSR::unregisterService(readerTestSrv);
        ::fwServices::OSR::unregisterService(showTestSrv);
    }

    {
        SReaderTest::sptr readerTestSrv = ::fwServices::factory::New<SReaderTest>();
        ::fwServices::OSR::registerService(buffer, readerTestSrv);

        SReaderTest::sptr reader2TestSrv = ::fwServices::factory::New<SReaderTest>();
        ::fwServices::OSR::registerService(buffer, reader2TestSrv);
        reader2TestSrv->setWorker(worker2);

        SShowTest::sptr showTestSrv = ::fwServices::factory::New<SShowTest>();
        ::fwServices::OSR::registerService(buffer, showTestSrv);
        showTestSrv->setWorker(worker1);

        buffer->signal(::fwData::Object::s_OBJECT_MODIFIED_SIG)->connect(showTestSrv->slot(IService::s_RECEIVE_SLOT));

        readerTestSrv->start();
        reader2TestSrv->start();
        showTestSrv->start();

        IService::SharedFutureType updateReaderFuture = readerTestSrv->update();
        IService::SharedFutureType updateReader2Future = reader2TestSrv->update();
        updateReaderFuture.wait();
        updateReader2Future.wait();

        IService::SharedFutureType stopReaderFuture = readerTestSrv->stop();
        IService::SharedFutureType stopReader2Future = reader2TestSrv->stop();
        IService::SharedFutureType stopShowFuture = showTestSrv->stop();
        stopReaderFuture.wait();
        stopReader2Future.wait();
        stopShowFuture.wait();

        buffer->signal(::fwData::Object::s_OBJECT_MODIFIED_SIG)->disconnect(showTestSrv->slot(IService::s_RECEIVE_SLOT));

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
    activeWorkers->addWorker("worker1",worker1);

    SReader2Test::sptr readerTestSrv = ::fwServices::factory::New<SReader2Test>();
    ::fwServices::OSR::registerService(buffer, readerTestSrv);

    SShowTest::sptr showTestSrv = ::fwServices::factory::New<SShowTest>();
    ::fwServices::OSR::registerService(buffer, showTestSrv);
    showTestSrv->setWorker(worker1);

    readerTestSrv->signal(SReader2Test::s_CHANGED_SIG)->connect(showTestSrv->slot(SShowTest::s_CHANGE_SLOT));

    readerTestSrv->start();
    showTestSrv->start();

    readerTestSrv->update().wait();

    IService::SharedFutureType stopReaderFuture = readerTestSrv->stop();
    IService::SharedFutureType stopShowFuture = showTestSrv->stop();
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
    activeWorkers->addWorker("worker1",worker1);

    SReaderTest::sptr readerTestSrv = ::fwServices::factory::New<SReaderTest>();
    ::fwServices::OSR::registerService(buffer, readerTestSrv);

    SShow2Test::sptr showTestSrv = ::fwServices::factory::New<SShow2Test>();
    ::fwServices::OSR::registerService(buffer, showTestSrv);
    showTestSrv->setWorker(worker1);

    ::fwCom::Connection connection;
    connection = buffer->signal(::fwData::Object::s_OBJECT_MODIFIED_SIG)->
                                            connect(showTestSrv->slot(IService::s_RECEIVE_SLOT));

    readerTestSrv->start();
    showTestSrv->start();

    readerTestSrv->update();

    ::boost::this_thread::sleep_for( ::boost::chrono::seconds(8));

    IService::SharedFutureType stopReaderFuture = readerTestSrv->stop();
    IService::SharedFutureType stopShowFuture = showTestSrv->stop();
    stopReaderFuture.wait();
    stopShowFuture.wait();

    connection.disconnect();

    CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_receiveCount);

    activeWorkers->clearRegistry();
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
