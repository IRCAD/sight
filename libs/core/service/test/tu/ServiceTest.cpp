/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ServiceTest.hpp"

#include <core/com/helper/SigSlotConnection.hpp>
#include <core/com/Slots.hxx>
#include <core/runtime/EConfigurationElement.hpp>
#include <core/runtime/helper.hpp>
#include <core/thread/Worker.hpp>
#include <core/TimeStamp.hpp>

#include <data/Composite.hpp>
#include <data/Image.hpp>
#include <data/Object.hpp>

#include <service/base.hpp>
#include <service/op/Get.hpp>

#include <utest/wait.hpp>

#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::ServiceTest);

//------------------------------------------------------------------------------

namespace sight::service
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

    auto services = service::OSR::getServices<service::IService>();
    for(auto srv : services)
    {
        if(srv->isStarted())
        {
            srv->stop();
        }

        service::OSR::unregisterService(srv);
    }
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceConfiguration()
{
    // Test adding service
    auto srv  = service::add<service::ut::ISTest>("sight::service::ut::STestNoData");
    auto srv2 = service::add<service::ut::ISTest>("sight::service::ut::STestNoData");

    CPPUNIT_ASSERT_EQUAL(service::IService::ConfigurationStatus::UNCONFIGURED, srv->getConfigurationStatus());
    CPPUNIT_ASSERT_EQUAL(TestService::s_UNCONFIGURED, srv->getOption());
    srv->configure();
    CPPUNIT_ASSERT_EQUAL(service::IService::ConfigurationStatus::CONFIGURED, srv->getConfigurationStatus());
    CPPUNIT_ASSERT_EQUAL(TestService::s_NOT_DEFINED, srv->getOption());

    const std::string OPTION1 = "configuredOption1";
    const std::string OPTION2 = "configuredOption2";

    service::IService::ConfigType config;
    config.add(TestService::s_OPTION_KEY, OPTION1);

    srv->setConfiguration(config);
    CPPUNIT_ASSERT_EQUAL(service::IService::ConfigurationStatus::UNCONFIGURED, srv->getConfigurationStatus());

    srv->configure();

    CPPUNIT_ASSERT_EQUAL(service::IService::ConfigurationStatus::CONFIGURED, srv->getConfigurationStatus());
    CPPUNIT_ASSERT_EQUAL(OPTION1, srv->getOption());

    service::IService::ConfigType config2;
    config2.add(TestService::s_OPTION_KEY, OPTION2);

    CPPUNIT_ASSERT_EQUAL(service::IService::ConfigurationStatus::UNCONFIGURED, srv2->getConfigurationStatus());
    CPPUNIT_ASSERT_EQUAL(TestService::s_UNCONFIGURED, srv2->getOption());

    srv2->configure(config2);

    CPPUNIT_ASSERT_EQUAL(service::IService::ConfigurationStatus::CONFIGURED, srv2->getConfigurationStatus());
    CPPUNIT_ASSERT_EQUAL(OPTION2, srv2->getOption());

    // Test erasing service
    service::OSR::unregisterService(srv);
    service::OSR::unregisterService(srv2);
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceCreationWithMultipleData()
{
    const std::string dataKey1 = "data1";
    const std::string dataKey2 = "data2";
    const std::string dataKey3 = "data3";
    data::Integer::sptr obj1   = data::Integer::New();
    data::Integer::sptr obj2   = data::Integer::New();
    data::Integer::sptr obj3   = data::Integer::New();

    // Test if the object support the service
    CPPUNIT_ASSERT(
        service::extension::Factory::getDefault()->support(
            obj1->getClassname(),
            "sight::service::ut::TestService"
        )
    );

    // Test adding service
    service::IService::sptr srv = service::add("sight::service::ut::STest2Inouts1Input");
    srv->setInOut(obj1, dataKey1);
    CPPUNIT_ASSERT(obj1 == srv->getObject(dataKey1, data::Access::inout));
    CPPUNIT_ASSERT(nullptr == srv->getObject(dataKey2, data::Access::inout));
    CPPUNIT_ASSERT(nullptr == srv->getObject(dataKey3, data::Access::in));

    // Test getting the service object
    CPPUNIT_ASSERT(srv->getInOut<data::Integer>(dataKey1).lock() == obj1);

    srv->setObject(obj2, dataKey2, 0, data::Access::inout, false, false);
    CPPUNIT_ASSERT(obj2 == srv->getObject(dataKey2, data::Access::inout));

    // Test getting the service object
    CPPUNIT_ASSERT(srv->getInOut<data::Integer>(dataKey2).lock() == obj2);

    srv->setInput(obj3, dataKey3);
    CPPUNIT_ASSERT(obj3 == srv->getObject(dataKey3, data::Access::in));

    // Test getting the service object
    CPPUNIT_ASSERT(srv->getInput<data::Integer>(dataKey3).lock() == obj3);

    // Test unregistering the objects
    srv->setInOut(nullptr, dataKey1);
    CPPUNIT_ASSERT(nullptr == srv->getObject(dataKey1, data::Access::inout));
    CPPUNIT_ASSERT(nullptr != srv->getObject(dataKey2, data::Access::inout));
    CPPUNIT_ASSERT(nullptr != srv->getObject(dataKey3, data::Access::in));

    srv->setInOut(nullptr, dataKey2);
    CPPUNIT_ASSERT(nullptr == srv->getObject(dataKey1, data::Access::inout));
    CPPUNIT_ASSERT(nullptr == srv->getObject(dataKey2, data::Access::inout));
    CPPUNIT_ASSERT(nullptr != srv->getObject(dataKey3, data::Access::in));

    srv->setInput(nullptr, dataKey3);
    CPPUNIT_ASSERT(nullptr == srv->getObject(dataKey1, data::Access::inout));
    CPPUNIT_ASSERT(nullptr == srv->getObject(dataKey2, data::Access::inout));
    CPPUNIT_ASSERT(nullptr == srv->getObject(dataKey3, data::Access::in));

    // Test erasing service
    service::remove(srv);
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceCreationWithTemplateMethods()
{
    const std::string dataKey = "data1";
    data::Integer::sptr obj   = data::Integer::New();

    // Test if the object support the service
    CPPUNIT_ASSERT(
        service::extension::Factory::getDefault()->support(
            obj->getClassname(),
            "sight::service::ut::TestService"
        )
    );

    // Test adding service
    auto srv = service::add<service::ut::TestService>("sight::service::ut::STest1Inout");
    srv->setInOut(obj, dataKey);
    CPPUNIT_ASSERT(srv->getObject(dataKey, data::Access::inout));
    CPPUNIT_ASSERT(obj == srv->getObject(dataKey, data::Access::inout));

    // Test getting the service its object
    CPPUNIT_ASSERT(srv->getInOut<data::Integer>(dataKey).lock() == obj);

    srv->setInOut(nullptr, dataKey);
    CPPUNIT_ASSERT(nullptr == srv->getObject(dataKey, data::Access::inout));

    // Test erasing service
    service::remove(srv);
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceCreationWithUUID()
{
    const std::string myUUID  = "myUUID";
    const std::string myUUID2 = "myUUID2";
    const std::string myUUID3 = "myUUID3";
    const std::string dataKey = "data1";
    size_t nbServices         = 0;

    data::Integer::sptr obj = data::Integer::New();
    service::IService::sptr service;
    service::IService::sptr service2;

    // Test if the object support the service
    CPPUNIT_ASSERT(
        service::extension::Factory::getDefault()->support(
            obj->getClassname(),
            "sight::service::ut::TestService"
        )
    );

    // Test adding service
    service = service::add("sight::service::ut::STest1Inout", myUUID);
    CPPUNIT_ASSERT(service);
    service->setInOut(obj, dataKey);

    service2 = service::add("sight::service::ut::STest1Inout", myUUID2);
    CPPUNIT_ASSERT(service2);
    service2->setInOut(obj, dataKey);

    nbServices = 2;
    CPPUNIT_ASSERT(core::tools::fwID::exist(myUUID));
    CPPUNIT_ASSERT(core::tools::fwID::exist(myUUID2));

    // Test getting the service its object
    service::IService::sptr service2bis = service::get(myUUID2);
    CPPUNIT_ASSERT(service2bis);
    CPPUNIT_ASSERT(service2bis->getInOut<data::Integer>(dataKey).lock() == obj);
    CPPUNIT_ASSERT_EQUAL(myUUID2, service2bis->getID());
    CPPUNIT_ASSERT(!core::tools::fwID::exist(myUUID3));
    CPPUNIT_ASSERT_EQUAL(nbServices, service::OSR::getServices("sight::service::ut::TestService").size());

    // Test erasing service
    service::remove(service);
    nbServices--;
    CPPUNIT_ASSERT_EQUAL(nbServices, service::OSR::getServices("sight::service::ut::TestService").size());
    service::remove(service2);
}

//------------------------------------------------------------------------------

void ServiceTest::testStartStopUpdate()
{
    const std::string myUUID = "myUUID";

    data::Integer::sptr obj = data::Integer::New();
    service::ut::TestService::sptr service;

    // Add service
    CPPUNIT_ASSERT(
        service::extension::Factory::getDefault()->support(
            obj->getClassname(),
            "sight::service::ut::TestService"
        )
    );
    service = service::add<service::ut::TestService>("sight::service::ut::STestNoData", myUUID);
    CPPUNIT_ASSERT(service);

    // Service must be stop when it is created
    CPPUNIT_ASSERT(service->isStopped());
    CPPUNIT_ASSERT(!service->isStarted());

    // Start service
    service->start().wait();
    CPPUNIT_ASSERT(service->isStarted());
    CPPUNIT_ASSERT(!service->isStopped());

    // Update service
    service->update().wait();
    CPPUNIT_ASSERT(service->getIsUpdated());

    // Stop service
    service->stop().wait();
    CPPUNIT_ASSERT(service->isStopped());
    CPPUNIT_ASSERT(!service->isStarted());

    // Erase Service
    service::OSR::unregisterService(service);
}

//------------------------------------------------------------------------------

void ServiceTest::testStartStopUpdateExceptions()
{
    // Test on the same worker
    {
        auto service = service::add<service::ut::TestService>("sight::service::ut::STestNoData");
        ServiceTest::startStopUpdateExceptions(service);
    }
    // Test on a different worker
    {
        auto service = service::add<service::ut::TestService>("sight::service::ut::STestNoData");
        auto worker  = core::thread::Worker::New();
        service->setWorker(worker);
        ServiceTest::startStopUpdateExceptions(service);
        worker->stop();
    }
}

//------------------------------------------------------------------------------

struct TestServiceSignals : public core::com::HasSlots
{
    typedef std::shared_ptr<TestServiceSignals> sptr;

    TestServiceSignals() :
        m_started(false),
        m_updated(false),
        m_stopped(false)
    {
        newSlot("start", &TestServiceSignals::start, this);
        newSlot("update", &TestServiceSignals::update, this);
        newSlot("stop", &TestServiceSignals::stop, this);

        m_worker = core::thread::Worker::New();
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

    core::thread::Worker::sptr m_worker;
    bool m_started;
    bool m_updated;
    bool m_stopped;
};

//------------------------------------------------------------------------------

TestServiceSignals::~TestServiceSignals()
{
    m_worker->stop();
}

//------------------------------------------------------------------------------

void ServiceTest::testCommunication()
{
    const std::string EVENT        = "EVENT";
    const std::string dataKey      = "data1";
    const std::string service1UUID = "service1UUID";
    const std::string service2UUID = "service2UUID";

    data::Composite::sptr obj = data::Composite::New();
    service::ut::TestService::sptr service1;
    service::ut::TestService::sptr service2;

    // Add services
    service::add("sight::service::ut::STest1Input", service1UUID);
    service1 = service::ut::ISTest::dynamicCast(service::get(service1UUID));
    CPPUNIT_ASSERT(service1);
    service1->setInput(obj, dataKey, true);

    service2 = service::add<service::ut::ISTest>("sight::service::ut::STest1Input", service2UUID);
    CPPUNIT_ASSERT(service2);
    service2->setInput(obj, dataKey, true);

    // Object used to check service signals
    TestServiceSignals::sptr receiver1 = std::make_shared<TestServiceSignals>();
    TestServiceSignals::sptr receiver2 = std::make_shared<TestServiceSignals>();

    core::com::helper::SigSlotConnection comHelper;
    comHelper.connect(service1, service::IService::s_STARTED_SIG, receiver1, "start");
    comHelper.connect(service1, service::IService::s_UPDATED_SIG, receiver1, "update");
    comHelper.connect(service1, service::IService::s_STOPPED_SIG, receiver1, "stop");

    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_started);
    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_stopped);

    comHelper.connect(service2, service::IService::s_STARTED_SIG, receiver2, "start");
    comHelper.connect(service2, service::IService::s_UPDATED_SIG, receiver2, "update");
    comHelper.connect(service2, service::IService::s_STOPPED_SIG, receiver2, "stop");

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
    comHelper.connect(
        service1,
        service::ut::ISTest::s_MSG_SENT_SIG,
        service2,
        service::ut::ISTest::s_UPDATE2_SLOT
    );

    CPPUNIT_ASSERT(!service2->getIsUpdated2());

    // Service1 send notification
    {
        auto sig  = service1->signal<service::ut::ISTest::MsgSentSignalType>(service::ut::ISTest::s_MSG_SENT_SIG);
        auto slot = service1->slot(service::IService::s_UPDATE_SLOT);
        core::com::Connection::Blocker block(sig->getConnection(slot));
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

    service::OSR::unregisterService(service1);
    service::OSR::unregisterService(service2);
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
    catch(const core::Exception& e)
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
    catch(core::Exception& e)
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
    catch(core::Exception& e)
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
    service::OSR::unregisterService(_service);
}

//------------------------------------------------------------------------------

void ServiceTest::testWithInAndOut()
{
    std::vector<data::Integer::sptr> obj;
    obj.push_back(data::Integer::New(18));
    obj.push_back(data::Integer::New(23));
    auto service = service::add<service::ut::TestServiceWithData>(
        "sight::service::ut::TestServiceWithData"
    );

    CPPUNIT_ASSERT(service);
    CPPUNIT_ASSERT_EQUAL(false, service->hasAllRequiredObjects());
    service->setInput(obj[0], service::ut::TestServiceWithData::s_INPUT);
    CPPUNIT_ASSERT_EQUAL(false, service->hasAllRequiredObjects());
    service->setInOut(obj[0], service::ut::TestServiceWithData::s_INOUT_GROUP, size_t(0));
    CPPUNIT_ASSERT_EQUAL(false, service->hasAllRequiredObjects());
    service->setInOut(obj[1], service::ut::TestServiceWithData::s_INOUT_GROUP, size_t(1));
    CPPUNIT_ASSERT_EQUAL(true, service->hasAllRequiredObjects());

    service->start().wait();
    CPPUNIT_ASSERT(service->isStarted());
    CPPUNIT_ASSERT(nullptr != service->getObject(service::ut::TestServiceWithData::s_INPUT, data::Access::in));
    service->update().wait();
    CPPUNIT_ASSERT(nullptr != service->getObject(service::ut::TestServiceWithData::s_OUTPUT, data::Access::out));
    data::Object::csptr output = service->getObject(service::ut::TestServiceWithData::s_OUTPUT, data::Access::out);
    CPPUNIT_ASSERT(output);
    data::Integer::csptr outInteger = data::Integer::dynamicCast(output);
    CPPUNIT_ASSERT(outInteger);

    CPPUNIT_ASSERT_EQUAL(obj[0]->value(), outInteger->value());

    auto outInteger2 = service->getOutput<data::Integer>(service::ut::TestServiceWithData::s_OUTPUT).lock();
    CPPUNIT_ASSERT(outInteger2);

    CPPUNIT_ASSERT_EQUAL(obj[0]->value(), outInteger2->value());

    // Test index access
    CPPUNIT_ASSERT_EQUAL(size_t(2), service->m_inoutGroup.size());
    CPPUNIT_ASSERT_EQUAL(obj[0]->value(), service->m_inoutGroup[0].lock()->value());
    CPPUNIT_ASSERT_EQUAL(obj[1]->value(), service->m_inoutGroup[1].lock()->value());

    // Test iterator access
    auto itObj = obj.begin();
    for(const auto& data : service->m_inoutGroup)
    {
        auto i = (*itObj)->value();
        CPPUNIT_ASSERT_EQUAL(i, data.second.lock()->value());
        ++itObj;
    }

    service->stop().wait();

    auto nullInteger = service->getOutput<data::Integer>(service::ut::TestServiceWithData::s_OUTPUT);
    CPPUNIT_ASSERT(nullInteger.expired());

    CPPUNIT_ASSERT(nullptr == service->getObject(service::ut::TestServiceWithData::s_OUTPUT, data::Access::out));
    CPPUNIT_ASSERT(nullptr != service->getObject(service::ut::TestServiceWithData::s_INPUT, data::Access::in));

    service::OSR::unregisterService(service);
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::service
