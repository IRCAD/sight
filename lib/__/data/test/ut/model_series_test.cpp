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

#include <data/model_series.hpp>

#include <utest_data/generator/mesh.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::model_series")
{
//------------------------------------------------------------------------------

    TEST_CASE("model")
    {
        auto series = std::make_shared<sight::data::model_series>();
        CHECK(series);

        sight::data::reconstruction::sptr rec1 = std::make_shared<sight::data::reconstruction>();
        CHECK(rec1);
        sight::data::reconstruction::sptr rec2 = std::make_shared<sight::data::reconstruction>();
        CHECK(rec2);

        sight::data::mesh::sptr mesh1 = std::make_shared<sight::data::mesh>();
        CHECK(mesh1);
        sight::utest_data::generator::mesh::generate_quad_mesh(mesh1);

        sight::data::mesh::sptr mesh2 = std::make_shared<sight::data::mesh>();
        CHECK(mesh2);
        sight::utest_data::generator::mesh::generate_quad_mesh(mesh2);

        rec1->set_mesh(mesh1);
        rec2->set_mesh(mesh2);

        sight::data::model_series::reconstruction_vector_t recs;
        recs.push_back(rec1);
        recs.push_back(rec2);

        series->set_reconstruction_db(recs);
        CHECK_EQ(2, (int) series->get_reconstruction_db().size());
        CHECK_EQ(rec1, series->get_reconstruction_db()[0]);
        CHECK_EQ(rec2, series->get_reconstruction_db()[1]);
    }

//------------------------------------------------------------------------------

    TEST_CASE("deep_copy")
    {
        auto series = std::make_shared<sight::data::model_series>();
        CHECK(series);

        auto rec1  = std::make_shared<sight::data::reconstruction>();
        auto mesh1 = std::make_shared<sight::data::mesh>();
        sight::utest_data::generator::mesh::generate_quad_mesh(mesh1);

        rec1->set_mesh(mesh1);
        sight::data::model_series::reconstruction_vector_t recs;
        recs.push_back(rec1);
        series->set_reconstruction_db(recs);

        auto second_series = std::make_shared<sight::data::model_series>();

        CHECK(*series != *second_series);

        second_series->deep_copy(series);

        CHECK(*series == *second_series);

        CHECK_EQ(1, (int) series->get_reconstruction_db().size());
        CHECK_EQ(1, (int) second_series->get_reconstruction_db().size());
        CHECK(series->get_reconstruction_db()[0] != second_series->get_reconstruction_db()[0]);
    }

//------------------------------------------------------------------------------

    TEST_CASE("shallow_copy")
    {
        auto series = std::make_shared<sight::data::model_series>();
        CHECK(series);

        auto rec1  = std::make_shared<sight::data::reconstruction>();
        auto mesh1 = std::make_shared<sight::data::mesh>();
        sight::utest_data::generator::mesh::generate_quad_mesh(mesh1);
        rec1->set_mesh(mesh1);
        sight::data::model_series::reconstruction_vector_t recs;
        recs.push_back(rec1);
        series->set_reconstruction_db(recs);

        auto second_series = std::make_shared<sight::data::model_series>();

        CHECK(*series != *second_series);

        second_series->shallow_copy(series);

        CHECK(*series == *second_series);

        CHECK(series->get_reconstruction_db()[0] == second_series->get_reconstruction_db()[0]);
        CHECK_EQ(series->get_reconstruction_db()[0], second_series->get_reconstruction_db()[0]);

        CHECK_EQ(recs[0], series->get_reconstruction_db()[0]);
        CHECK_EQ(recs[0], second_series->get_reconstruction_db()[0]);
        CHECK_EQ(1, (int) series->get_reconstruction_db().size());
        CHECK_EQ(1, (int) second_series->get_reconstruction_db().size());
    }

//------------------------------------------------------------------------------

    TEST_CASE("equality")
    {
        auto series1 = std::make_shared<sight::data::model_series>();
        auto series2 = std::make_shared<sight::data::model_series>();

        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
            series1->op; \
            CHECK_MESSAGE( \
                *series1 != *series2, \
                "Series must be different when using " #op " on the first one" \
            ); \
            CHECK_MESSAGE( \
                !(*series1 == *series2), \
                "Series must be different when using " #op " on the first one" \
            ); \
            series2->op; \
            CHECK_MESSAGE( \
                *series1 == *series2, \
                "Series must be equal when using " #op " on both" \
            ); \
            CHECK_MESSAGE( \
                !(*series1 != *series2), \
                "Series must be equal when using " #op " on both" \
            );

        TEST(set_reconstruction_db({std::make_shared<sight::data::reconstruction>()}));

    #undef TEST
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::data::model_series")
