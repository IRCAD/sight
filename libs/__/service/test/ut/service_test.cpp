/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "service_test.hpp"

#include <core/com/helper/sig_slot_connection.hpp>
#include <core/com/slots.hxx>
#include <core/runtime/helper.hpp>
#include <core/thread/worker.hpp>
#include <core/time_stamp.hpp>

#include <data/composite.hpp>
#include <data/image.hpp>
#include <data/object.hpp>

#include <service/op.hpp>

#include <utest/wait.hpp>

#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::service_test);

//------------------------------------------------------------------------------

namespace sight::service::ut
{

//------------------------------------------------------------------------------

void service_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void service_test::tearDown()
{
    // Clean up after the test run.
    // unregister the services that have not been unregistered because a test failed.

    auto services = sight::service::get_services<service::base>();
    for(const auto& srv : services)
    {
        if(srv->started())
        {
            srv->stop();
        }

        service::unregister_service(srv);
    }
}

//------------------------------------------------------------------------------

void service_test::test_service_configuration()
{
    // Test adding service
    auto srv  = service::add<service::ut::test_srv>("sight::service::ut::test_no_data");
    auto srv2 = service::add<service::ut::test_srv>("sight::service::ut::test_no_data");

    CPPUNIT_ASSERT_EQUAL(service::base::configuration_status::unconfigured, srv->config_status());
    CPPUNIT_ASSERT_EQUAL(test_service::UNCONFIGURED, srv->get_option());
    srv->configure();
    CPPUNIT_ASSERT_EQUAL(service::base::configuration_status::configured, srv->config_status());
    CPPUNIT_ASSERT_EQUAL(test_service::NOT_DEFINED, srv->get_option());

    const std::string optio_n1 = "configuredOption1";
    const std::string optio_n2 = "configuredOption2";

    service::config_t config;
    config.add(test_service::OPTION_KEY, optio_n1);

    srv->set_config(config);
    CPPUNIT_ASSERT_EQUAL(service::base::configuration_status::unconfigured, srv->config_status());

    srv->configure();

    CPPUNIT_ASSERT_EQUAL(service::base::configuration_status::configured, srv->config_status());
    CPPUNIT_ASSERT_EQUAL(optio_n1, srv->get_option());

    service::config_t config2;
    config2.add(test_service::OPTION_KEY, optio_n2);

    CPPUNIT_ASSERT_EQUAL(service::base::configuration_status::unconfigured, srv2->config_status());
    CPPUNIT_ASSERT_EQUAL(test_service::UNCONFIGURED, srv2->get_option());

    srv2->configure(config2);

    CPPUNIT_ASSERT_EQUAL(service::base::configuration_status::configured, srv2->config_status());
    CPPUNIT_ASSERT_EQUAL(optio_n2, srv2->get_option());

    // Test erasing service
    service::unregister_service(srv);
    service::unregister_service(srv2);
}

//------------------------------------------------------------------------------

void service_test::test_service_creation_with_multiple_data()
{
    const std::string data_key1 = "data1";
    const std::string data_key2 = "data2";
    const std::string data_key3 = "data3";
    data::integer::sptr obj1    = std::make_shared<data::integer>();
    data::integer::sptr obj2    = std::make_shared<data::integer>();
    data::integer::sptr obj3    = std::make_shared<data::integer>();

    // Test if the object support the service
    CPPUNIT_ASSERT(
        service::extension::factory::get()->support(
            obj1->get_classname(),
            "sight::service::ut::test_service"
        )
    );

    // Test adding service
    service::base::sptr srv = service::add("sight::service::ut::test2_inouts1_input");
    srv->set_inout(obj1, data_key1);
    CPPUNIT_ASSERT(obj1 == srv->data::has_data::object(data_key1, data::access::inout));
    CPPUNIT_ASSERT(nullptr == srv->data::has_data::object(data_key2, data::access::inout));
    CPPUNIT_ASSERT(nullptr == srv->data::has_data::object(data_key3, data::access::in));

    // Test getting the service object
    CPPUNIT_ASSERT(srv->inout<data::integer>(data_key1).lock() == obj1);

    srv->set_inout(obj2, data_key2, false, false);
    CPPUNIT_ASSERT(obj2 == srv->data::has_data::object(data_key2, data::access::inout));

    // Test getting the service object
    CPPUNIT_ASSERT(srv->inout<data::integer>(data_key2).lock() == obj2);

    srv->set_input(obj3, data_key3);
    CPPUNIT_ASSERT(obj3 == srv->data::has_data::object(data_key3, data::access::in));

    // Test getting the service object
    CPPUNIT_ASSERT(srv->input<data::integer>(data_key3).lock() == obj3);

    // Test unregistering the objects
    srv->set_inout(nullptr, data_key1);
    CPPUNIT_ASSERT(nullptr == srv->data::has_data::object(data_key1, data::access::inout));
    CPPUNIT_ASSERT(nullptr != srv->data::has_data::object(data_key2, data::access::inout));
    CPPUNIT_ASSERT(nullptr != srv->data::has_data::object(data_key3, data::access::in));

    srv->set_inout(nullptr, data_key2);
    CPPUNIT_ASSERT(nullptr == srv->data::has_data::object(data_key1, data::access::inout));
    CPPUNIT_ASSERT(nullptr == srv->data::has_data::object(data_key2, data::access::inout));
    CPPUNIT_ASSERT(nullptr != srv->data::has_data::object(data_key3, data::access::in));

    srv->set_input(nullptr, data_key3);
    CPPUNIT_ASSERT(nullptr == srv->data::has_data::object(data_key1, data::access::inout));
    CPPUNIT_ASSERT(nullptr == srv->data::has_data::object(data_key2, data::access::inout));
    CPPUNIT_ASSERT(nullptr == srv->data::has_data::object(data_key3, data::access::in));

    // Test erasing service
    service::remove(srv);
}

//------------------------------------------------------------------------------

void service_test::test_service_creation_with_template_methods()
{
    const std::string data_key = "data1";
    data::integer::sptr obj    = std::make_shared<data::integer>();

    // Test if the object support the service
    CPPUNIT_ASSERT(
        service::extension::factory::get()->support(
            obj->get_classname(),
            "sight::service::ut::test_service"
        )
    );

    // Test adding service
    auto srv = service::add<service::ut::test_service>("sight::service::ut::test1_inout");
    srv->set_inout(obj, data_key);
    CPPUNIT_ASSERT(srv->data::has_data::object(data_key, data::access::inout));
    CPPUNIT_ASSERT(obj == srv->data::has_data::object(data_key, data::access::inout));

    // Test getting the service its object
    CPPUNIT_ASSERT(srv->inout<data::integer>(data_key).lock() == obj);

    srv->set_inout(nullptr, data_key);
    CPPUNIT_ASSERT(nullptr == srv->data::has_data::object(data_key, data::access::inout));

    // Test erasing service
    service::remove(srv);
}

//------------------------------------------------------------------------------

void service_test::test_service_creation_with_uuid()
{
    const std::string my_uuid   = "myUUID";
    const std::string my_uui_d2 = "myUUID2";
    const std::string my_uui_d3 = "myUUID3";
    const std::string data_key  = "data1";
    std::size_t nb_services     = 0;

    data::integer::sptr obj = std::make_shared<data::integer>();
    service::base::sptr service;
    service::base::sptr service2;

    // Test if the object support the service
    CPPUNIT_ASSERT(
        service::extension::factory::get()->support(
            obj->get_classname(),
            "sight::service::ut::test_service"
        )
    );

    // Test adding service
    service = service::add("sight::service::ut::test1_inout", my_uuid);
    CPPUNIT_ASSERT(service);
    service->set_inout(obj, data_key);

    service2 = service::add("sight::service::ut::test1_inout", my_uui_d2);
    CPPUNIT_ASSERT(service2);
    service2->set_inout(obj, data_key);

    nb_services = 2;
    CPPUNIT_ASSERT(core::id::exist(my_uuid));
    CPPUNIT_ASSERT(core::id::exist(my_uui_d2));

    // Test getting the service its object
    service::base::sptr service2bis = service::get(my_uui_d2);
    CPPUNIT_ASSERT(service2bis);
    CPPUNIT_ASSERT(service2bis->inout<data::integer>(data_key).lock() == obj);
    CPPUNIT_ASSERT_EQUAL(my_uui_d2, service2bis->get_id());
    CPPUNIT_ASSERT(!core::id::exist(my_uui_d3));
    CPPUNIT_ASSERT_EQUAL(nb_services, sight::service::get_services("sight::service::ut::test_service").size());

    // Test erasing service
    service::remove(service);
    nb_services--;
    CPPUNIT_ASSERT_EQUAL(nb_services, sight::service::get_services("sight::service::ut::test_service").size());
    service::remove(service2);
}

//------------------------------------------------------------------------------

void service_test::test_start_stop_update()
{
    const std::string my_uuid = "myUUID";

    data::integer::sptr obj = std::make_shared<data::integer>();
    service::ut::test_service::sptr service;

    // Add service
    CPPUNIT_ASSERT(
        service::extension::factory::get()->support(
            obj->get_classname(),
            "sight::service::ut::test_service"
        )
    );
    service = service::add<service::ut::test_service>("sight::service::ut::test_no_data", my_uuid);
    CPPUNIT_ASSERT(service);

    // Service must be stop when it is created
    CPPUNIT_ASSERT(service->stopped());
    CPPUNIT_ASSERT(!service->started());

    // Start service
    service->start().wait();
    CPPUNIT_ASSERT(service->started());
    CPPUNIT_ASSERT(!service->stopped());

    // Update service
    service->update().wait();
    CPPUNIT_ASSERT(service->get_is_updated());

    // Stop service
    service->stop().wait();
    CPPUNIT_ASSERT(service->stopped());
    CPPUNIT_ASSERT(!service->started());

    // Erase Service
    service::unregister_service(service);
}

//------------------------------------------------------------------------------

void service_test::test_start_stop_update_exceptions()
{
    // Test on the same worker
    {
        auto service = service::add<service::ut::test_service>("sight::service::ut::test_no_data");
        service_test::start_stop_update_exceptions(service);
    }
    // Test on a different worker
    {
        auto service = service::add<service::ut::test_service>("sight::service::ut::test_no_data");
        auto worker  = core::thread::worker::make();
        service->set_worker(worker);
        service_test::start_stop_update_exceptions(service);
        worker->stop();
    }
}

//------------------------------------------------------------------------------

struct test_service_signals : public core::com::has_slots
{
    using sptr = std::shared_ptr<test_service_signals>;

