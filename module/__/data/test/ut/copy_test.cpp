/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include <data/string.hpp>

#include <service/op.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::module::data::copy")
{
//------------------------------------------------------------------------------

    TEST_CASE("on_start")
    {
        auto srv = sight::service::add("sight::module::data::copy");
        using namespace std::literals::string_literals;

        auto source = std::make_shared<sight::data::string>("Hello world");
        srv->set_input(source, "data.source", true, {}, 0);
        auto target = std::make_shared<sight::data::string>();
        srv->set_inout(target, "data.target", true, {}, 0);
        boost::property_tree::ptree ptree;
        ptree.put("mode", "copyOnStart");
        srv->set_config(ptree);
        CHECK_NOTHROW(srv->configure());

        CHECK_NOTHROW(srv->start().get());
        CHECK_EQ("Hello world"s, target->get_value());

        CHECK_NOTHROW(srv->stop().get());
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE("on_update")
    {
        auto srv = sight::service::add("sight::module::data::copy");
        using namespace std::literals::string_literals;

        auto source = std::make_shared<sight::data::string>("Hello world");
        srv->set_input(source, "data.source", true, {}, 0);
        auto target = std::make_shared<sight::data::string>();
        srv->set_inout(target, "data.target", true, {}, 0);
        boost::property_tree::ptree ptree;
        srv->set_config(ptree);
        CHECK_NOTHROW(srv->configure());
        CHECK_NOTHROW(srv->start().get());

        CHECK_NOTHROW(srv->update().get());
        CHECK_EQ("Hello world"s, target->get_value());

        CHECK_NOTHROW(srv->stop().get());
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE("on_update_batch")
    {
        auto srv = sight::service::add("sight::module::data::copy");
        using namespace std::literals::string_literals;

        auto source1 = std::make_shared<sight::data::string>("First");
        auto source2 = std::make_shared<sight::data::string>("Second");
        auto source3 = std::make_shared<sight::data::string>("Third");

        auto target1 = std::make_shared<sight::data::string>();
        auto target2 = std::make_shared<sight::data::string>();
        auto target3 = std::make_shared<sight::data::string>();

        srv->set_input(source1, "data.source", true, {}, 0);
        srv->set_input(source2, "data.source", true, {}, 1);
        srv->set_input(source3, "data.source", true, {}, 2);

        srv->set_inout(target1, "data.target", true, {}, 0);
        srv->set_inout(target2, "data.target", true, {}, 1);
        srv->set_inout(target3, "data.target", true, {}, 2);

        boost::property_tree::ptree ptree;
        srv->set_config(ptree);
        CHECK_NOTHROW(srv->configure());
        CHECK_NOTHROW(srv->start().get());

        CHECK_NOTHROW(srv->update().get());
        CHECK_EQ("First"s, target1->get_value());
        CHECK_EQ("Second"s, target2->get_value());
        CHECK_EQ("Third"s, target3->get_value());

        CHECK_NOTHROW(srv->stop().get());
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE("on_start_batch")
    {
        auto srv = sight::service::add("sight::module::data::copy");
        using namespace std::literals::string_literals;

        auto source1 = std::make_shared<sight::data::string>("Data1");
        auto source2 = std::make_shared<sight::data::string>("Data2");

        auto target1 = std::make_shared<sight::data::string>();
        auto target2 = std::make_shared<sight::data::string>();

        srv->set_input(source1, "data.source", true, {}, 0);
        srv->set_input(source2, "data.source", true, {}, 1);

        srv->set_inout(target1, "data.target", true, {}, 0);
        srv->set_inout(target2, "data.target", true, {}, 1);

        boost::property_tree::ptree ptree;
        ptree.put("mode", "copyOnStart");
        srv->set_config(ptree);
        CHECK_NOTHROW(srv->configure());

        // Copy should happen during start
        CHECK_NOTHROW(srv->start().get());
        CHECK_EQ("Data1"s, target1->get_value());
        CHECK_EQ("Data2"s, target2->get_value());

        // Updating should not copy again in copyOnStart mode
        CHECK_NOTHROW(srv->update().get());
        // Target should still have the old value
        CHECK_EQ("Data1"s, target1->get_value());
        CHECK_EQ("Data2"s, target2->get_value());

        CHECK_NOTHROW(srv->stop().get());
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::module::data::copy")
