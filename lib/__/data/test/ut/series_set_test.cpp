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

#include <data/series_set.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::series_set")
{
//------------------------------------------------------------------------------

    TEST_CASE("nominal")
    {
        static const auto s_DESCRIPTION1 = sight::core::tools::uuid::generate();
        static const auto s_DESCRIPTION2 = sight::core::tools::uuid::generate();
        static const auto s_DESCRIPTION3 = sight::core::tools::uuid::generate();

        // Create three series
        const auto& series1 = std::make_shared<sight::data::series>();
        series1->set_series_description(s_DESCRIPTION1);

        const auto& series2 = std::make_shared<sight::data::series>();
        series2->set_series_description(s_DESCRIPTION2);

        const auto& series3 = std::make_shared<sight::data::series>();
        series3->set_series_description(s_DESCRIPTION3);

        // Create an series set and add the three series
        auto series_set1 = std::make_shared<sight::data::series_set>();
        CHECK(series_set1);

        series_set1->push_back(series1);
        series_set1->push_back(series2);
        series_set1->push_back(series3);

        // Check the size
        CHECK_EQ(std::size_t(3), series_set1->size());

        // Check that the series set contains the three activities
        // get the set API from the series set, so we can use find()
        const auto& set = series_set1->get<1>();
        CHECK(set.find(series1) != set.cend());
        CHECK(set.find(series2) != set.cend());
        CHECK(set.find(series3) != set.cend());

        // series_set is sequenced, like a list
        auto it = series_set1->cbegin();
        CHECK(*it == series1);
        CHECK(*(++it) == series2);
        CHECK(*(++it) == series3);

        // It also provide a random access [] operator
        CHECK((*series_set1)[0] == series1);
        CHECK((*series_set1)[1] == series2);
        CHECK((*series_set1)[2] == series3);

        // Like in a std::set, activities stored in an series_set are unique
        series_set1->push_back(series1);
        CHECK(series_set1->size() == 3);

        // Inserting an already present series should not change the size, and should return false
        CHECK(series_set1->insert(series_set1->end(), series2).second == false);
        CHECK(series_set1->size() == 3);

        // Inserting a new series should change the size, and should return true
        CHECK(series_set1->insert(series_set1->end(), std::make_shared<sight::data::series>()).second == true);
        CHECK(series_set1->size() == 4);
    }

//------------------------------------------------------------------------------

    TEST_CASE("copy")
    {
        static const auto s_DESCRIPTION1 = sight::core::tools::uuid::generate();
        static const auto s_DESCRIPTION2 = sight::core::tools::uuid::generate();
        static const auto s_DESCRIPTION3 = sight::core::tools::uuid::generate();

        // Create three activities
        const auto& series1 = std::make_shared<sight::data::series>();
        series1->set_series_description(s_DESCRIPTION1);

        const auto& series2 = std::make_shared<sight::data::series>();
        series2->set_series_description(s_DESCRIPTION2);

        const auto& series3 = std::make_shared<sight::data::series>();
        series3->set_series_description(s_DESCRIPTION3);

        // Create an series set and add the three activities
        const auto& original_series_set = std::make_shared<sight::data::series_set>();
        original_series_set->push_back(series1);
        original_series_set->push_back(series2);
        original_series_set->push_back(series3);

        // Create another series and shallow copy the original series
        auto shallow_series_set = std::make_shared<sight::data::series_set>();
        shallow_series_set->shallow_copy(original_series_set);

        CHECK(shallow_series_set->size() == 3);
        CHECK((*shallow_series_set)[0] == series1);
        CHECK((*shallow_series_set)[1] == series2);
        CHECK((*shallow_series_set)[2] == series3);

        auto deep_series_set = std::make_shared<sight::data::series_set>();
        deep_series_set->deep_copy(original_series_set);

        CHECK(deep_series_set->size() == 3);

        // Pointers should be different...
        CHECK((*deep_series_set)[0] != series1);
        CHECK((*deep_series_set)[1] != series2);
        CHECK((*deep_series_set)[2] != series3);

        // ...But pointed value not !
        CHECK(is_equal((*deep_series_set)[0], series1));
        CHECK(is_equal((*deep_series_set)[1], series2));
        CHECK(is_equal((*deep_series_set)[2], series3));
    }
} // TEST_SUITE("sight::data::series_set")
