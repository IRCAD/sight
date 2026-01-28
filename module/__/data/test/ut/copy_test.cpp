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
        srv->set_input(source, "source");
        boost::property_tree::ptree ptree;
        ptree.put("in", "");
        ptree.put("out", "");
        ptree.put("mode", "copyOnStart");
        srv->set_config(ptree);
        CHECK_NOTHROW(srv->configure());

        CHECK_NOTHROW(srv->start().get());
        auto out_target = srv->output<sight::data::string>("outTarget");
        CHECK_EQ("Hello world"s, out_target.lock()->get_value());

        CHECK_NOTHROW(srv->stop().get());
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE("on_update")
    {
        auto srv = sight::service::add("sight::module::data::copy");
        using namespace std::literals::string_literals;

        auto source = std::make_shared<sight::data::string>("Hello world");
        srv->set_input(source, "source");
        auto target = std::make_shared<sight::data::string>();
        srv->set_inout(target, "target");
        boost::property_tree::ptree ptree;
        ptree.put("in", "");
        ptree.put("inout", "");
        srv->set_config(ptree);
        CHECK_NOTHROW(srv->configure());
        CHECK_NOTHROW(srv->start().get());

        CHECK_NOTHROW(srv->update().get());
        CHECK_EQ("Hello world"s, target->get_value());

        CHECK_NOTHROW(srv->stop().get());
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::module::data::copy")
