/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "activity_set_test.hpp"

#include <core/tools/uuid.hpp>

#include <data/activity_set.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::activity_set_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void activity_set_test::setUp()
{
}

//------------------------------------------------------------------------------

void activity_set_test::tearDown()
{
}

//------------------------------------------------------------------------------

void activity_set_test::nominalTest()
{
    static const auto configId1 = core::tools::UUID::generate();
    static const auto configId2 = core::tools::UUID::generate();
    static const auto configId3 = core::tools::UUID::generate();

    // Create three activities
    const auto& activity1 = std::make_shared<activity>();
    activity1->setActivityConfigId(configId1);

    const auto& activity2 = std::make_shared<activity>();
    activity2->setActivityConfigId(configId2);

    const auto& activity3 = std::make_shared<activity>();
    activity3->setActivityConfigId(configId3);

    // Create an activity set and add the three activities
    auto act_set = std::make_shared<activity_set>();
    CPPUNIT_ASSERT(act_set);

    act_set->push_back(activity1);
    act_set->push_back(activity2);
    act_set->push_back(activity3);

    // Check the size
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), act_set->size());

    // Check that the activity set contains the three activities
    // get the set API from the activity set, so we can use find()
    const auto& set = act_set->get<1>();
    CPPUNIT_ASSERT(set.find(activity1) != set.cend());
    CPPUNIT_ASSERT(set.find(activity2) != set.cend());
    CPPUNIT_ASSERT(set.find(activity3) != set.cend());

    // ActivitySet is sequenced, like a list
    auto it = act_set->cbegin();
    CPPUNIT_ASSERT(*it == activity1);
    CPPUNIT_ASSERT(*(++it) == activity2);
    CPPUNIT_ASSERT(*(++it) == activity3);

    // It also provide a random access [] operator
    CPPUNIT_ASSERT((*act_set)[0] == activity1);
    CPPUNIT_ASSERT((*act_set)[1] == activity2);
    CPPUNIT_ASSERT((*act_set)[2] == activity3);

    // Like in a std::set, activities stored in an ActivitySet are unique
    act_set->push_back(activity1);
    CPPUNIT_ASSERT(act_set->size() == 3);

    // Inserting an already present activity should not change the size, and should return false
    CPPUNIT_ASSERT(act_set->insert(act_set->end(), activity2).second == false);
    CPPUNIT_ASSERT(act_set->size() == 3);

    // Inserting a new activity should change the size, and should return true
    CPPUNIT_ASSERT(act_set->insert(act_set->end(), std::make_shared<activity>()).second == true);
    CPPUNIT_ASSERT(act_set->size() == 4);
}

//------------------------------------------------------------------------------

void activity_set_test::copyTest()
{
    static const auto configId1 = core::tools::UUID::generate();
    static const auto configId2 = core::tools::UUID::generate();
    static const auto configId3 = core::tools::UUID::generate();

    // Create three activities
    const auto& activity1 = std::make_shared<activity>();
    activity1->setActivityConfigId(configId1);

    const auto& activity2 = std::make_shared<activity>();
    activity2->setActivityConfigId(configId2);

    const auto& activity3 = std::make_shared<activity>();
    activity3->setActivityConfigId(configId3);

    // Create an activity set and add the three activities
    const auto& original_act_set = std::make_shared<activity_set>();
    original_act_set->push_back(activity1);
    original_act_set->push_back(activity2);
    original_act_set->push_back(activity3);

    // Create another activity and shallow copy the original activity
    auto shallow_act_set = std::make_shared<activity_set>();
    shallow_act_set->shallow_copy(original_act_set);

    CPPUNIT_ASSERT(shallow_act_set->size() == 3);
    CPPUNIT_ASSERT((*shallow_act_set)[0] == activity1);
    CPPUNIT_ASSERT((*shallow_act_set)[1] == activity2);
    CPPUNIT_ASSERT((*shallow_act_set)[2] == activity3);

    auto deep_act_set = std::make_shared<activity_set>();
    deep_act_set->deep_copy(original_act_set);

    CPPUNIT_ASSERT(deep_act_set->size() == 3);

    // Pointers should be different...
    CPPUNIT_ASSERT((*deep_act_set)[0] != activity1);
    CPPUNIT_ASSERT((*deep_act_set)[1] != activity2);
    CPPUNIT_ASSERT((*deep_act_set)[2] != activity3);

    // ...But pointed value not !
    CPPUNIT_ASSERT(is_equal((*deep_act_set)[0], activity1));
    CPPUNIT_ASSERT(is_equal((*deep_act_set)[1], activity2));
    CPPUNIT_ASSERT(is_equal((*deep_act_set)[2], activity3));
}

} // namespace sight::data::ut
