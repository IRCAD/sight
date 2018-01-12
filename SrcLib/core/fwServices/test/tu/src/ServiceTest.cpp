/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ServiceTest.hpp"

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwCore/TimeStamp.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Object.hpp>

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/helper.hpp>

#include <fwThread/Worker.hpp>

#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::ServiceTest );

//------------------------------------------------------------------------------

namespace fwServices
{
namespace ut
{

//------------------------------------------------------------------------------

void ServiceTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ServiceTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceCreation()
{
    ::fwData::Integer::sptr obj = ::fwData::Integer::New();
    ::fwServices::IService::sptr service;

    // Test if the object support the service
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(),
                                                                                  "::fwServices::ut::TestService") );

    // Test adding service
    ::fwServices::add(obj, "::fwServices::ut::TestService", "::fwServices::ut::TestServiceImplementation");
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::fwServices::ut::TestService") );

    // Test getting the service its object
    service = ::fwServices::get(obj, "::fwServices::ut::TestService");
    CPPUNIT_ASSERT(service);
    CPPUNIT_ASSERT_EQUAL(obj, service->getObject< ::fwData::Integer >());

    // Test erasing service
    ::fwServices::OSR::unregisterService(service);
    CPPUNIT_ASSERT( ::fwServices::OSR::has(obj, "::fwServices::ut::TestService") == false );
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceCreationWithMultipleData()
{
    ::fwData::Integer::sptr obj = ::fwData::Integer::New();
    ::fwServices::IService::sptr service;

    // Test if the object support the service
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(),
                                                                                  "::fwServices::ut::TestService") );

    // Test adding service
    ::fwServices::add(obj, "::fwServices::ut::TestService", "::fwServices::ut::TestServiceImplementation");
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::fwServices::ut::TestService") );

    // Test getting the service its object
    service = ::fwServices::get(obj, "::fwServices::ut::TestService");
    CPPUNIT_ASSERT(service);
    CPPUNIT_ASSERT_EQUAL(obj, service->getObject< ::fwData::Integer >());

    // Test erasing service
    ::fwServices::OSR::unregisterService(service);
    CPPUNIT_ASSERT( ::fwServices::OSR::has(obj, "::fwServices::ut::TestService") == false );
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceCreationWithTemplateMethods()
{
    ::fwData::Integer::sptr obj = ::fwData::Integer::New();
    ::fwServices::ut::TestService::sptr service;

    // Test if the object support the service
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(),
                                                                                  "::fwServices::ut::TestService") );

    // Test adding service
    ::fwServices::add< ::fwServices::ut::TestService >(obj, "::fwServices::ut::TestServiceImplementation");
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::fwServices::ut::TestService") );

    // Test getting the service its object
    service = ::fwServices::get< ::fwServices::ut::TestService >(obj);
    CPPUNIT_ASSERT(service);
    CPPUNIT_ASSERT_EQUAL(obj, service->getObject< ::fwData::Integer >());

    // Test erasing service
    ::fwServices::OSR::unregisterService(service);
    CPPUNIT_ASSERT( ::fwServices::OSR::has(obj, "::fwServices::ut::TestService") == false );
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceCreationWithUUID()
{
    const std::string myUUID  = "myUUID";
    const std::string myUUID2 = "myUUID2";
    const std::string myUUID3 = "myUUID3";
    size_t nbServices         = 0;

    ::fwData::Integer::sptr obj = ::fwData::Integer::New();
    ::fwServices::IService::sptr service;
    ::fwServices::IService::sptr service2;

    // Test if the object support the service
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(),
                                                                                  "::fwServices::ut::TestService") );

    // Test adding service
    service = ::fwServices::add(obj, "::fwServices::ut::TestService", "::fwServices::ut::TestServiceImplementation",
                                myUUID);
    ::fwServices::add(obj, "::fwServices::ut::TestService", "::fwServices::ut::TestServiceImplementation", myUUID2);
    nbServices = 2;
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::fwServices::ut::TestService") );
    CPPUNIT_ASSERT(::fwTools::fwID::exist(myUUID) );
    CPPUNIT_ASSERT(::fwTools::fwID::exist(myUUID2));

    // Test getting the service its object
    service2 = ::fwServices::get(myUUID2);
    CPPUNIT_ASSERT(service);
    CPPUNIT_ASSERT(service2);
    CPPUNIT_ASSERT_EQUAL(obj, service2->getObject< ::fwData::Integer >());
    CPPUNIT_ASSERT_EQUAL(myUUID2, service2->getID());
    CPPUNIT_ASSERT( !::fwTools::fwID::exist(myUUID3) );
    CPPUNIT_ASSERT_EQUAL( nbServices, ::fwServices::OSR::getServices(obj, "::fwServices::ut::TestService").size() );

    // Test erasing service
    ::fwServices::OSR::unregisterService(service);
    nbServices--;
    CPPUNIT_ASSERT_EQUAL( nbServices, ::fwServices::OSR::getServices(obj, "::fwServices::ut::TestService").size() );
    ::fwServices::OSR::unregisterService(service2);
}

