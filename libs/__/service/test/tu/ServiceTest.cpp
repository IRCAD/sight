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

#include "ServiceTest.hpp"

#include <core/com/helper/sig_slot_connection.hpp>
#include <core/com/slots.hxx>
#include <core/runtime/helper.hpp>
#include <core/thread/worker.hpp>
#include <core/time_stamp.hpp>

#include <data/Composite.hpp>
#include <data/Image.hpp>
#include <data/Object.hpp>

#include <service/op.hpp>
#include <service/op/Get.hpp>

#include <utest/wait.hpp>

#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::ServiceTest);

//------------------------------------------------------------------------------

namespace sight::service::ut
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

    auto services = sight::service::getServices<service::base>();
    for(const auto& srv : services)
    {
        if(srv->isStarted())
        {
            srv->stop();
        }

        service::unregisterService(srv);
    }
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceConfiguration()
{
    // Test adding service
    auto srv  = service::add<service::ut::test_srv>("sight::service::ut::STestNoData");
    auto srv2 = service::add<service::ut::test_srv>("sight::service::ut::STestNoData");

    CPPUNIT_ASSERT_EQUAL(service::base::ConfigurationStatus::UNCONFIGURED, srv->getConfigurationStatus());
    CPPUNIT_ASSERT_EQUAL(TestService::s_UNCONFIGURED, srv->getOption());
    srv->configure();
    CPPUNIT_ASSERT_EQUAL(service::base::ConfigurationStatus::CONFIGURED, srv->getConfigurationStatus());
    CPPUNIT_ASSERT_EQUAL(TestService::s_NOT_DEFINED, srv->getOption());

    const std::string OPTION1 = "configuredOption1";
    const std::string OPTION2 = "configuredOption2";

    service::config_t config;
    config.add(TestService::s_OPTION_KEY, OPTION1);

    srv->setConfiguration(config);
    CPPUNIT_ASSERT_EQUAL(service::base::ConfigurationStatus::UNCONFIGURED, srv->getConfigurationStatus());

    srv->configure();

    CPPUNIT_ASSERT_EQUAL(service::base::ConfigurationStatus::CONFIGURED, srv->getConfigurationStatus());
    CPPUNIT_ASSERT_EQUAL(OPTION1, srv->getOption());

    service::config_t config2;
    config2.add(TestService::s_OPTION_KEY, OPTION2);

    CPPUNIT_ASSERT_EQUAL(service::base::ConfigurationStatus::UNCONFIGURED, srv2->getConfigurationStatus());
    CPPUNIT_ASSERT_EQUAL(TestService::s_UNCONFIGURED, srv2->getOption());

    srv2->configure(config2);

    CPPUNIT_ASSERT_EQUAL(service::base::ConfigurationStatus::CONFIGURED, srv2->getConfigurationStatus());
    CPPUNIT_ASSERT_EQUAL(OPTION2, srv2->getOption());

    // Test erasing service
    service::unregisterService(srv);
    service::unregisterService(srv2);
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceCreationWithMultipleData()
{
    const std::string dataKey1 = "data1";
    const std::string dataKey2 = "data2";
    const std::string dataKey3 = "data3";
    data::Integer::sptr obj1   = std::make_shared<data::Integer>();
    data::Integer::sptr obj2   = std::make_shared<data::Integer>();
    data::Integer::sptr obj3   = std::make_shared<data::Integer>();

    // Test if the object support the service
    CPPUNIT_ASSERT(
        service::extension::Factory::getDefault()->support(
            obj1->get_classname(),
            "sight::service::ut::TestService"
        )
    );

    // Test adding service
    service::base::sptr srv = service::add("sight::service::ut::STest2Inouts1Input");
    srv->setInOut(obj1, dataKey1);
    CPPUNIT_ASSERT(obj1 == srv->data::has_data::object(dataKey1, data::Access::inout));
    CPPUNIT_ASSERT(nullptr == srv->data::has_data::object(dataKey2, data::Access::inout));
    CPPUNIT_ASSERT(nullptr == srv->data::has_data::object(dataKey3, data::Access::in));

    // Test getting the service object
    CPPUNIT_ASSERT(srv->inout<data::Integer>(dataKey1).lock() == obj1);

    srv->setInOut(obj2, dataKey2, false, false);
    CPPUNIT_ASSERT(obj2 == srv->data::has_data::object(dataKey2, data::Access::inout));

    // Test getting the service object
    CPPUNIT_ASSERT(srv->inout<data::Integer>(dataKey2).lock() == obj2);

    srv->setInput(obj3, dataKey3);
    CPPUNIT_ASSERT(obj3 == srv->data::has_data::object(dataKey3, data::Access::in));

    // Test getting the service object
    CPPUNIT_ASSERT(srv->input<data::Integer>(dataKey3).lock() == obj3);

    // Test unregistering the objects
    srv->setInOut(nullptr, dataKey1);
    CPPUNIT_ASSERT(nullptr == srv->data::has_data::object(dataKey1, data::Access::inout));
    CPPUNIT_ASSERT(nullptr != srv->data::has_data::object(dataKey2, data::Access::inout));
    CPPUNIT_ASSERT(nullptr != srv->data::has_data::object(dataKey3, data::Access::in));

    srv->setInOut(nullptr, dataKey2);
    CPPUNIT_ASSERT(nullptr == srv->data::has_data::object(dataKey1, data::Access::inout));
    CPPUNIT_ASSERT(nullptr == srv->data::has_data::object(dataKey2, data::Access::inout));
    CPPUNIT_ASSERT(nullptr != srv->data::has_data::object(dataKey3, data::Access::in));

    srv->setInput(nullptr, dataKey3);
    CPPUNIT_ASSERT(nullptr == srv->data::has_data::object(dataKey1, data::Access::inout));
    CPPUNIT_ASSERT(nullptr == srv->data::has_data::object(dataKey2, data::Access::inout));
    CPPUNIT_ASSERT(nullptr == srv->data::has_data::object(dataKey3, data::Access::in));

    // Test erasing service
    service::remove(srv);
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceCreationWithTemplateMethods()
{
    const std::string dataKey = "data1";
    data::Integer::sptr obj   = std::make_shared<data::Integer>();

    // Test if the object support the service
    CPPUNIT_ASSERT(
        service::extension::Factory::getDefault()->support(
            obj->get_classname(),
            "sight::service::ut::TestService"
        )
    );

    // Test adding service
    auto srv = service::add<service::ut::TestService>("sight::service::ut::STest1Inout");
    srv->setInOut(obj, dataKey);
    CPPUNIT_ASSERT(srv->data::has_data::object(dataKey, data::Access::inout));
    CPPUNIT_ASSERT(obj == srv->data::has_data::object(dataKey, data::Access::inout));

    // Test getting the service its object
    CPPUNIT_ASSERT(srv->inout<data::Integer>(dataKey).lock() == obj);

    srv->setInOut(nullptr, dataKey);
    CPPUNIT_ASSERT(nullptr == srv->data::has_data::object(dataKey, data::Access::inout));

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
    std::size_t nbServices    = 0;

    data::Integer::sptr obj = std::make_shared<data::Integer>();
    service::base::sptr service;
    service::base::sptr service2;

    // Test if the object support the service
    CPPUNIT_ASSERT(
        service::extension::Factory::getDefault()->support(
            obj->get_classname(),
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
    CPPUNIT_ASSERT(core::tools::id::exist(myUUID));
    CPPUNIT_ASSERT(core::tools::id::exist(myUUID2));

    // Test getting the service its object
    service::base::sptr service2bis = service::get(myUUID2);
    CPPUNIT_ASSERT(service2bis);
    CPPUNIT_ASSERT(service2bis->inout<data::Integer>(dataKey).lock() == obj);
    CPPUNIT_ASSERT_EQUAL(myUUID2, service2bis->get_id());
    CPPUNIT_ASSERT(!core::tools::id::exist(myUUID3));
    CPPUNIT_ASSERT_EQUAL(nbServices, sight::service::getServices("sight::service::ut::TestService").size());

    // Test erasing service
    service::remove(service);
    nbServices--;
    CPPUNIT_ASSERT_EQUAL(nbServices, sight::service::getServices("sight::service::ut::TestService").size());
    service::remove(service2);
}

//------------------------------------------------------------------------------

void ServiceTest::testStartStopUpdate()
{
    const std::string myUUID = "myUUID";

    data::Integer::sptr obj = std::make_shared<data::Integer>();
    service::ut::TestService::sptr service;

    // Add service
    CPPUNIT_ASSERT(
        service::extension::Factory::getDefault()->support(
            obj->get_classname(),
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
    service::unregisterService(service);
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
        auto worker  = core::thread::worker::make();
        service->set_worker(worker);
        ServiceTest::startStopUpdateExceptions(service);
        worker->stop();
    }
}

//------------------------------------------------------------------------------

struct TestServiceSignals : public core::com::has_slots
{
    using sptr = std::shared_ptr<TestServiceSignals>;

    TestServiceSignals()
    {
        new_slot("start", &TestServiceSignals::start, this);
        new_slot("update", &TestServiceSignals::update, this);
        new_slot("stop", &TestServiceSignals::stop, this);

        m_worker = core::thread::worker::make();
        m_slots.set_worker(m_worker);
    }

    ~TestServiceSignals() override;

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

    core::thread::worker::sptr m_worker;
    bool m_started {false};
    bool m_updated {false};
    bool m_stopped {false};
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

    data::Composite::sptr obj = std::make_shared<data::Composite>();
    service::ut::TestService::sptr service1;
    service::ut::TestService::sptr service2;

    // Add services
    service::add("sight::service::ut::STest1Input", service1UUID);
    service1 = std::dynamic_pointer_cast<service::ut::test_srv>(service::get(service1UUID));
    CPPUNIT_ASSERT(service1);
    service1->setInput(obj, dataKey, true);

    service2 = service::add<service::ut::test_srv>("sight::service::ut::STest1Input", service2UUID);
    CPPUNIT_ASSERT(service2);
    service2->setInput(obj, dataKey, true);

    // Object used to check service signals
    TestServiceSignals::sptr receiver1 = std::make_shared<TestServiceSignals>();
    TestServiceSignals::sptr receiver2 = std::make_shared<TestServiceSignals>();

    core::com::helper::sig_slot_connection comHelper;
    comHelper.connect(service1, service::signals::STARTED, receiver1, "start");
    comHelper.connect(service1, service::signals::UPDATED, receiver1, "update");
    comHelper.connect(service1, service::signals::STOPPED, receiver1, "stop");

    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_started);
    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_stopped);

    comHelper.connect(service2, service::signals::STARTED, receiver2, "start");
    comHelper.connect(service2, service::signals::UPDATED, receiver2, "update");
    comHelper.connect(service2, service::signals::STOPPED, receiver2, "stop");

    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_started);
    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_stopped);

    // Start services
    service1->start().wait();
    service2->start().wait();
    CPPUNIT_ASSERT(service1->isStarted());
    CPPUNIT_ASSERT(service2->isStarted());

    SIGHT_TEST_WAIT(receiver1->m_started && receiver2->m_started)
    CPPUNIT_ASSERT_EQUAL(true, receiver1->m_started);
    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_stopped);
    CPPUNIT_ASSERT_EQUAL(true, receiver2->m_started);
    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_stopped);

    // Register communication channel
    comHelper.connect(
        service1,
        service::ut::test_srv::signals::MSG_SENT,
        service2,
        service::ut::test_srv::slots::UPDATE2
    );

    CPPUNIT_ASSERT(!service2->getIsUpdated2());

    // Service1 send notification
    {
        auto sig =
            service1->signal<service::ut::test_srv::signals::msg_sent_t>(service::ut::test_srv::signals::MSG_SENT);
        auto slot = service1->slot(service::slots::UPDATE);
        core::com::connection::blocker block(sig->get_connection(slot));
        sig->async_emit(EVENT);
    }

    service1->update().wait();
    service2->update().wait();
    CPPUNIT_ASSERT(service2->getIsUpdated2());

    SIGHT_TEST_WAIT(receiver1->m_updated && receiver2->m_updated)
    CPPUNIT_ASSERT_EQUAL(true, receiver1->m_started);
    CPPUNIT_ASSERT_EQUAL(true, receiver1->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_stopped);
    CPPUNIT_ASSERT_EQUAL(true, receiver2->m_started);
    CPPUNIT_ASSERT_EQUAL(true, receiver2->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_stopped);

    // Test if service2 has received the message
    service1->stop().wait();
    service2->stop().wait();

    SIGHT_TEST_WAIT(receiver1->m_stopped && receiver2->m_stopped)
    CPPUNIT_ASSERT_EQUAL(true, receiver1->m_started);
    CPPUNIT_ASSERT_EQUAL(true, receiver1->m_updated);
    CPPUNIT_ASSERT_EQUAL(true, receiver1->m_stopped);
    CPPUNIT_ASSERT_EQUAL(true, receiver2->m_started);
    CPPUNIT_ASSERT_EQUAL(true, receiver2->m_updated);
    CPPUNIT_ASSERT_EQUAL(true, receiver2->m_stopped);

    comHelper.disconnect();

    service::unregisterService(service1);
    service::unregisterService(service2);
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
    catch(const core::exception& e)
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
    catch(core::exception& e)
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
    catch(core::exception& e)
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
    service::unregisterService(_service);
}

