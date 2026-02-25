/************************************************************************
 *
 * Copyright (C) 2024-2026 IRCAD France
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

#include "test_services.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <service/op.hpp>

#include <utest/wait.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

#include <ranges>

namespace
{

struct fixture
{
    fixture()
    {
        // Set up context before running a test
        sight::core::runtime::init();

        std::filesystem::path location = sight::core::runtime::get_resource_file_path("app_ut");
        CHECK(std::filesystem::exists(location));

        sight::core::runtime::add_modules(location);
        sight::core::runtime::load_module("sight::module::app");
    }
};

//------------------------------------------------------------------------------

auto create_order_srv(bool _start = true)
{
    auto srv = sight::service::add<sight::app::ut::test_order_srv>("sight::app::ut::test_order_srv");
    CHECK_NOTHROW(srv->configure());
    if(_start)
    {
        CHECK_NOTHROW(srv->start().get());
    }

    return srv;
}

} // namespace

TEST_SUITE("sight::app::update_sequence")
{
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "basic")
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
        sight::service::config_t config;
        boost::property_tree::read_xml(srv_config, config);

        auto update_srv = sight::service::add("sight::app::update_sequence");
        CHECK(update_srv->is_a("sight::app::update_sequence"));
        CHECK(update_srv->is_a("sight::app::updater"));
        update_srv->set_config(config);
        CHECK_NOTHROW(update_srv->configure());
        CHECK_NOTHROW(update_srv->start().get());

        sight::app::ut::test_order_srv::s_ORDER = 0;
        update_srv->update().get();
        CHECK_EQ((unsigned int) (0), srv0->update_order());
        CHECK_EQ((unsigned int) (1), srv1->update_order());
        CHECK_EQ((unsigned int) (2), srv2->update_order());
        CHECK_EQ((unsigned int) (3), srv3->update_order());

        CHECK_NOTHROW(update_srv->stop().get());
        CHECK_NOTHROW(srv0->stop().get());
        CHECK_NOTHROW(srv1->stop().get());
        CHECK_NOTHROW(srv2->stop().get());
        CHECK_NOTHROW(srv3->stop().get());

        sight::service::remove(update_srv);
        sight::service::remove(srv0);
        sight::service::remove(srv1);
        sight::service::remove(srv2);
        sight::service::remove(srv3);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "parent")
    {
        std::array<sight::app::ut::test_order_srv::sptr, 7> srv;
        for(const auto i : std::views::iota(0U, 7U))
        {
            srv[i] = create_order_srv();
        }

        const auto create_updater = [](std::stringstream& _config)
                                    {
                                        sight::service::config_t srv_config;
                                        boost::property_tree::read_xml(_config, srv_config);

                                        auto updater = sight::service::add("sight::app::update_sequence");
                                        updater->set_config(srv_config);
                                        CHECK_NOTHROW(updater->configure());
                                        CHECK_NOTHROW(updater->start().get());
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

        sight::app::ut::test_order_srv::s_ORDER = 0;
        main_updater->update().get();

        for(const auto i : std::views::iota(0U, 7U))
        {
            CHECK_EQ(i, srv[i]->update_order());
        }

        CHECK_NOTHROW(main_updater->stop().get());
        CHECK_NOTHROW(child_updater_1->stop().get());
        CHECK_NOTHROW(child_updater_1_1->stop().get());
        CHECK_NOTHROW(child_updater_2->stop().get());

        sight::service::remove(main_updater);
        sight::service::remove(child_updater_1);
        sight::service::remove(child_updater_1_1);
        sight::service::remove(child_updater_2);

        for(const auto i : std::views::iota(0U, 7U))
        {
            CHECK_NOTHROW(srv[i]->stop().get());
            sight::service::remove(srv[i]);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "call_start_stop")
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
        sight::service::config_t config;
        boost::property_tree::read_xml(srv_config, config);

        auto update_srv = sight::service::add("sight::app::update_sequence");
        CHECK(update_srv->is_a("sight::app::update_sequence"));
        CHECK(update_srv->is_a("sight::app::updater"));
        update_srv->set_config(config);
        CHECK_NOTHROW(update_srv->configure());
        CHECK_NOTHROW(update_srv->start().get());

        CHECK_EQ(true, srv_to_stop->started());
        CHECK_EQ(true, srv_to_start->stopped());

        update_srv->update().get();

        CHECK_EQ(true, srv_to_stop->stopped());
        CHECK_EQ(true, srv_to_start->started());

        CHECK_NOTHROW(update_srv->stop().get());
        CHECK_NOTHROW(srv_to_start->stop().get());

        sight::service::remove(update_srv);
        sight::service::remove(srv_to_stop);
        sight::service::remove(srv_to_start);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "call_slot_while_stopped")
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
        sight::service::config_t config;
        boost::property_tree::read_xml(srv_config, config);

        auto update_srv = sight::service::add("sight::app::update_sequence");
        CHECK(update_srv->is_a("sight::app::update_sequence"));
        CHECK(update_srv->is_a("sight::app::updater"));
        update_srv->set_config(config);
        CHECK_NOTHROW(update_srv->configure());
        CHECK_NOTHROW(update_srv->start().get());

        CHECK_EQ(true, srv_to_stop_1->started());
        CHECK_EQ(true, srv_to_update_but_stopped->stopped());

        update_srv->update().get();

        // srv_to_stop1 is skipped since srv_to_update_but_stopped cannot be updated
        CHECK_EQ(false, srv_to_stop_1->stopped());
        CHECK_EQ(true, srv_to_update_but_stopped->stopped());

        CHECK_NOTHROW(update_srv->stop().get());
        CHECK_NOTHROW(srv_to_stop_1->stop().get());

        sight::service::remove(update_srv);
        sight::service::remove(srv_to_stop_1);
        sight::service::remove(srv_to_update_but_stopped);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "call_slot_after_a_start")
    {
        auto srv_to_update_but_stopped = create_order_srv(false);
        auto srv_to_stop_1             = create_order_srv();

        std::stringstream srv_config;
        srv_config
        << "<config>"
        << "<service uid=" << std::quoted(srv_to_update_but_stopped->get_id()) << " slot=\"start\" />"
        << "<service uid=" << std::quoted(srv_to_update_but_stopped->get_id()) << " />"
        << "<service uid=" << std::quoted(srv_to_stop_1->get_id()) << " slot=\"stop\" />"
        << "</config>"
        ;
        sight::service::config_t config;
        boost::property_tree::read_xml(srv_config, config);

        auto update_srv = sight::service::add("sight::app::update_sequence");
        CHECK(update_srv->is_a("sight::app::update_sequence"));
        CHECK(update_srv->is_a("sight::app::updater"));
        sight::app::ut::test_order_srv::s_ORDER = 1001;
        update_srv->set_config(config);
        CHECK_EQ((unsigned int) (0), srv_to_update_but_stopped->update_order());
        CHECK_NOTHROW(update_srv->configure());
        CHECK_NOTHROW(update_srv->start().get());

        CHECK_EQ(true, srv_to_stop_1->started());
        CHECK_EQ(true, srv_to_update_but_stopped->stopped());

        update_srv->update().get();

        CHECK_EQ(true, srv_to_stop_1->stopped());
        CHECK_EQ(true, srv_to_update_but_stopped->started());

        CHECK_NOTHROW(update_srv->stop().get());
        CHECK_NOTHROW(srv_to_update_but_stopped->stop().get());

        sight::service::remove(update_srv);
        sight::service::remove(srv_to_stop_1);
        sight::service::remove(srv_to_update_but_stopped);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "call_stop_while_stopped")
    {
        auto srv_to_stop = create_order_srv(false);

        std::stringstream srv_config;
        srv_config
        << "<config>"
        << "<service uid=" << std::quoted(srv_to_stop->get_id()) << " slot=\"stop\" />"
        << "</config>"
        ;
        sight::service::config_t config;
        boost::property_tree::read_xml(srv_config, config);

        auto update_srv = sight::service::add("sight::app::update_sequence");
        CHECK(update_srv->is_a("sight::app::update_sequence"));
        CHECK(update_srv->is_a("sight::app::updater"));
        update_srv->set_config(config);
        CHECK_NOTHROW(update_srv->configure());
        CHECK_NOTHROW(update_srv->start().get());

        CHECK_EQ(false, srv_to_stop->started());

        CHECK_NOTHROW(update_srv->update().get());

        CHECK_EQ(true, srv_to_stop->stopped());
        CHECK_EQ(false, srv_to_stop->started());

        CHECK_NOTHROW(update_srv->stop().get());

        sight::service::remove(update_srv);
        sight::service::remove(srv_to_stop);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "call_start_slot_stop")
    {
        auto srv = create_order_srv(false);

        std::stringstream srv_config;
        srv_config
        << "<config>"
        << "<service uid=" << std::quoted(srv->get_id()) << " slot=\"start\" />"
        << "<service uid=" << std::quoted(srv->get_id()) << " />"
        << "<service uid=" << std::quoted(srv->get_id()) << " slot=\"stop\"  />"
        << "</config>"
        ;
        sight::service::config_t config;
        boost::property_tree::read_xml(srv_config, config);

        auto update_srv = sight::service::add("sight::app::update_sequence");
        CHECK(update_srv->is_a("sight::app::update_sequence"));
        CHECK(update_srv->is_a("sight::app::updater"));
        update_srv->set_config(config);
        CHECK_NOTHROW(update_srv->configure());
        CHECK_NOTHROW(update_srv->start().get());

        CHECK_EQ(false, srv->started());
        sight::app::ut::test_order_srv::s_ORDER = 100;
        update_srv->update().get();
        CHECK_EQ((unsigned int) (100), srv->update_order());

        CHECK_EQ(true, srv->stopped());
        CHECK_EQ(false, srv->started());

        CHECK_NOTHROW(update_srv->stop().get());

        sight::service::remove(update_srv);
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "call_stop_slot_start")
    {
        auto srv = create_order_srv(false);

        std::stringstream srv_config;
        srv_config
        << "<config>"
        << "<service uid=" << std::quoted(srv->get_id()) << " slot=\"stop\" />"
        << "<service uid=" << std::quoted(srv->get_id()) << " />"
        << "<service uid=" << std::quoted(srv->get_id()) << " slot=\"start\"  />"
        << "</config>"
        ;
        sight::service::config_t config;
        boost::property_tree::read_xml(srv_config, config);

        auto update_srv = sight::service::add("sight::app::update_sequence");
        CHECK(update_srv->is_a("sight::app::update_sequence"));
        CHECK(update_srv->is_a("sight::app::updater"));
        update_srv->set_config(config);
        CHECK_NOTHROW(update_srv->configure());
        CHECK_NOTHROW(update_srv->start().get());

        CHECK_EQ(false, srv->started());
        sight::app::ut::test_order_srv::s_ORDER = 99;
        update_srv->update().get();
        CHECK_EQ((unsigned int) (0), srv->update_order());

        CHECK_EQ(true, srv->stopped());
        CHECK_EQ(false, srv->started());

        CHECK_NOTHROW(update_srv->stop().get());

        sight::service::remove(update_srv);
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "call_stop_start")
    {
        auto srv = create_order_srv();

        std::stringstream srv_config;
        srv_config
        << "<config>"
        << "<service uid=" << std::quoted(srv->get_id()) << " slot=\"stop\" />"
        << "<service uid=" << std::quoted(srv->get_id()) << " slot=\"start\"  />"
        << "</config>"
        ;
        sight::service::config_t config;
        boost::property_tree::read_xml(srv_config, config);

        auto update_srv = sight::service::add("sight::app::update_sequence");
        CHECK(update_srv->is_a("sight::app::update_sequence"));
        CHECK(update_srv->is_a("sight::app::updater"));
        update_srv->set_config(config);
        CHECK_NOTHROW(update_srv->configure());
        CHECK_NOTHROW(update_srv->start().get());
        bool call_start = false;
        bool call_stop  = false;

        auto start_called = sight::core::com::new_slot(
            [&call_start]()
        {
            call_start = true;
        });

        start_called->set_worker(sight::core::thread::get_default_worker());
        srv->signal("started")->connect(start_called);

        auto stop_called = sight::core::com::new_slot(
            [&call_stop]()
        {
            call_stop = true;
        });

        stop_called->set_worker(sight::core::thread::get_default_worker());
        srv->signal("stopped")->connect(stop_called);

        CHECK_EQ(true, srv->started());
        CHECK_EQ(false, srv->stopped());

        update_srv->update().get();

        CHECK_EQ(true, srv->started());
        CHECK_EQ(false, srv->stopped());

        SIGHT_TEST_WAIT(call_start == true);
        SIGHT_TEST_WAIT(call_stop == true);

        CHECK_NOTHROW(update_srv->stop().get());
        CHECK_NOTHROW(srv->stop().get());

        sight::service::remove(update_srv);
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "ignore_stopped")
    {
        auto srv0 = create_order_srv();
        auto srv1 = create_order_srv(false);
        auto srv2 = create_order_srv();
        auto srv3 = create_order_srv();

        std::stringstream srv_config;
        srv_config
        << "<config>"
        << "<service uid=" << std::quoted(srv0->get_id()) << "/>"
        << "<service uid=" << std::quoted(srv1->get_id()) << " ignore_stopped=\"true\" />"
        << "<service uid=" << std::quoted(srv2->get_id()) << "/>"
        << "<service uid=" << std::quoted(srv3->get_id()) << "/>"
        << "</config>"
        ;
        sight::service::config_t config;
        boost::property_tree::read_xml(srv_config, config);

        auto update_srv = sight::service::add("sight::app::update_sequence");
        CHECK(update_srv->is_a("sight::app::update_sequence"));
        CHECK(update_srv->is_a("sight::app::updater"));
        update_srv->set_config(config);
        CHECK_NOTHROW(update_srv->configure());
        CHECK_NOTHROW(update_srv->start().get());

        sight::app::ut::test_order_srv::s_ORDER = 0;
        update_srv->update().get();
        CHECK_EQ((unsigned int) (0), srv0->update_order());
        CHECK_EQ((unsigned int) (0), srv1->update_order());
        CHECK_EQ((unsigned int) (1), srv2->update_order());
        CHECK_EQ((unsigned int) (2), srv3->update_order());

        CHECK_NOTHROW(update_srv->stop().get());
        CHECK_NOTHROW(srv3->stop().get());
        CHECK_NOTHROW(srv2->stop().get());
        CHECK_NOTHROW(srv0->stop().get());

        sight::service::remove(update_srv);
        sight::service::remove(srv3);
        sight::service::remove(srv2);
        sight::service::remove(srv1);
        sight::service::remove(srv0);
    }
} // TEST_SUITE