//------------------------------------------------------------------------------

void ServiceTest::testStartStopUpdate()
{
    const std::string myUUID = "myUUID";

    ::fwData::Integer::sptr obj = ::fwData::Integer::New();
    ::fwServices::ut::TestService::sptr service;

    // Add service
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(),
                                                                                  "::fwServices::ut::TestService") );
    ::fwServices::add(obj, "::fwServices::ut::TestService", "::fwServices::ut::TestServiceImplementation", myUUID);
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::fwServices::ut::TestService") );

    // Get service
    service = ::fwServices::ut::TestService::dynamicCast( ::fwServices::get(myUUID) );
    CPPUNIT_ASSERT(service);

    // Service must be stop when it is created
    CPPUNIT_ASSERT(service->isStopped());
    CPPUNIT_ASSERT(!service->isStarted());

    // Start service
    service->start();
    CPPUNIT_ASSERT(service->isStarted());
    CPPUNIT_ASSERT(!service->isStopped());

    // Update service
    service->update();
    CPPUNIT_ASSERT(service->getIsUpdated());

    // Stop service
    service->stop();
    CPPUNIT_ASSERT(service->isStopped());
    CPPUNIT_ASSERT(!service->isStarted());

    // Erase Service
    ::fwServices::OSR::unregisterService(service);
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::fwServices::ut::TestService") == false );
}

//------------------------------------------------------------------------------

void ServiceTest::testStartStopUpdateExceptions()
{
    // Test on the same worker
    {
        auto service = ::fwServices::add< ::fwServices::ut::TestService>("::fwServices::ut::TestServiceImplementation");
        ServiceTest::startStopUpdateExceptions(service);
    }
    // Test on a different worker
    {
        auto service = ::fwServices::add< ::fwServices::ut::TestService>("::fwServices::ut::TestServiceImplementation");
        service->setWorker(::fwThread::Worker::New());
        ServiceTest::startStopUpdateExceptions(service);
    }

}

//------------------------------------------------------------------------------

struct TestServiceSignals : public ::fwCom::HasSlots
{
    typedef std::shared_ptr< TestServiceSignals > sptr;

    TestServiceSignals() :
        m_started(false),
        m_updated(false),
        m_stopped(false)
    {
        newSlot("start", &TestServiceSignals::start, this);
        newSlot("update", &TestServiceSignals::update, this);
        newSlot("stop", &TestServiceSignals::stop, this);

        m_worker = ::fwThread::Worker::New();
        m_slots.setWorker(m_worker);
    }

    virtual ~TestServiceSignals();

    //------------------------------------------------------------------------------

    void start()
    {
        m_started = true;
    }
    //------------------------------------------------------------------------------

    void update()
    {
        m_updated = true;
    }
    //------------------------------------------------------------------------------

    void stop()
    {
        m_stopped = true;
    }

    ::fwThread::Worker::sptr m_worker;
    bool m_started;
    bool m_updated;
    bool m_stopped;

};

//------------------------------------------------------------------------------

TestServiceSignals::~TestServiceSignals()
{
}

