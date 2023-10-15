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

// cspell:ignore Arent

#include "related_study_test.hpp"

#include <activity/validator/base.hpp>

#include <data/series.hpp>
#include <data/vector.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::activity::validator::ut::related_study_test);

namespace sight::activity::validator::ut
{

//------------------------------------------------------------------------------

void related_study_test::studiesAreRelatedTest()
{
    activity::validator::base::sptr validator =
        activity::validator::factory::make("sight::activity::validator::related_study");
    CPPUNIT_ASSERT(validator);

    auto series1 = std::make_shared<data::series>();
    series1->setStudyInstanceUID("These studies look related");
    auto series2 = std::make_shared<data::series>();
    series2->setStudyInstanceUID("These studies look related");

    auto vector = std::make_shared<data::vector>();
    vector->push_back(series1);
    vector->push_back(series2);

    CPPUNIT_ASSERT(validator->validate(activity::extension::activity_info {}, vector).first);
}

//------------------------------------------------------------------------------

void related_study_test::studiesArentRelatedTest()
{
    activity::validator::base::sptr validator =
        activity::validator::factory::make("sight::activity::validator::related_study");
    CPPUNIT_ASSERT(validator);

    auto series1 = std::make_shared<data::series>();
    series1->setStudyInstanceUID("My study is better than yours");
    auto series2 = std::make_shared<data::series>();
    series2->setStudyInstanceUID("Well, in any case our studies aren't related");

    auto vector = std::make_shared<data::vector>();
    vector->push_back(series1);
    vector->push_back(series2);

    CPPUNIT_ASSERT(!validator->validate(activity::extension::activity_info {}, vector).first);
}

} // namespace sight::activity::validator::ut
