/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "concat_test.hpp"

#include "data/object.hpp"

#include <data/boolean.hpp>
#include <data/integer.hpp>
#include <data/string.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::concat_test);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void concat_test::setUp()
{
}

//------------------------------------------------------------------------------

void concat_test::tearDown()
{
}

//------------------------------------------------------------------------------

void concat_test::string_to_string()
{
    auto srv = service::add("sight::module::data::concat");
    using namespace std::literals::string_literals;

    auto target = std::make_shared<sight::data::string>();
    auto prefix = std::make_shared<sight::data::string>();
    auto suffix = std::make_shared<sight::data::string>();

    srv->set_input(prefix, "fragments", true, false, 0);
    srv->set_input(target, "fragments", true, false, 1);
    srv->set_input(suffix, "fragments", true, false, 2);
    srv->set_inout(target, "target");

    CPPUNIT_ASSERT_NO_THROW(srv->configure());
    CPPUNIT_ASSERT_NO_THROW(srv->start().get());
    // Test that it does NOT concat at start
    CPPUNIT_ASSERT_EQUAL(""s, target->get_value());

    // Test that it concats at update if the key is set
    prefix->set_value("a");
    target->set_value("b");
    suffix->set_value("c");

    CPPUNIT_ASSERT_NO_THROW(srv->update().get());
    CPPUNIT_ASSERT_EQUAL("abc"s, target->get_value());

    // Test automatic update on fragment change
    target->set_value("b");
    prefix->set_value("x");
    prefix->emit(sight::data::object::MODIFIED_SIG);

    CPPUNIT_ASSERT_EQUAL("xbc"s, target->get_value());

    CPPUNIT_ASSERT_NO_THROW(srv->stop().get());
    service::remove(srv);
}

//------------------------------------------------------------------------------

void concat_test::integer_to_integer()
{
    auto srv = service::add("sight::module::data::concat");
    using namespace std::literals::string_literals;

    auto target = std::make_shared<sight::data::integer>();
    auto prefix = std::make_shared<sight::data::integer>();
    auto suffix = std::make_shared<sight::data::integer>();

    srv->set_input(prefix, "fragments", true, false, 0);
    srv->set_input(target, "fragments", true, false, 1);
    srv->set_input(suffix, "fragments", true, false, 2);
    srv->set_inout(target, "target");

    CPPUNIT_ASSERT_NO_THROW(srv->configure());
    CPPUNIT_ASSERT_NO_THROW(srv->start().get());
    // Test that it does NOT concat at start
    CPPUNIT_ASSERT_EQUAL(std::int64_t(0), target->get_value());

    // Test that it concats at update if the key is set
    prefix->set_value(1);
    target->set_value(2);
    suffix->set_value(3);

    CPPUNIT_ASSERT_NO_THROW(srv->update().get());
    CPPUNIT_ASSERT_EQUAL(std::int64_t(123), target->get_value());

    CPPUNIT_ASSERT_NO_THROW(srv->stop().get());
    service::remove(srv);
}

//------------------------------------------------------------------------------

void concat_test::format()
{
    auto srv = service::add("sight::module::data::concat");
    using namespace std::literals::string_literals;

    constexpr auto scam_template_str =
        "Dear %1%,\n\n"
        "We have noticed your interest in the new XT 22000 light saber.\n"
        "Click here %2% to save %3% galactic credits!";

    auto scam_template = std::make_shared<sight::data::string>(scam_template_str);

    auto scam      = std::make_shared<sight::data::string>();
    auto pigeon    = std::make_shared<sight::data::string>();
    auto honey_jar = std::make_shared<sight::data::string>();
    auto money     = std::make_shared<sight::data::integer>();

    srv->set_inout(scam_template, "format");
    srv->set_input(pigeon, "fragments", true, false, 0);
    srv->set_input(honey_jar, "fragments", true, false, 1);
    srv->set_input(money, "fragments", true, false, 2);
    srv->set_inout(scam, "target");

    CPPUNIT_ASSERT_NO_THROW(srv->configure());
    CPPUNIT_ASSERT_NO_THROW(srv->start().get());

    // Test that it concats at update if the key is set
    pigeon->set_value("Obi Wan Kenobi");
    honey_jar->set_value("<a href=\"https://www.sithsareus.com/pigeon\">The Old Jedi Hideout</a>");
    money->set_value(10000);

    CPPUNIT_ASSERT_NO_THROW(srv->update().get());

    const std::string expected =
        "Dear Obi Wan Kenobi,\n\n"
        "We have noticed your interest in the new XT 22000 light saber.\n"
        "Click here <a href=\"https://www.sithsareus.com/pigeon\">The Old Jedi Hideout</a> to save 10000 galactic credits!";

    CPPUNIT_ASSERT_EQUAL(expected, scam->get_value());

    CPPUNIT_ASSERT_NO_THROW(srv->stop().get());
    service::remove(srv);
}

} // namespace sight::module::data::ut
