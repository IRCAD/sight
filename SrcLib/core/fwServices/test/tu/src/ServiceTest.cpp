/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "ServiceTest.hpp"

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwCom/helper/SigSlotConnection.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/TimeStamp.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Object.hpp>

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/helper.hpp>

#include <fwTest/helper/wait.hpp>

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
    // unregister the services that have not been unregistered because a test failed.

    auto services = ::fwServices::OSR::getServices< ::fwServices::IService >();
    for (auto srv: services)
    {
        if (srv->isStarted())
        {
            srv->stop();
        }
        ::fwServices::OSR::unregisterService(srv);
    }
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceCreation()
{
#ifndef REMOVE_DEPRECATED
    FW_DEPRECATED_MSG("Deprecated test", "20.0");
    ::fwData::Integer::sptr obj = ::fwData::Integer::New();
    ::fwServices::IService::sptr service;

    // Test if the object support the service
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(),
                                                                                  "::fwServices::ut::TestService") );

    // deprecated: Test adding service
    ::fwServices::add(obj, "::fwServices::ut::TestService", "::fwServices::ut::TestServiceImplementation");
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::fwServices::ut::TestService") );

    // Test getting the service its object
    service = ::fwServices::get(obj, "::fwServices::ut::TestService");
    CPPUNIT_ASSERT(service);
    CPPUNIT_ASSERT_EQUAL(obj, service->getObject< ::fwData::Integer >());

    // Test erasing service
    ::fwServices::OSR::unregisterService(service);
    CPPUNIT_ASSERT( ::fwServices::OSR::has(obj, "::fwServices::ut::TestService") == false );
