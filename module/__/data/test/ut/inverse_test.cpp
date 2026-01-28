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

#include "data/integer.hpp"

#include <data/boolean.hpp>
#include <data/string.hpp>

#include <service/op.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::module::data::inverse")
{
//------------------------------------------------------------------------------

    TEST_CASE("inverse_true_to_false")
    {
        auto srv = sight::service::add("sight::module::data::inverse");
        using namespace std::literals::string_literals;

        auto source = std::make_shared<sight::data::boolean>(true);
        srv->set_input(source, "source");
        auto target = std::make_shared<sight::data::boolean>(true);
        srv->set_inout(target, "target");

        CHECK_EQ(true, target->value());

        CHECK_NOTHROW(srv->configure());
        CHECK_NOTHROW(srv->start().get());

        CHECK_NOTHROW(srv->update().get());
        CHECK_EQ(false, target->value());

        CHECK_NOTHROW(srv->stop().get());
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE("inverse_false_to_true")
    {
        auto srv = sight::service::add("sight::module::data::inverse");
        using namespace std::literals::string_literals;

        auto source = std::make_shared<sight::data::boolean>(false);
        srv->set_input(source, "source");
        auto target = std::make_shared<sight::data::boolean>(false);
        srv->set_inout(target, "target");

        CHECK_EQ(false, target->value());

        CHECK_NOTHROW(srv->configure());
        CHECK_NOTHROW(srv->start().get());

        CHECK_NOTHROW(srv->update().get());
        CHECK_EQ(true, target->value());

        CHECK_NOTHROW(srv->stop().get());
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE("inverse_with_string")
    {
        auto srv = sight::service::add("sight::module::data::inverse");
        using namespace std::literals::string_literals;

        auto source = std::make_shared<sight::data::string>("true");
        srv->set_input(source, "source");

        auto target = std::make_shared<sight::data::boolean>();
        srv->set_inout(target, "target");

        CHECK_NOTHROW(srv->configure());
        CHECK_NOTHROW(srv->start().get());

        CHECK_NOTHROW(srv->update().get());
        CHECK_EQ("false"s, target->to_string());

        source = std::make_shared<sight::data::string>("false");
        srv->set_input(source, "source");

        CHECK_NOTHROW(srv->update().get());
        CHECK_EQ("true"s, target->to_string());

        CHECK_NOTHROW(srv->stop().get());
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE("inverse_with_int")
    {
        auto srv = sight::service::add("sight::module::data::inverse");
        using namespace std::literals::string_literals;

        auto source = std::make_shared<sight::data::integer>(1);
        srv->set_input(source, "source");

        auto target = std::make_shared<sight::data::boolean>();
        srv->set_inout(target, "target");

        CHECK_NOTHROW(srv->configure());
        CHECK_NOTHROW(srv->start().get());

        CHECK_NOTHROW(srv->update().get());
        CHECK_EQ(false, target->value());

        source = std::make_shared<sight::data::integer>(0);
        srv->set_input(source, "source");

        CHECK_NOTHROW(srv->update().get());
        CHECK_EQ(true, target->value());

        CHECK_NOTHROW(srv->stop().get());
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE("inverse_invalid_boolean")
    {
        auto srv = sight::service::add("sight::module::data::inverse");
        using namespace std::literals::string_literals;

        auto source = std::make_shared<sight::data::string>("not_a_boolean");
        srv->set_input(source, "source");
        auto target = std::make_shared<sight::data::string>();
        srv->set_inout(target, "target");

        CHECK_NOTHROW(srv->configure());
        CHECK_NOTHROW(srv->start().get());

        CHECK_THROWS(srv->update().get());

        CHECK_NOTHROW(srv->stop().get());
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::module::data::inverse")
