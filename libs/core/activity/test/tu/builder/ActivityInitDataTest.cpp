/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "ActivityInitDataTest.hpp"

#include <activity/builder/ActivityInitData.hpp>
#include <activity/extension/Activity.hpp>

#include <data/Activity.hpp>
#include <data/String.hpp>
#include <data/Vector.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::activity::builder::ut::ActivityInitDataTest);

namespace sight::activity::builder::ut
{

//------------------------------------------------------------------------------

void ActivityInitDataTest::basicTest()
{
    activity::extension::ActivityInfo activityInfo;

    activity::extension::ActivityRequirement requirement;
    requirement.name      = "shouldBePresent";
    requirement.type      = "sight::data::String";
    requirement.minOccurs = 0;
    requirement.maxOccurs = 0;
    activityInfo.requirements.push_back(requirement);

    requirement.name      = "shouldntBeAString";
    requirement.maxOccurs = 1;
    activity::extension::ActivityRequirementKey reqKey;
    reqKey.key       = "key";
    requirement.keys = {reqKey};
    activityInfo.requirements.push_back(requirement);

    auto selection                = data::Vector::New();
    data::Activity::sptr activity = builder::ActivityInitData::New()->buildData(activityInfo, selection);
    CPPUNIT_ASSERT(activity->get<data::String>("shouldBePresent") != nullptr);
    CPPUNIT_ASSERT(activity->get<data::String>("shouldntBeAString") == nullptr);
}

} //namespace sight::activity::builder::ut
