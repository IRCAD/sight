/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <data/activity.hpp>
#include <data/integer.hpp>
#include <data/string.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::activity")
{
//------------------------------------------------------------------------------

    TEST_CASE("activity_config_id")
    {
        const auto activity                                         = std::make_shared<sight::data::activity>();
        const sight::data::activity::config_id_t activity_config_id = "Visu2D";
        CHECK(activity);
        activity->set_activity_config_id(activity_config_id);
        CHECK_EQ(activity_config_id, activity->get_activity_config_id());

        auto activity2 = std::make_shared<sight::data::activity>();
        CHECK(*activity2 != *activity);

        std::ranges::copy(*activity, std::inserter(*activity2, activity2->begin()));
        activity2->set_activity_config_id(activity->get_activity_config_id());
        CHECK(*activity2 == *activity);
    }

//------------------------------------------------------------------------------

    TEST_CASE("data")
    {
        const auto activity = std::make_shared<sight::data::activity>();
        auto activity2      = std::make_shared<sight::data::activity>();
        CHECK(activity);
        CHECK(activity2);

        // Both data are "empty"
        CHECK(*activity2 == *activity);

        std::ranges::copy(*activity, std::inserter(*activity2, activity2->begin()));
        activity2->set_activity_config_id(activity->get_activity_config_id());
        CHECK(*activity2 == *activity);
    }

//------------------------------------------------------------------------------

    TEST_CASE("equality")
    {
        const auto activity = std::make_shared<sight::data::activity>();
        auto activity1      = std::make_shared<sight::data::activity>();
        auto activity2      = std::make_shared<sight::data::activity>();

        CHECK(*activity1 == *activity2);
        CHECK(!(*activity1 != *activity2));

        activity1->set_activity_config_id("1");
        CHECK(*activity1 != *activity2);
        CHECK(!(*activity1 == *activity2));
        activity2->set_activity_config_id(activity1->get_activity_config_id());
        CHECK(*activity1 == *activity2);
        CHECK(!(*activity1 != *activity2));

        (*activity1)["data"] = std::make_shared<sight::data::integer>(2);
        CHECK(*activity1 != *activity2);
        CHECK(!(*activity1 == *activity2));
        std::ranges::copy(*activity1, std::inserter(*activity2, activity2->begin()));
        CHECK(*activity1 == *activity2);
        CHECK(!(*activity1 != *activity2));
    }

//------------------------------------------------------------------------------

    TEST_CASE("shallow_copy")
    {
        const auto activity = std::make_shared<sight::data::activity>();
        CHECK(activity);
        activity->set_activity_config_id("MyActivity");
        (*activity)["data"] = std::make_shared<sight::data::string>("Hello world");

        auto activity2 = std::make_shared<sight::data::activity>();
        CHECK(*activity != *activity2);
        activity2->shallow_copy(activity);
        CHECK(*activity == *activity2);
    }
} // TEST_SUITE("sight::data::activity")
