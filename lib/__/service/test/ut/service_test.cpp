/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "test_service.hpp"

#include <core/com/helper/sig_slot_connection.hpp>
#include <core/com/slots.hxx>
#include <core/runtime/helper.hpp>
#include <core/thread/worker.hpp>
#include <core/time_stamp.hpp>

#include <data/image.hpp>
#include <data/map.hpp>
#include <data/object.hpp>

#include <service/op.hpp>

#include <utest/wait.hpp>

#include <doctest/doctest.h>

namespace
{

struct fixture
{
    fixture()
    {
        m_worker = sight::core::thread::worker::make();
    }

    ~fixture()
    {
        // Clean up after the test run.
        // unregister the services that have not been unregistered because a test failed.

        auto services = sight::service::get_services<sight::service::base>();
        for(const auto& srv : services)
        {
            if(srv->started())
            {
                srv->stop().wait();
            }

            sight::service::unregister_service(srv);
        }

        m_worker->stop();
        m_worker.reset();
    }

    sight::core::thread::worker::sptr m_worker;
};

} // namespace

//------------------------------------------------------------------------------

namespace sight::service::ut
{

//------------------------------------------------------------------------------

struct test_service_signals : public sight::core::com::has_slots
{
    using sptr = std::shared_ptr<test_service_signals>;