// Wait at worst 1s for a given condition
#define WAIT(cond) \
    ::fwCore::TimeStamp BOOST_PP_CAT(timeStamp, __LINE__); \
    BOOST_PP_CAT(timeStamp, __LINE__).setLifePeriod(1000); \
    BOOST_PP_CAT(timeStamp, __LINE__).modified(); \
    while(!(cond) && !BOOST_PP_CAT(timeStamp, __LINE__).periodExpired()) \
    { \
        std::this_thread::sleep_for( std::chrono::milliseconds(10)); \
    }

//------------------------------------------------------------------------------

void ServiceTest::testCommunication()
{
    registry::ActiveWorkers::sptr activeWorkers = registry::ActiveWorkers::getDefault();
    activeWorkers->initRegistry();

    const std::string EVENT        = "EVENT";
    const std::string service1UUID = "service1UUID";
    const std::string service2UUID = "service2UUID";

    ::fwData::Composite::sptr obj = ::fwData::Composite::New();
    ::fwServices::ut::TestService::sptr service1;
    ::fwServices::ut::TestService::sptr service2;

    // Add services
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(),
                                                                                  "::fwServices::ut::TestService") );
    ::fwServices::add(obj, "::fwServices::ut::TestService", "::fwServices::ut::TestServiceImplementation",
                      service1UUID);
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::fwServices::ut::TestService") );
    service1 = ::fwServices::ut::TestService::dynamicCast( ::fwServices::get(service1UUID) );
    CPPUNIT_ASSERT(service1);

    ::fwServices::add(obj, "::fwServices::ut::TestService", "::fwServices::ut::TestServiceImplementation",
                      service2UUID);
    service2 = ::fwServices::ut::TestService::dynamicCast( ::fwServices::get(service2UUID) );
    CPPUNIT_ASSERT(service2);

    // Object used to check service signals
    TestServiceSignals::sptr receiver1 = std::make_shared< TestServiceSignals>();
    TestServiceSignals::sptr receiver2 = std::make_shared< TestServiceSignals>();

    ::fwCom::helper::SigSlotConnection comHelper;
    comHelper.connect(service1, ::fwServices::IService::s_STARTED_SIG, receiver1, "start");
    comHelper.connect(service1, ::fwServices::IService::s_UPDATED_SIG, receiver1, "update");
    comHelper.connect(service1, ::fwServices::IService::s_STOPPED_SIG, receiver1, "stop");

    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_started);
    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_stopped);

    comHelper.connect(service2, ::fwServices::IService::s_STARTED_SIG, receiver2, "start");
    comHelper.connect(service2, ::fwServices::IService::s_UPDATED_SIG, receiver2, "update");
    comHelper.connect(service2, ::fwServices::IService::s_STOPPED_SIG, receiver2, "stop");

    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_started);
    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_stopped);

    // Start services
    service1->start().wait();
    service2->start().wait();
    CPPUNIT_ASSERT(service1->isStarted());
    CPPUNIT_ASSERT(service2->isStarted());

    WAIT(receiver1->m_started && receiver2->m_started)
    CPPUNIT_ASSERT_EQUAL(true, receiver1->m_started);
    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_stopped);
    CPPUNIT_ASSERT_EQUAL(true, receiver2->m_started);
    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_stopped);

    // Register communication channel
    comHelper.connect( obj, service1, service1->getObjSrvConnections() );
    comHelper.connect( obj, service2, service2->getObjSrvConnections() );
    comHelper.connect( service1, ::fwServices::ut::TestServiceImplementation::s_MSG_SENT_SIG,
                       service2, ::fwServices::ut::TestServiceImplementation::s_UPDATE2_SLOT );

    CPPUNIT_ASSERT(!service2->getIsUpdated2());

    // Service1 send notification
    ::fwServices::ut::TestServiceImplementation::MsgSentSignalType::sptr sig;
    sig = service1->signal< ::fwServices::ut::TestServiceImplementation::MsgSentSignalType >(
        ::fwServices::ut::TestServiceImplementation::s_MSG_SENT_SIG);
    {
        ::fwCom::SlotBase::sptr slot;
        slot = service1->slot( ::fwServices::IService::s_UPDATE_SLOT );
        ::fwCom::Connection::Blocker block(sig->getConnection(slot));
        sig->asyncEmit(EVENT);
    }

    service1->update().wait();
    service2->update().wait();
    CPPUNIT_ASSERT(service2->getIsUpdated2());

    WAIT(receiver1->m_updated && receiver2->m_updated)
    CPPUNIT_ASSERT_EQUAL(true, receiver1->m_started);
    CPPUNIT_ASSERT_EQUAL(true, receiver1->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_stopped);
    CPPUNIT_ASSERT_EQUAL(true, receiver2->m_started);
    CPPUNIT_ASSERT_EQUAL(true, receiver2->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_stopped);

    // Test if service2 has received the message
    service1->stop().wait();
    service2->stop().wait();

    WAIT(receiver1->m_stopped && receiver2->m_stopped)
    CPPUNIT_ASSERT_EQUAL(true, receiver1->m_started);
    CPPUNIT_ASSERT_EQUAL(true, receiver1->m_updated);
    CPPUNIT_ASSERT_EQUAL(true, receiver1->m_stopped);
    CPPUNIT_ASSERT_EQUAL(true, receiver2->m_started);
    CPPUNIT_ASSERT_EQUAL(true, receiver2->m_updated);
    CPPUNIT_ASSERT_EQUAL(true, receiver2->m_stopped);

    comHelper.disconnect();

    ::fwServices::OSR::unregisterService(service1);
    ::fwServices::OSR::unregisterService(service2);

    activeWorkers->clearRegistry();
}

