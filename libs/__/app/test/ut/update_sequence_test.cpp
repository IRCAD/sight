/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include "update_sequence_test.hpp"

#include "test_services.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <service/op.hpp>

#include <utest/wait.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <ranges>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::app::ut::update_sequence_test);

namespace sight::app::ut
{

//------------------------------------------------------------------------------

auto create_order_srv(bool _start = true)
{
    auto srv = service::add<sight::app::ut::test_order_srv>("sight::app::ut::test_order_srv");
    CPPUNIT_ASSERT_NO_THROW(srv->configure());
    if(_start)
    {
        CPPUNIT_ASSERT_NO_THROW(srv->start().get());
    }

    return srv;
}

//------------------------------------------------------------------------------

void update_sequence_test::setUp()
{
    // Set up context before running a test
    core::runtime::init();

    std::filesystem::path location = core::runtime::get_resource_file_path("app_ut");
    CPPUNIT_ASSERT(std::filesystem::exists(location));

    core::runtime::add_modules(location);
    core::runtime::load_module("sight::module::app");
}

//------------------------------------------------------------------------------

void update_sequence_test::tearDown()
{
}

//------------------------------------------------------------------------------

void update_sequence_test::basic()
{
    auto srv0 = create_order_srv();
    auto srv1 = create_order_srv();
    auto srv2 = create_order_srv();
    auto srv3 = create_order_srv();

    std::stringstream srv_config;
    srv_config
    << "<config>"
    << "<service uid=" << std::quoted(srv0->get_id()) << "/>"
    << "<service uid=" << std::quoted(srv1->get_id()) << "/>"
    << "<service uid=" << std::quoted(srv2->get_id()) << "/>"
    << "<service uid=" << std::quoted(srv3->get_id()) << "/>"
    << "</config>"
    ;
    service::config_t config;
    boost::property_tree::read_xml(srv_config, config);

    auto update_srv = service::add("sight::app::update_sequence");
    CPPUNIT_ASSERT(update_srv->is_a("sight::app::update_sequence"));
    CPPUNIT_ASSERT(update_srv->is_a("sight::app::updater"));
    update_srv->set_config(config);
    CPPUNIT_ASSERT_NO_THROW(update_srv->configure());
    CPPUNIT_ASSERT_NO_THROW(update_srv->start().get());

    test_order_srv::s_ORDER = 0;
    update_srv->update().get();
    CPPUNIT_ASSERT_EQUAL((unsigned int) (0), srv0->update_order());
    CPPUNIT_ASSERT_EQUAL((unsigned int) (1), srv1->update_order());
    CPPUNIT_ASSERT_EQUAL((unsigned int) (2), srv2->update_order());
    CPPUNIT_ASSERT_EQUAL((unsigned int) (3), srv3->update_order());

    CPPUNIT_ASSERT_NO_THROW(update_srv->stop().get());

    service::remove(update_srv);
}

//------------------------------------------------------------------------------

void update_sequence_test::parent()
{
    std::array<sight::app::ut::test_order_srv::sptr, 7> srv;
    for(const auto i : std::views::iota(0U, 7U))
    {
        srv[i] = create_order_srv();
    }

    const auto create_updater = [](std::stringstream& _config)
                                {
                                    service::config_t srv_config;
                                    boost::property_tree::read_xml(_config, srv_config);

                                    auto updater = service::add("sight::app::update_sequence");
                                    updater->set_config(srv_config);
                                    CPPUNIT_ASSERT_NO_THROW(updater->configure());
                                    CPPUNIT_ASSERT_NO_THROW(updater->start().get());
                                    return updater;
                                };

    sight::service::base::sptr main_updater;
    sight::service::base::sptr child_updater_1;
    sight::service::base::sptr child_updater_2;
    sight::service::base::sptr child_updater_1_1;

    const std::string updater_1   = "updater_1";
    const std::string updater_2   = "updater_2";
    const std::string updater_1_1 = "updater_1_1";
    {
        std::stringstream srv_config;
        srv_config
        << "<config>"
        << "<service uid=" << std::quoted(srv[0]->get_id()) << "/>"
        << "<updater uid=" << std::quoted(updater_1) << "/>"
        << "<service uid=" << std::quoted(srv[3]->get_id()) << "/>"
        << "<updater uid=" << std::quoted(updater_2) << "/>"
        << "<service uid=" << std::quoted(srv[6]->get_id()) << "/>"
        << "</config>";
        main_updater = create_updater(srv_config);
    }
    {
        std::stringstream srv_config;
        srv_config
        << "<config parent=" << std::quoted(updater_1) << ">"
        << "<updater uid=" << std::quoted(updater_1_1) << "/>"
        << "<service uid=" << std::quoted(srv[2]->get_id()) << "/>"
        << "</config>";
        child_updater_1 = create_updater(srv_config);
    }
    {
        std::stringstream srv_config;
        srv_config
        << "<config parent=" << std::quoted(updater_2) << ">"
        << "<service uid=" << std::quoted(srv[4]->get_id()) << "/>"
        << "<service uid=" << std::quoted(srv[5]->get_id()) << "/>"
        << "</config>";
        child_updater_2 = create_updater(srv_config);
    }
    {
        std::stringstream srv_config;
        srv_config
        << "<config parent=" << std::quoted(updater_1_1) << ">"
        << "<service uid=" << std::quoted(srv[1]->get_id()) << "/>"
        << "</config>";
        child_updater_1_1 = create_updater(srv_config);
    }

    test_order_srv::s_ORDER = 0;
    main_updater->update().get();

    for(const auto i : std::views::iota(0U, 7U))
    {
        CPPUNIT_ASSERT_EQUAL(i, srv[i]->update_order());
    }

    CPPUNIT_ASSERT_NO_THROW(main_updater->stop().get());
}

//------------------------------------------------------------------------------

void update_sequence_test::call_start_stop()
{
    auto srv_to_start = create_order_srv(false);
    auto srv_to_stop  = create_order_srv();

    std::stringstream srv_config;
    srv_config
    << "<config>"
    << "<service uid=" << std::quoted(srv_to_start->get_id()) << " slot=\"start\" />"
    << "<service uid=" << std::quoted(srv_to_stop->get_id()) << " slot=\"stop\" />"
    << "</config>"
    ;
    service::config_t config;
    boost::property_tree::read_xml(srv_config, config);

    auto update_srv = service::add("sight::app::update_sequence");
    CPPUNIT_ASSERT(update_srv->is_a("sight::app::update_sequence"));
    CPPUNIT_ASSERT(update_srv->is_a("sight::app::updater"));
    update_srv->set_config(config);
    CPPUNIT_ASSERT_NO_THROW(update_srv->configure());
    CPPUNIT_ASSERT_NO_THROW(update_srv->start().get());

    CPPUNIT_ASSERT_EQUAL(true, srv_to_stop->started());
    CPPUNIT_ASSERT_EQUAL(true, srv_to_start->stopped());

    update_srv->update().get();

    CPPUNIT_ASSERT_EQUAL(true, srv_to_stop->stopped());
    CPPUNIT_ASSERT_EQUAL(true, srv_to_start->started());

    CPPUNIT_ASSERT_NO_THROW(update_srv->stop().get());

    service::remove(update_srv);
}

//------------------------------------------------------------------------------

void update_sequence_test::call_slot_while_stopped()
{
    auto srv_to_update_but_stopped = create_order_srv(false);
    auto srv_to_stop_1             = create_order_srv();

    std::stringstream srv_config;
    srv_config
    << "<config>"
    << "<service uid=" << std::quoted(srv_to_update_but_stopped->get_id()) << " />"
    << "<service uid=" << std::quoted(srv_to_stop_1->get_id()) << " slot=\"stop\" />"
    << "</config>"
    ;
    service::config_t config;
    boost::property_tree::read_xml(srv_config, config);

    auto update_srv = service::add("sight::app::update_sequence");
    CPPUNIT_ASSERT(update_srv->is_a("sight::app::update_sequence"));
    CPPUNIT_ASSERT(update_srv->is_a("sight::app::updater"));
    update_srv->set_config(config);
    CPPUNIT_ASSERT_NO_THROW(update_srv->configure());
    CPPUNIT_ASSERT_NO_THROW(update_srv->start().get());

    CPPUNIT_ASSERT_EQUAL(true, srv_to_stop_1->started());
    CPPUNIT_ASSERT_EQUAL(true, srv_to_update_but_stopped->stopped());

    update_srv->update().get();

    // srv_to_stop1 is skipped since srv_to_update_but_stopped cannot be updated
    CPPUNIT_ASSERT_EQUAL(false, srv_to_stop_1->stopped());
    CPPUNIT_ASSERT_EQUAL(true, srv_to_update_but_stopped->stopped());

    CPPUNIT_ASSERT_NO_THROW(update_srv->stop().get());

    service::remove(update_srv);
}

} // namespace sight::app::ut