    test_service_signals()
    {
        new_slot("start", &test_service_signals::start, this);
        new_slot("update", &test_service_signals::update, this);
        new_slot("stop", &test_service_signals::stop, this);

        m_worker = core::thread::worker::make();
        m_slots.set_worker(m_worker);
    }

    ~test_service_signals() override;

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

test_service_signals::~test_service_signals()
{
    m_worker->stop();
}

//------------------------------------------------------------------------------

void service_test::test_communication()
{
    const std::string event         = "EVENT";
    const std::string data_key      = "data1";
    const std::string service1_uuid = "service1UUID";
    const std::string service2_uuid = "service2UUID";

    data::composite::sptr obj = std::make_shared<data::composite>();
    service::ut::test_service::sptr service1;
    service::ut::test_service::sptr service2;

    // Add services
    service::add("sight::service::ut::test1_input", service1_uuid);
    service1 = std::dynamic_pointer_cast<service::ut::test_srv>(service::get(service1_uuid));
    CPPUNIT_ASSERT(service1);
    service1->set_input(obj, data_key, true);

    service2 = service::add<service::ut::test_srv>("sight::service::ut::test1_input", service2_uuid);
    CPPUNIT_ASSERT(service2);
    service2->set_input(obj, data_key, true);

    // Object used to check service signals
    test_service_signals::sptr receiver1 = std::make_shared<test_service_signals>();
    test_service_signals::sptr receiver2 = std::make_shared<test_service_signals>();

    core::com::helper::sig_slot_connection com_helper;
    com_helper.connect(service1, service::signals::STARTED, receiver1, "start");
    com_helper.connect(service1, service::signals::UPDATED, receiver1, "update");
    com_helper.connect(service1, service::signals::STOPPED, receiver1, "stop");

    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_started);
    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_stopped);