#endif
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceConfiguration()
{
    // Test adding service
    auto srv  = ::fwServices::add< ::fwServices::ut::TestService >("::fwServices::ut::TestServiceImplementation");
    auto srv2 = ::fwServices::add< ::fwServices::ut::TestService >("::fwServices::ut::TestServiceImplementation");

    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::ConfigurationStatus::UNCONFIGURED, srv->getConfigurationStatus());
    CPPUNIT_ASSERT_EQUAL(TestService::s_UNCONFIGURED, srv->getOption());
    srv->configure();
    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::ConfigurationStatus::CONFIGURED, srv->getConfigurationStatus());
    CPPUNIT_ASSERT_EQUAL(TestService::s_NOT_DEFINED, srv->getOption());

    const std::string OPTION1 = "configuredOption1";
    const std::string OPTION2 = "configuredOption2";

    ::fwServices::IService::ConfigType config;
    config.add(TestService::s_OPTION_KEY, OPTION1);

    srv->setConfiguration(config);
    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::ConfigurationStatus::UNCONFIGURED, srv->getConfigurationStatus());

    srv->configure();

    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::ConfigurationStatus::CONFIGURED, srv->getConfigurationStatus());
    CPPUNIT_ASSERT_EQUAL(OPTION1, srv->getOption());

    ::fwServices::IService::ConfigType config2;
    config2.add(TestService::s_OPTION_KEY, OPTION2);

    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::ConfigurationStatus::UNCONFIGURED, srv2->getConfigurationStatus());
    CPPUNIT_ASSERT_EQUAL(TestService::s_UNCONFIGURED, srv2->getOption());

    srv2->configure(config2);

    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::ConfigurationStatus::CONFIGURED, srv2->getConfigurationStatus());
    CPPUNIT_ASSERT_EQUAL(OPTION2, srv2->getOption());

    // Test erasing service
    ::fwServices::OSR::unregisterService(srv);
    ::fwServices::OSR::unregisterService(srv2);
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceCreationWithMultipleData()
{
    const std::string dataKey1 = "data1";
    const std::string dataKey2 = "data2";
    const std::string dataKey3 = "data3";
    ::fwData::Integer::sptr obj1 = ::fwData::Integer::New();
    ::fwData::Integer::sptr obj2 = ::fwData::Integer::New();
    ::fwData::Integer::sptr obj3 = ::fwData::Integer::New();

    // Test if the object support the service
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj1->getClassname(),
                                                                                  "::fwServices::ut::TestService") );
    typedef ::fwServices::IService::AccessType AccessType;

    // Test adding service
    ::fwServices::IService::sptr srv = ::fwServices::add("::fwServices::ut::TestServiceImplementation");
    srv->registerInOut(obj1, dataKey1);
    CPPUNIT_ASSERT_EQUAL(true, ::fwServices::OSR::isRegistered(dataKey1, AccessType::INOUT, srv));
    CPPUNIT_ASSERT(obj1 == ::fwServices::OSR::getRegistered(dataKey1, AccessType::INOUT, srv));
    CPPUNIT_ASSERT_EQUAL(false, ::fwServices::OSR::isRegistered(dataKey2, AccessType::INOUT, srv));
    CPPUNIT_ASSERT_EQUAL(false, ::fwServices::OSR::isRegistered(dataKey3, AccessType::INOUT, srv));

    // Test getting the service object
    CPPUNIT_ASSERT_EQUAL(obj1, srv->getInOut< ::fwData::Integer >(dataKey1));

    srv->registerObject(obj2, dataKey2, AccessType::INOUT);
    CPPUNIT_ASSERT_EQUAL(true, ::fwServices::OSR::isRegistered(dataKey2, AccessType::INOUT, srv));
    CPPUNIT_ASSERT(obj2 == ::fwServices::OSR::getRegistered(dataKey2, AccessType::INOUT, srv));

    // Test getting the service object
    CPPUNIT_ASSERT_EQUAL(obj2, srv->getInOut< ::fwData::Integer >(dataKey2));

    srv->registerInput(obj3, dataKey3);
    CPPUNIT_ASSERT_EQUAL(true, ::fwServices::OSR::isRegistered(dataKey3, AccessType::INPUT, srv));
    CPPUNIT_ASSERT(obj3 == ::fwServices::OSR::getRegistered(dataKey3, AccessType::INPUT, srv));

    // Test getting the service object
    CPPUNIT_ASSERT(obj3 == srv->getInput< ::fwData::Integer >(dataKey3));

    // Test unregistering the objects
    srv->unregisterInOut(dataKey1);
    CPPUNIT_ASSERT_EQUAL(false, ::fwServices::OSR::isRegistered(dataKey1, AccessType::INOUT, srv));
    CPPUNIT_ASSERT_EQUAL(true, ::fwServices::OSR::isRegistered(dataKey2, AccessType::INOUT, srv));
    CPPUNIT_ASSERT_EQUAL(true, ::fwServices::OSR::isRegistered(dataKey3, AccessType::INPUT, srv));

    srv->unregisterInOut(dataKey2);
    CPPUNIT_ASSERT_EQUAL(false, ::fwServices::OSR::isRegistered(dataKey1, AccessType::INOUT, srv));
    CPPUNIT_ASSERT_EQUAL(false, ::fwServices::OSR::isRegistered(dataKey2, AccessType::INOUT, srv));
    CPPUNIT_ASSERT_EQUAL(true, ::fwServices::OSR::isRegistered(dataKey3, AccessType::INPUT, srv));

    srv->unregisterInput(dataKey3);
    CPPUNIT_ASSERT_EQUAL(false, ::fwServices::OSR::isRegistered(dataKey1, AccessType::INOUT, srv));
    CPPUNIT_ASSERT_EQUAL(false, ::fwServices::OSR::isRegistered(dataKey2, AccessType::INOUT, srv));
    CPPUNIT_ASSERT_EQUAL(false, ::fwServices::OSR::isRegistered(dataKey3, AccessType::INPUT, srv));

    // Test erasing service
    ::fwServices::OSR::unregisterService(srv);
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceCreationWithTemplateMethods()
{
    const std::string dataKey = "data";
    ::fwData::Integer::sptr obj = ::fwData::Integer::New();

    // Test if the object support the service
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(),
                                                                                  "::fwServices::ut::TestService") );

    // Test adding service
    ::fwServices::ut::TestService::sptr srv =
        ::fwServices::add< ::fwServices::ut::TestService > ("::fwServices::ut::TestServiceImplementation");
    srv->registerInOut(obj, dataKey);
    CPPUNIT_ASSERT(::fwServices::OSR::isRegistered(dataKey, ::fwServices::IService::AccessType::INOUT, srv));
    CPPUNIT_ASSERT(obj == ::fwServices::OSR::getRegistered(dataKey, ::fwServices::IService::AccessType::INOUT, srv));

    // Test getting the service its object
    CPPUNIT_ASSERT_EQUAL(obj, srv->getInOut< ::fwData::Integer >(dataKey));

    srv->unregisterInOut(dataKey);
    CPPUNIT_ASSERT_EQUAL(false,
                         ::fwServices::OSR::isRegistered(dataKey, ::fwServices::IService::AccessType::INOUT, srv));

    // Test erasing service
    ::fwServices::OSR::unregisterService(srv);
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceCreationWithUUID()
{
    const std::string myUUID  = "myUUID";
    const std::string myUUID2 = "myUUID2";
    const std::string myUUID3 = "myUUID3";
    const std::string dataKey = "data";
    size_t nbServices         = 0;

    ::fwData::Integer::sptr obj = ::fwData::Integer::New();
    ::fwServices::IService::sptr service;
    ::fwServices::IService::sptr service2;

    // Test if the object support the service
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(),
                                                                                  "::fwServices::ut::TestService") );

    // Test adding service
    service = ::fwServices::add("::fwServices::ut::TestServiceImplementation", myUUID);
    CPPUNIT_ASSERT(service);
    service->registerInOut(obj, dataKey);

    service2 = ::fwServices::add("::fwServices::ut::TestServiceImplementation", myUUID2);
    CPPUNIT_ASSERT(service2);
    service2->registerInOut(obj, dataKey);

    nbServices = 2;
    CPPUNIT_ASSERT(::fwTools::fwID::exist(myUUID) );
    CPPUNIT_ASSERT(::fwTools::fwID::exist(myUUID2));

    // Test getting the service its object
    ::fwServices::IService::sptr service2bis = ::fwServices::get(myUUID2);
    CPPUNIT_ASSERT(service2bis);
    CPPUNIT_ASSERT_EQUAL(obj, service2bis->getInOut< ::fwData::Integer >(dataKey));
    CPPUNIT_ASSERT_EQUAL(myUUID2, service2bis->getID());
    CPPUNIT_ASSERT( !::fwTools::fwID::exist(myUUID3) );
    CPPUNIT_ASSERT_EQUAL( nbServices, ::fwServices::OSR::getServices("::fwServices::ut::TestService").size() );

    // Test erasing service
    ::fwServices::OSR::unregisterService(service);
    nbServices--;
    CPPUNIT_ASSERT_EQUAL( nbServices, ::fwServices::OSR::getServices("::fwServices::ut::TestService").size() );
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
    service = ::fwServices::add< ::fwServices::ut::TestService >("::fwServices::ut::TestServiceImplementation", myUUID);
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

