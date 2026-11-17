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

// cspell:ignore Arent

#include <activity/validator/base.hpp>

#include <data/series.hpp>
#include <data/vector.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::activity::validator::related_study")
{
//------------------------------------------------------------------------------

    TEST_CASE("studies_are_related")
    {
        auto validator = sight::activity::validator::factory::make("sight::activity::validator::related_study");
        CHECK(validator);

        auto series1 = std::make_shared<sight::data::series>();
        series1->set_study_instance_uid("These studies look related");
        auto series2 = std::make_shared<sight::data::series>();
        series2->set_study_instance_uid("These studies look related");

        auto vector = std::make_shared<sight::data::vector>();
        vector->push_back(series1);
        vector->push_back(series2);

        CHECK(validator->validate(sight::activity::extension::activity_info {}, vector).first);
    }

//------------------------------------------------------------------------------

    TEST_CASE("studies_arent_related")
    {
        auto validator = sight::activity::validator::factory::make("sight::activity::validator::related_study");
        CHECK(validator);

        auto series1 = std::make_shared<sight::data::series>();
        series1->set_study_instance_uid("My study is better than yours");
        auto series2 = std::make_shared<sight::data::series>();
        series2->set_study_instance_uid("Well, in any case our studies aren't related");

        auto vector = std::make_shared<sight::data::vector>();
        vector->push_back(series1);
        vector->push_back(series2);

        CHECK(!validator->validate(sight::activity::extension::activity_info {}, vector).first);
    }
}