    com_helper.connect(service2, service::signals::STARTED, receiver2, "start");
    com_helper.connect(service2, service::signals::UPDATED, receiver2, "update");
    com_helper.connect(service2, service::signals::STOPPED, receiver2, "stop");

    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_started);
    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_stopped);

    // Start services
    service1->start().wait();
    service2->start().wait();
    CPPUNIT_ASSERT(service1->started());
    CPPUNIT_ASSERT(service2->started());

    SIGHT_TEST_WAIT(receiver1->m_started && receiver2->m_started)
    CPPUNIT_ASSERT_EQUAL(true, receiver1->m_started);
    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver1->m_stopped);
    CPPUNIT_ASSERT_EQUAL(true, receiver2->m_started);
    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_updated);
    CPPUNIT_ASSERT_EQUAL(false, receiver2->m_stopped);

    // Register communication channel
    com_helper.connect(
        service1,
        service::ut::test_srv::signals::MSG_SENT,
        service2,
        service::ut::test_srv::slots::UPDATE2
    );

    CPPUNIT_ASSERT(!service2->get_is_updated2());

    // Service1 send notification
    {
        auto sig =
            service1->signal<service::ut::test_srv::signals::msg_sent_t>(service::ut::test_srv::signals::MSG_SENT);
        auto slot = service1->slot(service::slots::UPDATE);
        core::com::connection::blocker block(sig->get_connection(slot));
        sig->async_emit(event);
    }

    service1->update().wait();
    service2->update().wait();
    CPPUNIT_ASSERT(service2->get_is_updated2());

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

    com_helper.disconnect();

    service::unregister_service(service1);
    service::unregister_service(service2);
}