//------------------------------------------------------------------------------

void ServiceTest::testCommunication()
{
    registry::ActiveWorkers::sptr activeWorkers = registry::ActiveWorkers::getDefault();
    activeWorkers->initRegistry();

    const std::string EVENT        = "EVENT";
    const std::string dataKey      = "data";
    const std::string service1UUID = "service1UUID";
    const std::string service2UUID = "service2UUID";

    ::fwData::Composite::sptr obj = ::fwData::Composite::New();
    ::fwServices::ut::TestService::sptr service1;
    ::fwServices::ut::TestService::sptr service2;

    // Add services
    ::fwServices::add("::fwServices::ut::TestServiceImplementation", service1UUID);
    service1 = ::fwServices::ut::TestService::dynamicCast( ::fwServices::get(service1UUID) );
    CPPUNIT_ASSERT(service1);
    service1->registerInput(obj, dataKey, true);

    service2 = ::fwServices::add< ::fwServices::ut::TestService >("::fwServices::ut::TestServiceImplementation",
                                                                  service2UUID);
    CPPUNIT_ASSERT(service2);
    service2->registerInput(obj, dataKey, true);

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

    fwTestWaitMacro(receiver1->m_started && receiver2->m_started)
    CPPUNIT_ASSERT_EQUAL(true, receiver1->m_started);
    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_stopped);
    CPPUNIT_ASSERT_EQUAL(true, receiver2->m_started);
    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_stopped);

    // Register communication channel
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

    fwTestWaitMacro(receiver1->m_updated && receiver2->m_updated)
    CPPUNIT_ASSERT_EQUAL(true, receiver1->m_started);
    CPPUNIT_ASSERT_EQUAL(true, receiver1->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_stopped);
    CPPUNIT_ASSERT_EQUAL(true, receiver2->m_started);
    CPPUNIT_ASSERT_EQUAL(true, receiver2->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_stopped);

    // Test if service2 has received the message
    service1->stop().wait();
    service2->stop().wait();

    fwTestWaitMacro(receiver1->m_stopped && receiver2->m_stopped)
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
    catch(::fwCore::Exception& e)
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
    catch(::fwCore::Exception& e)
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

