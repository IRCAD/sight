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

#include <core/object.hpp>
#include <core/tools/uuid.hpp>

#include <doctest/doctest.h>

#include <functional>
#include <future>

TEST_SUITE("sight::core::tools::uuid")
{
// Global variables to replace class members
    static sight::core::object::sptr g_object;
    static std::string g_uuid;

//-----------------------------------------------------------------------------

    static void run_uuidcreation()
    {
        std::string uuid = sight::core::tools::uuid::generate();

        auto obj = std::make_shared<sight::core::object>();

        CHECK(!sight::core::object::from_uuid(uuid));

        obj->set_uuid(uuid);

        CHECK_EQ(uuid, obj->get_uuid());
        CHECK_EQ(obj, sight::core::object::from_uuid(uuid));

        auto obj2          = std::make_shared<sight::core::object>();
        std::string uui_d2 = obj2->get_uuid();
        CHECK_EQ(obj2, sight::core::object::from_uuid(uui_d2));
    }

//-----------------------------------------------------------------------------

    static void run_access_to_object_uuid()
    {
        g_object->set_uuid(g_uuid);

        CHECK(sight::core::object::from_uuid(g_uuid));
        CHECK_EQ(g_uuid, g_object->get_uuid());
        CHECK_EQ(g_object, sight::core::object::from_uuid(g_uuid));
    }

//-----------------------------------------------------------------------------

    TEST_CASE("object_uuidtest")
    {
        const std::string uuid = "myUUID";

        sight::core::object::sptr obj = std::make_shared<sight::core::object>();

        CHECK(!sight::core::object::from_uuid(uuid));

        obj->set_uuid(uuid);

        CHECK(sight::core::object::from_uuid(uuid));
        CHECK_EQ(uuid, obj->get_uuid());
        CHECK_EQ(obj, sight::core::object::from_uuid(uuid));

        auto obj2          = std::make_shared<sight::core::object>();
        std::string uui_d2 = obj2->get_uuid();
        CHECK_EQ(obj2, sight::core::object::from_uuid(uui_d2));
    }

//-----------------------------------------------------------------------------

    TEST_CASE("conccurent_access_on_uuid_map")
    {
        std::vector<std::future<void> > futures;
        futures.reserve(10);
        for(unsigned int i = 0 ; i < 10 ; ++i)
        {
            futures.push_back(std::async(std::launch::async, run_uuidcreation));
        }

        for(auto& future : futures)
        {
            const auto status = future.wait_for(std::chrono::seconds(1));
            CHECK(status == std::future_status::ready);
            future.get(); // Trigger exceptions
        }
    }

//-----------------------------------------------------------------------------

    TEST_CASE("conccurent_access_on_same_obj_uuidtest")
    {
        // Set up context before running a test.
        g_object = std::make_shared<sight::core::object>();
        g_uuid   = sight::core::tools::uuid::generate();

        const auto fn = [](auto&& ...){run_access_to_object_uuid();};
        std::vector<std::future<void> > futures;
        futures.reserve(10);
        for(unsigned int i = 0 ; i < 10 ; ++i)
        {
            futures.push_back(std::async(std::launch::async, fn));
        }

        for(auto& future : futures)
        {
            const auto status = future.wait_for(std::chrono::seconds(1));
            CHECK(status == std::future_status::ready);
            future.get(); // Trigger exceptions
        }
    }
} // TEST_SUITE