//------------------------------------------------------------------------------

void service_test::start_stop_update_exceptions(test_service::sptr _service)
{
    // Service must be stop when it is created
    CPPUNIT_ASSERT(_service->stopped());

    // Start service
    _service->start().wait();
    CPPUNIT_ASSERT(_service->started());

    // Stop service
    _service->stop().wait();
    CPPUNIT_ASSERT(_service->stopped());

    // Start service with exceptions
    _service->set_raise_exception(true);
    _service->start().wait();
    CPPUNIT_ASSERT(_service->stopped());

    // Check we can catch the exception
    bool exception_caught = false;
    try
    {
        _service->start().get();
    }
    catch(const core::exception& e)
    {
        exception_caught = true;
        CPPUNIT_ASSERT_EQUAL(std::string("start error"), std::string(e.what()));
    }
    CPPUNIT_ASSERT(exception_caught);
    CPPUNIT_ASSERT(_service->stopped());

    // Start service again
    _service->set_raise_exception(false);
    _service->start().wait();
    CPPUNIT_ASSERT(_service->started());

    // Update service
    _service->update().wait();
    CPPUNIT_ASSERT(_service->get_is_updated());
    _service->reset_is_updated();
    CPPUNIT_ASSERT(!_service->get_is_updated());

    // Update service with exception caught
    _service->set_raise_exception(true);
    exception_caught = false;
    try
    {
        _service->update().get();
    }
    catch(core::exception& e)
    {
        exception_caught = true;
        CPPUNIT_ASSERT_EQUAL(std::string("update error"), std::string(e.what()));
    }
    CPPUNIT_ASSERT(exception_caught);
    CPPUNIT_ASSERT(!_service->get_is_updated());

    // Update service without exception caught
    _service->update().wait();
    CPPUNIT_ASSERT(!_service->get_is_updated());

    // Update service
    _service->set_raise_exception(false);
    _service->update().wait();
    CPPUNIT_ASSERT(_service->get_is_updated());

    // Stop service with exception caught
    _service->set_raise_exception(true);
    exception_caught = false;
    try
    {
        _service->stop().get();
    }
    catch(core::exception& e)
    {
        exception_caught = true;
        CPPUNIT_ASSERT_EQUAL(std::string("stop error"), std::string(e.what()));
    }
    CPPUNIT_ASSERT(exception_caught);
    CPPUNIT_ASSERT(_service->started());

    // Update service without exception caught
    _service->stop().wait();
    CPPUNIT_ASSERT(_service->started());

    _service->set_raise_exception(false);
    _service->stop().wait();
    CPPUNIT_ASSERT(_service->stopped());

    // Erase Service
    service::unregister_service(_service);
}

