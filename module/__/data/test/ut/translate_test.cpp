/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <data/boolean.hpp>
#include <data/integer.hpp>
#include <data/string.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::module::data::translate")
{
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

    TEST_CASE("string_to_string")
    {
        auto srv = sight::service::add("sight::module::data::translate");
        using namespace std::literals::string_literals;

        auto key   = std::make_shared<sight::data::string>();
        auto value = std::make_shared<sight::data::string>();

        srv->set_input(key, "key");
        srv->set_inout(value, "value");

        {
            std::stringstream config_string;
            config_string
            << "<map>"
               "    <entry key=\"1\" value=\"value1\" />"
               "    <entry key=\"#2\" value=\"value2\" />"
               "    <entry key=\"Num 3\" value=\"value3\" />"
               "</map>";
            sight::service::base::config_t config;
            boost::property_tree::read_xml(config_string, config);
            srv->set_config(config);
        }

        CHECK_NOTHROW(srv->configure());
        CHECK_NOTHROW(srv->start().get());
        // Test that it does NOT translate at start if the key is NOT set
        CHECK_EQ(""s, value->get_value());

        // Test that it translates at start if the key is set
        CHECK_NOTHROW(srv->stop().get());
        key->set_value("#2");
        CHECK_NOTHROW(srv->start().get());
        CHECK_EQ("value2"s, value->get_value());

        // Test that it translates at update if the key is set
        key->set_value("1");
        CHECK_NOTHROW(srv->update().get());
        CHECK_EQ("value1"s, value->get_value());

        // Test that it translates when data is modified if the key is set
        key->set_value("Num 3");
        key->emit(sight::data::object::MODIFIED_SIG);
        CHECK_EQ("value3"s, value->get_value());

        CHECK_NOTHROW(srv->stop().get());
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE("integer_to_boolean")
    {
        auto srv = sight::service::add("sight::module::data::translate");
        using namespace std::literals::string_literals;

        auto key   = std::make_shared<sight::data::integer>();
        auto value = std::make_shared<sight::data::boolean>();

        srv->set_input(key, "key");
        srv->set_inout(value, "value");

        {
            std::stringstream config_string;
            config_string
            << "<map>"
               "    <entry key=\"1\" value=\"false\" />"
               "    <entry key=\"2\" value=\"false\" />"
               "    <entry key=\"-14\" value=\"true\" />"
               "</map>";
            sight::service::base::config_t config;
            boost::property_tree::read_xml(config_string, config);
            srv->set_config(config);
        }

        CHECK_NOTHROW(srv->configure());
        CHECK_NOTHROW(srv->start().get());

        key->set_value(1);
        CHECK_NOTHROW(srv->update().get());
        CHECK_EQ(false, value->get_value());

        // Test that it does NOT translate at update if the key does not exists
        key->set_value(5);
        CHECK_THROWS_AS(srv->update().get(), sight::core::exception);
        CHECK_EQ(false, value->get_value());

        key->set_value(-14);
        key->emit(sight::data::object::MODIFIED_SIG);
        CHECK_EQ(true, value->get_value());

        CHECK_NOTHROW(srv->stop().get());
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::module::data::translate")