    test_service_signals()
    {
        new_slot("start", &test_service_signals::start, this);
        new_slot("update", &test_service_signals::update, this);
        new_slot("stop", &test_service_signals::stop, this);

        m_worker = sight::core::thread::worker::make();
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

    sight::core::thread::worker::sptr m_worker;
    bool m_started {false};
    bool m_updated {false};
    bool m_stopped {false};
};

//------------------------------------------------------------------------------

test_service_signals::~test_service_signals()
{
    m_worker->stop();
}

} // namespace sight::service::ut

//------------------------------------------------------------------------------

TEST_SUITE("sight::service::service")
{
//------------------------------------------------------------------------------

    TEST_CASE("service_configuration")
    {
        // Test adding service
        auto srv  = sight::service::add<sight::service::ut::test_srv>("sight::service::ut::test_no_data");
        auto srv2 = sight::service::add<sight::service::ut::test_srv>("sight::service::ut::test_no_data");

        CHECK_EQ(sight::service::base::configuration_status::unconfigured, srv->config_status());
        CHECK_EQ(sight::service::ut::test_service::UNCONFIGURED, srv->get_option());
        srv->configure();
        CHECK_EQ(sight::service::base::configuration_status::configured, srv->config_status());
        CHECK_EQ(sight::service::ut::test_service::NOT_DEFINED, srv->get_option());

        const std::string option_1 = "configuredOption1";
        const std::string option_2 = "configuredOption2";

        sight::service::config_t config;
        config.add(sight::service::ut::test_service::OPTION_KEY, option_1);

        srv->set_config(config);
        CHECK_EQ(sight::service::base::configuration_status::unconfigured, srv->config_status());

        srv->configure();

        CHECK_EQ(sight::service::base::configuration_status::configured, srv->config_status());
        CHECK_EQ(option_1, srv->get_option());

        sight::service::config_t config2;
        config2.add(sight::service::ut::test_service::OPTION_KEY, option_2);

        CHECK_EQ(sight::service::base::configuration_status::unconfigured, srv2->config_status());
        CHECK_EQ(sight::service::ut::test_service::UNCONFIGURED, srv2->get_option());

        srv2->configure(config2);

        CHECK_EQ(sight::service::base::configuration_status::configured, srv2->config_status());
        CHECK_EQ(option_2, srv2->get_option());

        // Test erasing service
        sight::service::unregister_service(srv);
        sight::service::unregister_service(srv2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("service_creation_with_multiple_data")
    {
        const std::string data_key1     = "data1";
        const std::string data_key2     = "data2";
        const std::string data_key3     = "data3";
        sight::data::integer::sptr obj1 = std::make_shared<sight::data::integer>();
        sight::data::integer::sptr obj2 = std::make_shared<sight::data::integer>();
        sight::data::integer::sptr obj3 = std::make_shared<sight::data::integer>();

        // Test if the object support the service
        CHECK(
            sight::service::extension::factory::get()->support(
                obj1->get_classname(),
                "sight::service::ut::test_service"
            )
        );

        // Test adding service
        sight::service::base::sptr srv = sight::service::add("sight::service::ut::test2_inouts1_input");
        srv->set_inout(obj1, data_key1);
        CHECK(obj1 == srv->sight::data::has_data::object(data_key1, sight::data::access::inout));
        CHECK(nullptr == srv->sight::data::has_data::object(data_key2, sight::data::access::inout));
        CHECK(nullptr == srv->sight::data::has_data::object(data_key3, sight::data::access::in));

        // Test getting the service object
        CHECK(srv->inout<sight::data::integer>(data_key1).lock() == obj1);

        srv->set_inout(obj2, data_key2, false, false);
        CHECK(obj2 == srv->sight::data::has_data::object(data_key2, sight::data::access::inout));

        // Test getting the service object
        CHECK(srv->inout<sight::data::integer>(data_key2).lock() == obj2);

        srv->set_input(obj3, data_key3);
        CHECK(obj3 == srv->sight::data::has_data::object(data_key3, sight::data::access::in));

        // Test getting the service object
        CHECK(srv->input<sight::data::integer>(data_key3).lock() == obj3);

        // Test unregistering the objects
        srv->set_inout(nullptr, data_key1);
        CHECK(nullptr == srv->sight::data::has_data::object(data_key1, sight::data::access::inout));
        CHECK(nullptr != srv->sight::data::has_data::object(data_key2, sight::data::access::inout));
        CHECK(nullptr != srv->sight::data::has_data::object(data_key3, sight::data::access::in));

        srv->set_inout(nullptr, data_key2);
        CHECK(nullptr == srv->sight::data::has_data::object(data_key1, sight::data::access::inout));
        CHECK(nullptr == srv->sight::data::has_data::object(data_key2, sight::data::access::inout));
        CHECK(nullptr != srv->sight::data::has_data::object(data_key3, sight::data::access::in));

        srv->set_input(nullptr, data_key3);
        CHECK(nullptr == srv->sight::data::has_data::object(data_key1, sight::data::access::inout));
        CHECK(nullptr == srv->sight::data::has_data::object(data_key2, sight::data::access::inout));
        CHECK(nullptr == srv->sight::data::has_data::object(data_key3, sight::data::access::in));

        // Test erasing service
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE("service_creation_with_template_methods")
    {
        const std::string data_key     = "data1";
        sight::data::integer::sptr obj = std::make_shared<sight::data::integer>();

        // Test if the object support the service
        CHECK(
            sight::service::extension::factory::get()->support(
                obj->get_classname(),
                "sight::service::ut::test_service"
            )
        );

        // Test adding service
        auto srv = sight::service::add<sight::service::ut::test_service>("sight::service::ut::test1_inout");
        srv->set_inout(obj, data_key);
        CHECK(srv->sight::data::has_data::object(data_key, sight::data::access::inout));
        CHECK(obj == srv->sight::data::has_data::object(data_key, sight::data::access::inout));

        // Test getting the service its object
        CHECK(srv->inout<sight::data::integer>(data_key).lock() == obj);

        srv->set_inout(nullptr, data_key);
        CHECK(nullptr == srv->sight::data::has_data::object(data_key, sight::data::access::inout));

        // Test erasing service
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE("service_creation_with_uuid")
    {
        const std::string my_uuid   = "myUUID";
        const std::string my_uui_d2 = "myUUID2";
        const std::string my_uui_d3 = "myUUID3";
        const std::string data_key  = "data1";
        std::size_t nb_services     = 0;

        sight::data::integer::sptr obj = std::make_shared<sight::data::integer>();
        sight::service::base::sptr service;
        sight::service::base::sptr service2;

        // Test if the object support the service
        CHECK(
            sight::service::extension::factory::get()->support(
                obj->get_classname(),
                "sight::service::ut::test_service"
            )
        );

        // Test adding service
        service = sight::service::add("sight::service::ut::test1_inout", my_uuid);
        CHECK(service);
        service->set_inout(obj, data_key);

        service2 = sight::service::add("sight::service::ut::test1_inout", my_uui_d2);
        CHECK(service2);
        service2->set_inout(obj, data_key);

        nb_services = 2;
        CHECK(sight::core::id::exist(my_uuid));
        CHECK(sight::core::id::exist(my_uui_d2));

        // Test getting the service its object
        sight::service::base::sptr service2bis = sight::service::get(my_uui_d2);
        CHECK(service2bis);
        CHECK(service2bis->inout<sight::data::integer>(data_key).lock() == obj);
        CHECK_EQ(my_uui_d2, service2bis->get_id());
        CHECK(!sight::core::id::exist(my_uui_d3));
        CHECK_EQ(nb_services, sight::service::get_services("sight::service::ut::test_service").size());

        // Test erasing service
        sight::service::remove(service);
        nb_services--;
        CHECK_EQ(nb_services, sight::service::get_services("sight::service::ut::test_service").size());
        sight::service::remove(service2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("start_stop_update")
    {
        const std::string my_uuid = "myUUID";

        sight::data::integer::sptr obj = std::make_shared<sight::data::integer>();
        sight::service::ut::test_service::sptr service;

        // Add service
        CHECK(
            sight::service::extension::factory::get()->support(
                obj->get_classname(),
                "sight::service::ut::test_service"
            )
        );
        service = sight::service::add<sight::service::ut::test_service>("sight::service::ut::test_no_data", my_uuid);
        CHECK(service);

        // Service must be stop when it is created
        CHECK(service->stopped());
        CHECK(!service->started());

        // Start service
        service->start().wait();
        CHECK(service->started());
        CHECK(!service->stopped());

        // Update service
        service->update().wait();
        CHECK(service->is_updated());

        // Stop service
        service->stop().wait();
        CHECK(service->stopped());
        CHECK(!service->started());

        // Erase Service
        sight::service::unregister_service(service);
    }

//------------------------------------------------------------------------------

    TEST_CASE("start_stop_update_exceptions")
    {
        // Test on the same worker
        {
            auto service = sight::service::add<sight::service::ut::test_service>("sight::service::ut::test_no_data");

            // Service must be stop when it is created
            CHECK(service->stopped());

            // Start service
            service->start().wait();
            CHECK(service->started());

            // Stop service
            service->stop().wait();
            CHECK(service->stopped());

            // Start service with exceptions
            service->set_raise_exception(true);
            service->start().wait();
            CHECK(service->stopped());

            // Check we can catch the exception
            bool exception_caught = false;
            try
            {
                service->start().get();
            }
            catch(const sight::core::exception& e)
            {
                exception_caught = true;
                CHECK_EQ(std::string("start error"), std::string(e.what()));
            }
            CHECK(exception_caught);
            CHECK(service->stopped());

            // Start service again
            service->set_raise_exception(false);
            service->start().wait();
            CHECK(service->started());

            // Update service
            service->update().wait();
            CHECK(service->is_updated());
            service->reset_is_updated();
            CHECK(!service->is_updated());

            // Update service with exception caught
            service->set_raise_exception(true);
            exception_caught = false;
            try
            {
                service->update().get();
            }
            catch(sight::core::exception& e)
            {
                exception_caught = true;
                CHECK_EQ(std::string("update error"), std::string(e.what()));
            }
            CHECK(exception_caught);
            CHECK(!service->is_updated());

            // Update service without exception caught
            service->update().wait();
            CHECK(!service->is_updated());

            // Update service
            service->set_raise_exception(false);
            service->update().wait();
            CHECK(service->is_updated());

            // Stop service with exception caught
            service->set_raise_exception(true);
            exception_caught = false;
            try
            {
                service->stop().get();
            }
            catch(sight::core::exception& e)
            {
                exception_caught = true;
                CHECK_EQ(std::string("stop error"), std::string(e.what()));
            }
            CHECK(exception_caught);
            CHECK(service->started());

            // Update service without exception caught
            service->stop().wait();
            CHECK(service->started());

            service->set_raise_exception(false);
            service->stop().wait();
            CHECK(service->stopped());

            // Erase Service
            sight::service::unregister_service(service);
        }
        // Test on a different worker
        {
            auto service = sight::service::add<sight::service::ut::test_service>("sight::service::ut::test_no_data");
            auto worker  = sight::core::thread::worker::make();
            service->set_worker(worker);

            // Service must be stop when it is created
            CHECK(service->stopped());

            // Start service
            service->start().wait();
            CHECK(service->started());

            // Stop service
            service->stop().wait();
            CHECK(service->stopped());

            // Start service with exceptions
            service->set_raise_exception(true);
            service->start().wait();
            CHECK(service->stopped());

            // Check we can catch the exception
            bool exception_caught = false;
            try
            {
                service->start().get();
            }
            catch(const sight::core::exception& e)
            {
                exception_caught = true;
                CHECK_EQ(std::string("start error"), std::string(e.what()));
            }
            CHECK(exception_caught);
            CHECK(service->stopped());

            // Start service again
            service->set_raise_exception(false);
            service->start().wait();
            CHECK(service->started());

            // Update service
            service->update().wait();
            CHECK(service->is_updated());
            service->reset_is_updated();
            CHECK(!service->is_updated());

            // Update service with exception caught
            service->set_raise_exception(true);
            exception_caught = false;
            try
            {
                service->update().get();
            }
            catch(sight::core::exception& e)
            {
                exception_caught = true;
                CHECK_EQ(std::string("update error"), std::string(e.what()));
            }
            CHECK(exception_caught);
            CHECK(!service->is_updated());

            // Update service without exception caught
            service->update().wait();
            CHECK(!service->is_updated());

            // Update service
            service->set_raise_exception(false);
            service->update().wait();
            CHECK(service->is_updated());

            // Stop service with exception caught
            service->set_raise_exception(true);
            exception_caught = false;
            try
            {
                service->stop().get();
            }
            catch(sight::core::exception& e)
            {
                exception_caught = true;
                CHECK_EQ(std::string("stop error"), std::string(e.what()));
            }
            CHECK(exception_caught);
            CHECK(service->started());

            // Update service without exception caught
            service->stop().wait();
            CHECK(service->started());

            service->set_raise_exception(false);
            service->stop().wait();
            CHECK(service->stopped());

            // Erase Service
            sight::service::unregister_service(service);
            worker->stop();
        }
    }

//------------------------------------------------------------------------------

    struct test_service_signals : public sight::core::com::has_slots
    {
        using sptr = std::shared_ptr<test_service_signals>;

        test_service_signals()
        {
            new_slot("start", &test_service_signals::start, this);
            new_slot("update", &test_service_signals::update, this);
            new_slot("stop", &test_service_signals::stop, this);

            m_worker = sight::core::thread::worker::make();
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

        sight::core::thread::worker::sptr m_worker;
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

    TEST_CASE("communication")
    {
        const std::string event         = "EVENT";
        const std::string data_key      = "data1";
        const std::string service1_uuid = "service1UUID";
        const std::string service2_uuid = "service2UUID";

        sight::data::map::sptr obj = std::make_shared<sight::data::map>();
        sight::service::ut::test_service::sptr service1;
        sight::service::ut::test_service::sptr service2;

        // Add services
        sight::service::add("sight::service::ut::test1_input", service1_uuid);
        service1 = std::dynamic_pointer_cast<sight::service::ut::test_srv>(sight::service::get(service1_uuid));
        CHECK(service1);
        service1->set_input(obj, data_key, true);

        service2 = sight::service::add<sight::service::ut::test_srv>("sight::service::ut::test1_input", service2_uuid);
        CHECK(service2);
        service2->set_input(obj, data_key, true);

        // Object used to check service signals
        test_service_signals::sptr receiver1 = std::make_shared<test_service_signals>();
        test_service_signals::sptr receiver2 = std::make_shared<test_service_signals>();

        sight::core::com::helper::sig_slot_connection com_helper;
        com_helper.connect(service1, sight::service::signals::STARTED, receiver1, "start");
        com_helper.connect(service1, sight::service::signals::UPDATED, receiver1, "update");
        com_helper.connect(service1, sight::service::signals::STOPPED, receiver1, "stop");

        CHECK_EQ(false, receiver1->m_started);
        CHECK_EQ(false, receiver1->m_updated);
        CHECK_EQ(false, receiver1->m_stopped);

        com_helper.connect(service2, sight::service::signals::STARTED, receiver2, "start");
        com_helper.connect(service2, sight::service::signals::UPDATED, receiver2, "update");
        com_helper.connect(service2, sight::service::signals::STOPPED, receiver2, "stop");

        CHECK_EQ(false, receiver2->m_started);
        CHECK_EQ(false, receiver2->m_updated);
        CHECK_EQ(false, receiver2->m_stopped);

        // Start services
        service1->start().wait();
        service2->start().wait();
        CHECK(service1->started());
        CHECK(service2->started());

        SIGHT_TEST_WAIT(receiver1->m_started && receiver2->m_started)
        CHECK_EQ(true, receiver1->m_started);
        CHECK_EQ(false, receiver1->m_updated);
        CHECK_EQ(false, receiver1->m_stopped);
        CHECK_EQ(true, receiver2->m_started);
        CHECK_EQ(false, receiver2->m_updated);
        CHECK_EQ(false, receiver2->m_stopped);

        // Register communication channel
        com_helper.connect(
            service1,
            sight::service::ut::test_srv::signals::MSG_SENT,
            service2,
            sight::service::ut::test_srv::slots::UPDATE2
        );

        CHECK(!service2->is_updated2());

        // Service1 send notification
        {
            service1->async_emit(service1.get(), sight::service::ut::test_srv::signals::MSG_SENT, event);
        }

        service1->update().wait();
        service2->update().wait();
        CHECK(service2->is_updated2());

        SIGHT_TEST_WAIT(receiver1->m_updated && receiver2->m_updated)
        CHECK_EQ(true, receiver1->m_started);
        CHECK_EQ(true, receiver1->m_updated);
        CHECK_EQ(false, receiver1->m_stopped);
        CHECK_EQ(true, receiver2->m_started);
        CHECK_EQ(true, receiver2->m_updated);
        CHECK_EQ(false, receiver2->m_stopped);

        // Test if service2 has received the message
        service1->stop().wait();
        service2->stop().wait();

        SIGHT_TEST_WAIT(receiver1->m_stopped && receiver2->m_stopped)
        CHECK_EQ(true, receiver1->m_started);
        CHECK_EQ(true, receiver1->m_updated);
        CHECK_EQ(true, receiver1->m_stopped);
        CHECK_EQ(true, receiver2->m_started);
        CHECK_EQ(true, receiver2->m_updated);
        CHECK_EQ(true, receiver2->m_stopped);

        com_helper.disconnect();

        sight::service::unregister_service(service1);
        sight::service::unregister_service(service2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("with_in_and_out")
    {
        std::vector<sight::data::integer::sptr> obj;
        obj.push_back(std::make_shared<sight::data::integer>(18));
        obj.push_back(std::make_shared<sight::data::integer>(23));
        auto service = sight::service::add<sight::service::ut::test_service_with_data>(
            "sight::service::ut::test_service_with_data"
        );

        CHECK(service);
        service->set_input(obj[0], sight::service::ut::test_service_with_data::INPUT);
        service->set_inout(obj[0], sight::service::ut::test_service_with_data::INOUT_GROUP, true, false, 0);
        service->set_inout(obj[1], sight::service::ut::test_service_with_data::INOUT_GROUP, true, false, 1);

        service->start().wait();
        CHECK(service->started());
        CHECK(
            nullptr
            != service->sight::data::has_data::object(
                sight::service::ut::test_service_with_data::INPUT,
                sight::data::access::in
            )
        );
        service->update().wait();
        CHECK(
            nullptr
            != service->sight::data::has_data::object(
                sight::service::ut::test_service_with_data::OUTPUT,
                sight::data::access::out
            )
        );
        sight::data::object::csptr output = service->sight::data::has_data::object(
            sight::service::ut::test_service_with_data::OUTPUT,
            sight::data::access::out
        );
        CHECK(output);
        sight::data::integer::csptr out_integer = std::dynamic_pointer_cast<const sight::data::integer>(output);
        CHECK(out_integer);

        CHECK_EQ(
            obj[0]->value(),
            out_integer->value()
        );

        {
            auto out_integer2 =
                service->output<sight::data::integer>(sight::service::ut::test_service_with_data::OUTPUT).lock();
            CHECK(out_integer2);

            CHECK_EQ(obj[0]->value(), out_integer2->value());
        }

        // Test index access
        CHECK_EQ(std::size_t(2), service->m_inout_group.size());
        CHECK_EQ(obj[0]->value(), service->m_inout_group[0].lock()->value());
        CHECK_EQ(obj[1]->value(), service->m_inout_group[1].lock()->value());

        // Test iterator access
        auto it_obj = obj.begin();
        for(const auto& data : service->m_inout_group)
        {
            auto i = (*it_obj)->value();
            CHECK_EQ(i, data.second->lock()->value());
            ++it_obj;
        }

        service->stop().wait();

        auto null_integer = service->output<sight::data::integer>(sight::service::ut::test_service_with_data::OUTPUT);
        CHECK(null_integer.expired());

        CHECK(
            nullptr
            == service->sight::data::has_data::object(
                sight::service::ut::test_service_with_data::OUTPUT,
                sight::data::access::out
            )
        );
        CHECK(
            nullptr
            != service->sight::data::has_data::object(
                sight::service::ut::test_service_with_data::INPUT,
                sight::data::access::in
            )
        );

        sight::service::unregister_service(service);
    }

//------------------------------------------------------------------------------

    TEST_CASE("properties")
    {
        {
            // Default value, property object automatically created
            auto service =
                sight::service::add<sight::service::ut::test1_property>("sight::service::ut::test1_property");
            sight::service::config_t config;
            CHECK_EQ(sight::service::base::configuration_status::unconfigured, service->config_status());
            service->set_config(config);
            service->configure();
            CHECK_EQ(sight::service::base::configuration_status::configured, service->config_status());

            service->start().wait();
            CHECK(service->started());
            CHECK(nullptr != service->sight::data::has_data::object("prop1", sight::data::access::inout));
            service->update().wait();
            CHECK_EQ(std::int64_t(12), *service->m_prop1);
            service->stop().wait();
        }
        {
            // Value set directly in configuring
            auto service =
                sight::service::add<sight::service::ut::test1_property>("sight::service::ut::test1_property");
            sight::service::config_t config;
            config.add("properties.<xmlattr>.prop1", 1234);

            service->set_config(config);
            service->configure();

            service->start().wait();
            CHECK(service->started());
            CHECK(nullptr != service->sight::data::has_data::object("prop1", sight::data::access::inout));
            service->update().wait();
            CHECK_EQ(std::int64_t(1234), service->m_prop1.value());
            service->stop().wait();
        }
        {
            // Value set as object
            auto service =
                sight::service::add<sight::service::ut::test1_property>("sight::service::ut::test1_property");
            const auto i1 = std::make_shared<sight::data::integer>(18);
            sight::service::config_t config;
            service->set_config(config);
            service->set_inout(i1, "prop1");
            service->configure();

            service->start().wait();
            CHECK(service->started());
            CHECK(nullptr != service->sight::data::has_data::object("prop1", sight::data::access::inout));
            service->update().wait();
            CHECK_EQ(i1->value(), *service->m_prop1);
            service->stop().wait();
            sight::service::unregister_service(service);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "test_auto_connections")
    {
        const std::string data_key1     = "data1";
        const std::string data_key2     = "data2";
        const std::string data_key3     = "data3";
        sight::data::integer::sptr obj1 = std::make_shared<sight::data::integer>();
        sight::data::integer::sptr obj2 = std::make_shared<sight::data::integer>();
        sight::data::integer::sptr obj3 = std::make_shared<sight::data::integer>();

        // Test if the object support the service
        CHECK(
            sight::service::extension::factory::get()->support(
                obj1->get_classname(),
                "sight::service::ut::test_service"
            )
        );

        // Test adding service
        auto srv = sight::service::add<sight::service::ut::test_srv>("sight::service::ut::test2_inouts1_input");
        srv->set_inout(obj1, data_key1);
        srv->set_inout(obj2, data_key2);
        srv->set_input(obj3, data_key3);
        srv->set_worker(m_worker);

        srv->start().wait();

        CHECK(not srv->is_updated());
        CHECK(not srv->is_updated2());

        // Auto-connected by default because there is a match in the auto_connections() map
        obj1->signal<sight::data::object::modified_signal_t>(sight::data::object::MODIFIED_SIG)->emit();

        CHECK(srv->is_updated());
        CHECK(not srv->is_updated2());

        srv->reset_is_updated();

        // Auto-connected by default because there is a match in the auto_connections() map
        obj2->signal<sight::data::object::modified_signal_t>(sight::data::object::MODIFIED_SIG)->emit();

        CHECK(not srv->is_updated());
        CHECK(srv->is_updated2());

        srv->stop().wait();

        // Auto-connected by default because there is a match in the auto_connections() map
        // BUT, it has been configured to false in set_inout()
        srv->set_inout(obj2, data_key2, false, false);
        srv->start().wait();

        srv->reset_is_updated2();

        obj2->signal<sight::data::object::modified_signal_t>(sight::data::object::MODIFIED_SIG)->emit();

        CHECK(not srv->is_updated());
        CHECK(not srv->is_updated2());

        // Not auto-connected by default because there is no match in the auto_connections() map
        obj3->signal<sight::data::object::modified_signal_t>(sight::data::object::MODIFIED_SIG)->emit();

        CHECK(not srv->is_updated());
        CHECK(not srv->is_updated2());

        srv->stop().wait();

        sight::service::unregister_service(srv);
    }

//------------------------------------------------------------------------------
} // end TEST_SUITE