//------------------------------------------------------------------------------

void service_test::test_with_in_and_out()
{
    std::vector<data::integer::sptr> obj;
    obj.push_back(std::make_shared<data::integer>(18));
    obj.push_back(std::make_shared<data::integer>(23));
    auto service = service::add<service::ut::test_service_with_data>(
        "sight::service::ut::test_service_with_data"
    );

    CPPUNIT_ASSERT(service);
    service->set_input(obj[0], service::ut::test_service_with_data::INPUT);
    service->set_inout(obj[0], service::ut::test_service_with_data::INOUT_GROUP, true, false, 0);
    service->set_inout(obj[1], service::ut::test_service_with_data::INOUT_GROUP, true, false, 1);

    service->start().wait();
    CPPUNIT_ASSERT(service->started());
    CPPUNIT_ASSERT(
        nullptr
        != service->data::has_data::object(service::ut::test_service_with_data::INPUT, data::access::in)
    );
    service->update().wait();
    CPPUNIT_ASSERT(
        nullptr
        != service->data::has_data::object(service::ut::test_service_with_data::OUTPUT, data::access::out)
    );
    data::object::csptr output = service->data::has_data::object(
        service::ut::test_service_with_data::OUTPUT,
        data::access::out
    );
    CPPUNIT_ASSERT(output);
    data::integer::csptr out_integer = std::dynamic_pointer_cast<const data::integer>(output);
    CPPUNIT_ASSERT(out_integer);

    CPPUNIT_ASSERT_EQUAL(obj[0]->value(), out_integer->value());

    {
        auto out_integer2 = service->output<data::integer>(service::ut::test_service_with_data::OUTPUT).lock();
        CPPUNIT_ASSERT(out_integer2);

        CPPUNIT_ASSERT_EQUAL(obj[0]->value(), out_integer2->value());
    }

    // Test index access
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), service->m_inout_group.size());
    CPPUNIT_ASSERT_EQUAL(obj[0]->value(), service->m_inout_group[0].lock()->value());
    CPPUNIT_ASSERT_EQUAL(obj[1]->value(), service->m_inout_group[1].lock()->value());

    // Test iterator access
    auto it_obj = obj.begin();
    for(const auto& data : service->m_inout_group)
    {
        auto i = (*it_obj)->value();
        CPPUNIT_ASSERT_EQUAL(i, data.second->lock()->value());
        ++it_obj;
    }

    service->stop().wait();

    auto null_integer = service->output<data::integer>(service::ut::test_service_with_data::OUTPUT);
    CPPUNIT_ASSERT(null_integer.expired());

    CPPUNIT_ASSERT(
        nullptr
        == service->data::has_data::object(service::ut::test_service_with_data::OUTPUT, data::access::out)
    );
    CPPUNIT_ASSERT(
        nullptr
        != service->data::has_data::object(service::ut::test_service_with_data::INPUT, data::access::in)
    );

    service::unregister_service(service);
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
