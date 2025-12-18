/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <core/id.hpp>
#include <core/object.hpp>
#include <core/tools/failed.hpp>
#include <core/tools/uuid.hpp>

#include <doctest/doctest.h>

#include <chrono>
#include <future>
#include <thread>

TEST_SUITE("sight::core::tools::id")
{
//-----------------------------------------------------------------------------

    static void run_id_creation()
    {
        const std::string id = sight::core::tools::uuid::generate();

        auto obj = std::make_shared<sight::core::object>();

        CHECK(sight::core::id::exist(id) == false);

        obj->set_id(id);

        CHECK(sight::core::id::exist(id));

        CHECK_EQ(id, obj->get_id());

        CHECK_EQ(obj, sight::core::id::get_object(id));

        auto obj2 = std::make_shared<sight::core::object>();

        CHECK(obj2->has_id() == false);
        CHECK_THROWS_AS(obj2->get_id(sight::core::id::policy::must_exist), sight::core::tools::failed);

        const std::string id2 = obj2->get_id(sight::core::id::policy::generate);
        CHECK_NOTHROW(obj2->get_id(sight::core::id::policy::must_exist));

        CHECK(obj2->has_id() == true);
        CHECK(sight::core::id::exist(id2));

        obj2->reset_id();
        CHECK(obj2->has_id() == false);

        CHECK(sight::core::id::exist(id2) == false);
        CHECK(!sight::core::id::get_object(id2));
    }

//------------------------------------------------------------------P-----------

    TEST_CASE("object_id")
    {
        const std::string id = "myID";

        auto obj = std::make_shared<sight::core::object>();

        CHECK(sight::core::id::exist(id) == false);

        obj->set_id(id);

        CHECK(sight::core::id::exist(id));
        CHECK_EQ(id, obj->get_id());
        CHECK_EQ(obj, sight::core::id::get_object(id));

        sight::core::object::sptr obj2 = std::make_shared<sight::core::object>();

        CHECK(obj2->has_id() == false);
        CHECK_THROWS_AS(obj2->get_id(sight::core::id::policy::must_exist), sight::core::tools::failed);

        std::string gen_id = obj2->get_id(sight::core::id::policy::generate);
        CHECK_NOTHROW(obj2->get_id(sight::core::id::policy::must_exist));

        CHECK(obj2->has_id() == true);
        CHECK(sight::core::id::exist(gen_id));

        obj2->reset_id();
        CHECK(obj2->has_id() == false);

        CHECK(sight::core::id::exist(gen_id) == false);
        CHECK(!sight::core::id::get_object(gen_id));
    }

//-----------------------------------------------------------------------------

    TEST_CASE("concurrent_access_on_id_map")
    {
        std::vector<std::future<void> > futures;
        futures.reserve(10);
        for(unsigned int i = 0 ; i < 10 ; ++i)
        {
            futures.push_back(std::async(std::launch::async, run_id_creation));
        }

        for(auto& future : futures)
        {
            const auto status = future.wait_for(std::chrono::seconds(1));
            CHECK(status == std::future_status::ready);
            future.get(); // Trigger exceptions
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("join_id")
    {
        // Build an id with a specific format
        const std::string config_id  = "my_object_id";
        const std::string service_id = "my_service_id";
        const auto real_id           = sight::core::string::join('-', config_id, 6, 6, 6, service_id);

        CHECK_EQ(false, sight::core::id::exist(real_id));
        CHECK_EQ(config_id + std::string("-6-6-6-") + service_id, real_id);

        auto obj = std::make_shared<sight::core::object>();
        obj->set_id("", std::string(""), config_id, "", std::string(""), 6, 6, 6, service_id, "", std::string(""));
        CHECK_EQ(true, sight::core::id::exist(real_id));
    }

//------------------------------------------------------------------------------

    TEST_CASE("short_id")
    {
        // Build an id with a specific format
        const std::string config_id  = "my_object_id";
        const std::string service_id = "my_service_id";

        auto obj = std::make_shared<sight::core::object>();
        obj->set_id("", std::string(""), config_id, "", std::string(""), 6, 6, 6, service_id, "", std::string(""));
        CHECK_EQ(service_id, obj->base_id());
    }
} // TEST_SUITE
