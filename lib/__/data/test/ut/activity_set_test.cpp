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

#include <core/tools/uuid.hpp>

#include <data/activity_set.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::activity_set")
{
//------------------------------------------------------------------------------

    TEST_CASE("nominal")
    {
        static const auto s_CONFIG_ID1 = sight::core::tools::uuid::generate();
        static const auto s_CONFIG_ID2 = sight::core::tools::uuid::generate();
        static const auto s_CONFIG_ID3 = sight::core::tools::uuid::generate();

        // Create three activities
        const auto& activity1 = std::make_shared<sight::data::activity>();
        activity1->set_activity_config_id(s_CONFIG_ID1);

        const auto& activity2 = std::make_shared<sight::data::activity>();
        activity2->set_activity_config_id(s_CONFIG_ID2);

        const auto& activity3 = std::make_shared<sight::data::activity>();
        activity3->set_activity_config_id(s_CONFIG_ID3);

        // Create an activity set and add the three activities
        auto act_set = std::make_shared<sight::data::activity_set>();
        CHECK(act_set);

        act_set->push_back(activity1);
        act_set->push_back(activity2);
        act_set->push_back(activity3);

        // Check the size
        CHECK_EQ(std::size_t(3), act_set->size());

        // Check that the activity set contains the three activities
        // get the set API from the activity set, so we can use find()
        const auto& set = act_set->get<1>();
        CHECK(set.find(activity1) != set.cend());
        CHECK(set.find(activity2) != set.cend());
        CHECK(set.find(activity3) != set.cend());

        // ActivitySet is sequenced, like a list
        auto it = act_set->cbegin();
        CHECK(*it == activity1);
        CHECK(*(++it) == activity2);
        CHECK(*(++it) == activity3);

        // It also provide a random access [] operator
        CHECK((*act_set)[0] == activity1);
        CHECK((*act_set)[1] == activity2);
        CHECK((*act_set)[2] == activity3);

        // Like in a std::set, activities stored in an ActivitySet are unique
        act_set->push_back(activity1);
        CHECK(act_set->size() == 3);

        // Inserting an already present activity should not change the size, and should return false
        CHECK(act_set->insert(act_set->end(), activity2).second == false);
        CHECK(act_set->size() == 3);

        // Inserting a new activity should change the size, and should return true
        CHECK(act_set->insert(act_set->end(), std::make_shared<sight::data::activity>()).second == true);
        CHECK(act_set->size() == 4);
    }

//------------------------------------------------------------------------------

    TEST_CASE("copy")
    {
        static const auto s_CONFIG_ID1 = sight::core::tools::uuid::generate();
        static const auto s_CONFIG_ID2 = sight::core::tools::uuid::generate();
        static const auto s_CONFIG_ID3 = sight::core::tools::uuid::generate();

        // Create three activities
        const auto& activity1 = std::make_shared<sight::data::activity>();
        activity1->set_activity_config_id(s_CONFIG_ID1);

        const auto& activity2 = std::make_shared<sight::data::activity>();
        activity2->set_activity_config_id(s_CONFIG_ID2);

        const auto& activity3 = std::make_shared<sight::data::activity>();
        activity3->set_activity_config_id(s_CONFIG_ID3);

        // Create an activity set and add the three activities
        const auto& original_act_set = std::make_shared<sight::data::activity_set>();
        original_act_set->push_back(activity1);
        original_act_set->push_back(activity2);
        original_act_set->push_back(activity3);

        // Create another activity and shallow copy the original activity
        auto shallow_act_set = std::make_shared<sight::data::activity_set>();
        shallow_act_set->shallow_copy(original_act_set);

        CHECK(shallow_act_set->size() == 3);
        CHECK((*shallow_act_set)[0] == activity1);
        CHECK((*shallow_act_set)[1] == activity2);
        CHECK((*shallow_act_set)[2] == activity3);

        auto deep_act_set = std::make_shared<sight::data::activity_set>();
        deep_act_set->deep_copy(original_act_set);

        CHECK(deep_act_set->size() == 3);

        // Pointers should be different...
        CHECK((*deep_act_set)[0] != activity1);
        CHECK((*deep_act_set)[1] != activity2);
        CHECK((*deep_act_set)[2] != activity3);

        // ...But pointed value not !
        CHECK(is_equal((*deep_act_set)[0], activity1));
        CHECK(is_equal((*deep_act_set)[1], activity2));
        CHECK(is_equal((*deep_act_set)[2], activity3));
    }
} // TEST_SUITE("sight::data::activity_set")