//------------------------------------------------------------------------------

void ServiceTest::startStopUpdateExceptions(TestService::sptr _service)
{
    // Service must be stop when it is created
    CPPUNIT_ASSERT(_service->isStopped());

    // Start service
    _service->start().wait();
    CPPUNIT_ASSERT(_service->isStarted());

    // Stop service
    _service->stop().wait();
    CPPUNIT_ASSERT(_service->isStopped());

    // Start service with exceptions
    _service->setRaiseException(true);
    _service->start().wait();
    CPPUNIT_ASSERT(_service->isStopped());

    // Check we can catch the exception
    bool exceptionCaught = false;
    try
    {
        _service->start().get();
    }
    catch(const ::fwCore::Exception& e)
    {
        exceptionCaught = true;
        CPPUNIT_ASSERT_EQUAL(std::string("start error"), std::string(e.what()));
    }
    CPPUNIT_ASSERT(exceptionCaught);
    CPPUNIT_ASSERT(_service->isStopped());

    // Start service again
    _service->setRaiseException(false);
    _service->start().wait();
    CPPUNIT_ASSERT(_service->isStarted());

    // Update service
    _service->update().wait();
    CPPUNIT_ASSERT(_service->getIsUpdated());
    _service->resetIsUpdated();
    CPPUNIT_ASSERT(!_service->getIsUpdated());

    // Update service with exception caught
    _service->setRaiseException(true);
    exceptionCaught = false;
    try
    {
        _service->update().get();
    }
    catch(::fwCore::Exception e)
    {
        exceptionCaught = true;
        CPPUNIT_ASSERT_EQUAL(std::string("update error"), std::string(e.what()));
    }
    CPPUNIT_ASSERT(exceptionCaught);
    CPPUNIT_ASSERT(!_service->getIsUpdated());

    // Update service without exception caught
    _service->update().wait();
    CPPUNIT_ASSERT(!_service->getIsUpdated());

    // Update service
    _service->setRaiseException(false);
    _service->update().wait();
    CPPUNIT_ASSERT(_service->getIsUpdated());

    // Stop service with exception caught
    _service->setRaiseException(true);
    exceptionCaught = false;
    try
    {
        _service->stop().get();
    }
    catch(::fwCore::Exception e)
    {
        exceptionCaught = true;
        CPPUNIT_ASSERT_EQUAL(std::string("stop error"), std::string(e.what()));
    }
    CPPUNIT_ASSERT(exceptionCaught);
    CPPUNIT_ASSERT(_service->isStarted());

    // Update service without exception caught
    _service->stop().wait();
    CPPUNIT_ASSERT(_service->isStarted());

    _service->setRaiseException(false);
    _service->stop().wait();
    CPPUNIT_ASSERT(_service->isStopped());

    // Erase Service
    ::fwServices::OSR::unregisterService(_service);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