void ServiceTest::testWithInAndOut()
{
    ::fwData::Integer::sptr obj                         = ::fwData::Integer::New(18);
    ::fwData::Integer::sptr obj2                        = ::fwData::Integer::New(23);
    ::fwServices::ut::TestServiceWithData::sptr service =
        ::fwServices::add< ::fwServices::ut::TestServiceWithData >("::fwServices::ut::TestServiceWithData");

    CPPUNIT_ASSERT(service);
    CPPUNIT_ASSERT_EQUAL(false, service->hasAllRequiredObjects());
    service->registerInput(obj, ::fwServices::ut::TestServiceWithData::s_INPUT);
    CPPUNIT_ASSERT_EQUAL(true, service->hasAllRequiredObjects());

    service->registerGroup();
    CPPUNIT_ASSERT_EQUAL(false, service->hasAllRequiredObjects());
    service->registerInOut(obj, KEY_GROUP_NAME(::fwServices::ut::TestServiceWithData::s_INOUT_GROUP, 0));
    CPPUNIT_ASSERT_EQUAL(false, service->hasAllRequiredObjects());
    service->registerInOut(obj2, KEY_GROUP_NAME(::fwServices::ut::TestServiceWithData::s_INOUT_GROUP, 1));
    CPPUNIT_ASSERT_EQUAL(true, service->hasAllRequiredObjects());

    service->start();
    CPPUNIT_ASSERT(service->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, ::fwServices::OSR::isRegistered(::fwServices::ut::TestServiceWithData::s_INPUT,
                                                               ::fwServices::IService::AccessType::INPUT, service));
    service->update();
    CPPUNIT_ASSERT_EQUAL(true, ::fwServices::OSR::isRegistered(::fwServices::ut::TestServiceWithData::s_OUTPUT,
                                                               ::fwServices::IService::AccessType::OUTPUT, service));
    ::fwData::Object::csptr output =
        ::fwServices::OSR::getRegistered(::fwServices::ut::TestServiceWithData::s_OUTPUT,
                                         ::fwServices::IService::AccessType::OUTPUT, service);
    CPPUNIT_ASSERT(output);
    ::fwData::Integer::csptr outInteger = ::fwData::Integer::dynamicCast(output);
    CPPUNIT_ASSERT(outInteger);

    CPPUNIT_ASSERT_EQUAL(obj->value(), outInteger->value());

    ::fwData::Integer::csptr outInteger2 = service->getOutput< ::fwData::Integer >(
        ::fwServices::ut::TestServiceWithData::s_OUTPUT);
    CPPUNIT_ASSERT(outInteger2);

    CPPUNIT_ASSERT_EQUAL(obj->value(), outInteger2->value());

    service->stop();

    ::fwData::Integer::csptr nullInteger = service->getOutput< ::fwData::Integer >(
        ::fwServices::ut::TestServiceWithData::s_OUTPUT);
    CPPUNIT_ASSERT(nullptr == nullInteger);

    CPPUNIT_ASSERT_EQUAL(false, ::fwServices::OSR::isRegistered(::fwServices::ut::TestServiceWithData::s_OUTPUT,
                                                                ::fwServices::IService::AccessType::OUTPUT, service));

    CPPUNIT_ASSERT_EQUAL(true, ::fwServices::OSR::isRegistered(::fwServices::ut::TestServiceWithData::s_INPUT,
                                                               ::fwServices::IService::AccessType::INPUT, service));
    service->unregisterInput(::fwServices::ut::TestServiceWithData::s_INPUT);
    CPPUNIT_ASSERT_EQUAL(false, ::fwServices::OSR::isRegistered(::fwServices::ut::TestServiceWithData::s_INPUT,
                                                                ::fwServices::IService::AccessType::INPUT, service));
    ::fwServices::OSR::unregisterService(service);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