//------------------------------------------------------------------------------

void ServiceTest::testWithInAndOut()
{
    std::vector<data::Integer::sptr> obj;
    obj.push_back(std::make_shared<data::Integer>(18));
    obj.push_back(std::make_shared<data::Integer>(23));
    auto service = service::add<service::ut::TestServiceWithData>(
        "sight::service::ut::TestServiceWithData"
    );

    CPPUNIT_ASSERT(service);
    service->setInput(obj[0], service::ut::TestServiceWithData::s_INPUT);
    service->setInOut(obj[0], service::ut::TestServiceWithData::s_INOUT_GROUP, true, false, 0);
    service->setInOut(obj[1], service::ut::TestServiceWithData::s_INOUT_GROUP, true, false, 1);

    service->start().wait();
    CPPUNIT_ASSERT(service->isStarted());
    CPPUNIT_ASSERT(
        nullptr
        != service->data::has_data::object(service::ut::TestServiceWithData::s_INPUT, data::Access::in)
    );
    service->update().wait();
    CPPUNIT_ASSERT(
        nullptr
        != service->data::has_data::object(service::ut::TestServiceWithData::s_OUTPUT, data::Access::out)
    );
    data::Object::csptr output = service->data::has_data::object(
        service::ut::TestServiceWithData::s_OUTPUT,
        data::Access::out
    );
    CPPUNIT_ASSERT(output);
    data::Integer::csptr outInteger = std::dynamic_pointer_cast<const data::Integer>(output);
    CPPUNIT_ASSERT(outInteger);

    CPPUNIT_ASSERT_EQUAL(obj[0]->value(), outInteger->value());

    {
        auto outInteger2 = service->output<data::Integer>(service::ut::TestServiceWithData::s_OUTPUT).lock();
        CPPUNIT_ASSERT(outInteger2);

        CPPUNIT_ASSERT_EQUAL(obj[0]->value(), outInteger2->value());
    }

    // Test index access
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), service->m_inoutGroup.size());
    CPPUNIT_ASSERT_EQUAL(obj[0]->value(), service->m_inoutGroup[0].lock()->value());
    CPPUNIT_ASSERT_EQUAL(obj[1]->value(), service->m_inoutGroup[1].lock()->value());

    // Test iterator access
    auto itObj = obj.begin();
    for(const auto& data : service->m_inoutGroup)
    {
        auto i = (*itObj)->value();
        CPPUNIT_ASSERT_EQUAL(i, data.second->lock()->value());
        ++itObj;
    }

    service->stop().wait();

    auto nullInteger = service->output<data::Integer>(service::ut::TestServiceWithData::s_OUTPUT);
    CPPUNIT_ASSERT(nullInteger.expired());

    CPPUNIT_ASSERT(
        nullptr
        == service->data::has_data::object(service::ut::TestServiceWithData::s_OUTPUT, data::Access::out)
    );
    CPPUNIT_ASSERT(
        nullptr
        != service->data::has_data::object(service::ut::TestServiceWithData::s_INPUT, data::Access::in)
    );

    service::unregisterService(service);
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
