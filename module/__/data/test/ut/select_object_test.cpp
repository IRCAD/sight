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

#include <core/com/slot.hpp>
#include <core/com/slot.hxx>

#include <data/string.hpp>

#include <service/op.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::module::data::select_object")
{
//------------------------------------------------------------------------------

    TEST_CASE("basic")
    {
        auto select_object = sight::service::add("sight::module::data::select_object");
        auto object1       = std::make_shared<sight::data::string>("Hello");
        auto object2       = std::make_shared<sight::data::string>("world");

        CHECK_NOTHROW(select_object->start().get());
        select_object->slot("add")->run(std::dynamic_pointer_cast<sight::data::object>(object1));
        CHECK(select_object->output<sight::data::string>("object").lock() == object1);
        select_object->slot("remove")->run();
        CHECK(select_object->output<sight::data::string>("object").lock() == nullptr);
        select_object->slot("add")->run(std::dynamic_pointer_cast<sight::data::object>(object2));
        CHECK(select_object->output<sight::data::string>("object").lock() == object2);

        CHECK_NOTHROW(select_object->stop().get());
        sight::service::remove(select_object);
    }
} // TEST_SUITE("sight::module::data::select_object")
