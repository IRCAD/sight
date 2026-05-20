/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <data/color.hpp>
#include <data/string.hpp>
#include <data/transfer_function.hpp>

#include <doctest/doctest.h>

#include <glm/common.hpp>
#include <glm/gtc/epsilon.hpp>

#include <algorithm>

static const double EPSILON = 1e-5;

// There might be some uncertainty when sampling, so we need to include an epsilon when testing equality
#define ASSERT_COLOR_EQUALS(c1, c2) \
        CHECK(glm::all(glm::epsilonEqual(c1, c2, EPSILON)));

//------------------------------------------------------------------------------

static sight::data::transfer_function::sptr create_tf_color()
{
    sight::data::transfer_function::sptr tf = std::make_shared<sight::data::transfer_function>();

    tf->set_background_color(sight::data::transfer_function::color_t(1.0, 0.3, 0.6, 0.1));
    tf->set_name("color_t");
    tf->set_window_min_max({-40.33, 150.});

    auto tf_data = tf->pieces().emplace_back(std::make_shared<sight::data::transfer_function_piece>());
    tf_data->set_clamped(false);
    tf_data->set_interpolation_mode(sight::data::transfer_function::interpolation_mode::nearest);
    tf_data->set_window_min_max({-40.33, 150.});

    tf_data->insert({-40.33, sight::data::transfer_function::color_t(0.9, 0.2, 0.3, 0.4)});
    tf_data->insert({3, sight::data::transfer_function::color_t(0.1, 0.2, 0.9, 0.4)}); // Invert point 3 <=> -0.2, for
    // tests
    tf_data->insert({-0.2, sight::data::transfer_function::color_t(0.1, 0.9, 0.3, 0.4)});
    tf_data->insert({150, sight::data::transfer_function::color_t(0.1, 0.2, 0.3, 0.9)});

    sight::data::string::sptr my_string = std::make_shared<sight::data::string>("fieldStringValue");
    tf->set_field("fieldStringKey", my_string);

    return tf;
}

//------------------------------------------------------------------------------

static void check_tf_color(sight::data::transfer_function::sptr _tf)
{
    CHECK(sight::data::transfer_function::color_t(1.0, 0.3, 0.6, 0.1) == _tf->background_color());
    CHECK_EQ(std::string("color_t"), _tf->name());

    CHECK_EQ(-200.02, _tf->window());
    CHECK_EQ(900.6, _tf->level());

    const auto tf_data = _tf->pieces().front();
    CHECK_EQ(false, tf_data->clamped());
    CHECK_EQ(sight::data::transfer_function::interpolation_mode::nearest, tf_data->get_interpolation_mode());

    CHECK_EQ(static_cast<std::size_t>(4), tf_data->size());
    CHECK_EQ(-40.33, tf_data->min_max().first);
    CHECK_EQ(150., tf_data->min_max().second);

    CHECK(sight::data::transfer_function::color_t(0.9, 0.2, 0.3, 0.4) == tf_data->find(-40.33)->second);
    CHECK(sight::data::transfer_function::color_t(0.1, 0.9, 0.3, 0.4) == tf_data->find(-0.2)->second);
    CHECK(sight::data::transfer_function::color_t(0.1, 0.2, 0.9, 0.4) == tf_data->find(3)->second);
    CHECK(sight::data::transfer_function::color_t(0.1, 0.2, 0.3, 0.9) == tf_data->find(150)->second);

    CHECK_EQ(
        std::string("fieldStringValue"),
        std::dynamic_pointer_cast<sight::data::string>(_tf->get_field("fieldStringKey"))->value()
    );
}

//------------------------------------------------------------------------------

TEST_SUITE("sight::data::transfer_function")
{
//------------------------------------------------------------------------------

    TEST_CASE("constructor")
    {
        sight::data::transfer_function::sptr tf = std::make_shared<sight::data::transfer_function>();

        CHECK_MESSAGE(0.0 == tf->level(), "Wrong level");
        CHECK_MESSAGE(2.0 == tf->window(), "Wrong window");

        CHECK_EQ(std::string(), tf->name());
        CHECK(sight::data::transfer_function::color_t() == tf->background_color());

        auto tf_data = tf->pieces().emplace_back(std::make_shared<sight::data::transfer_function_piece>());

        CHECK_EQ(sight::data::transfer_function::interpolation_mode::linear, tf_data->get_interpolation_mode());
        CHECK_EQ(true, tf_data->clamped());
        CHECK_EQ(std::size_t(0), tf_data->size());
    }

//------------------------------------------------------------------------------
    TEST_CASE("default_tf")
    {
        sight::data::transfer_function::csptr tf = sight::data::transfer_function::create_default_tf();

        CHECK_EQ(50.0, doctest::Approx(tf->level()).epsilon(EPSILON));
        CHECK_EQ(500.0, doctest::Approx(tf->window()).epsilon(EPSILON));

        CHECK_EQ(50.0, doctest::Approx(tf->pieces()[0]->level()).epsilon(EPSILON));
        CHECK_EQ(500.0, doctest::Approx(tf->pieces()[0]->window()).epsilon(EPSILON));

        CHECK_EQ(sight::data::transfer_function::DEFAULT_TF_NAME, tf->name());
        CHECK(sight::data::transfer_function::color_t() == tf->background_color());

        const auto first_piece = tf->pieces().front();

        CHECK_EQ(sight::data::transfer_function::interpolation_mode::linear, first_piece->get_interpolation_mode());
        CHECK_EQ(false, first_piece->clamped());
        CHECK_EQ(std::size_t(2), first_piece->size());
    }

//------------------------------------------------------------------------------

    TEST_CASE("classic_get_set")
    {
        sight::data::transfer_function::sptr tf = sight::data::transfer_function::create_default_tf();

        // Test getTFData()
        const auto tf_data                                 = tf->pieces().front();
        auto itr                                           = tf_data->cbegin();
        const sight::data::transfer_function::value_t key1 = itr->first;
        const sight::data::transfer_function::value_t key2 = (++itr)->first;

        auto itr_color                                       = tf_data->cbegin();
        const sight::data::transfer_function::color_t color1 = itr_color->second;
        const sight::data::transfer_function::color_t color2 = (++itr_color)->second;

        CHECK_EQ(0.0, doctest::Approx(key1).epsilon(1e-10));
        CHECK_EQ(1.0, doctest::Approx(key2).epsilon(1e-10));

        const sight::data::transfer_function::color_t expected_color1(0.0, 0.0, 0.0, 0.0);
        const sight::data::transfer_function::color_t expected_color2(1.0, 1.0, 1.0, 1.0);
        CHECK(expected_color1 == color1);
        CHECK(expected_color2 == color2);
        CHECK(expected_color1 == tf_data->find(key1)->second);

        // Test erase
        tf_data->erase(1.0);
        CHECK_EQ((std::size_t) 1, tf_data->size());

        // Test clear()
        tf_data->clear();
        const std::size_t expected_cleared_size = 0;
        CHECK_EQ(expected_cleared_size, tf_data->size());
    }

//------------------------------------------------------------------------------

    TEST_CASE("shallow_and_deep_copy")
    {
        const sight::data::transfer_function::sptr tf = create_tf_color();
        tf->set_level(900.6);
        tf->set_window(-200.02);

        check_tf_color(tf);

        const sight::data::transfer_function::sptr shallow_copy_tf = std::make_shared<sight::data::transfer_function>();
        shallow_copy_tf->shallow_copy(tf);
        check_tf_color(shallow_copy_tf);
        CHECK_EQ(tf->pieces()[0], shallow_copy_tf->pieces()[0]);

        const sight::data::transfer_function::sptr deep_copy_tf = sight::data::object::copy(tf);
        check_tf_color(deep_copy_tf);
        // Pointer addresses must be different
        CHECK(tf->pieces()[0] != deep_copy_tf->pieces()[0]);
    }

//------------------------------------------------------------------------------

    TEST_CASE("set_tf_data")
    {
        sight::data::transfer_function::sptr tf = create_tf_color();
        tf->set_level(900.6);
        tf->set_window(-200.02);

        auto piece = tf->pieces()[0];
        sight::data::transfer_function::data_t tf_data;
        tf_data[-40.33] = sight::data::transfer_function::color_t(0.9, 0.2, 0.3, 0.4);
        tf_data[3]      = sight::data::transfer_function::color_t(0.1, 0.2, 0.9, 0.4);
        tf_data[-0.2]   = sight::data::transfer_function::color_t(0.1, 0.9, 0.3, 0.4);
        tf_data[150]    = sight::data::transfer_function::color_t(0.1, 0.2, 0.3, 0.9);
        std::ranges::copy(tf_data, inserter(*piece));

        check_tf_color(tf);
    }

//------------------------------------------------------------------------------

    TEST_CASE("linear_color")
    {
        sight::data::transfer_function::sptr tf = create_tf_color();

        // Value = -40.33 => color : {0.9, 0.2, 0.3, 0.4}
        // Value = -0.2   => color : {0.1, 0.9, 0.3, 0.4}
        // Value = 3      => color : {0.1, 0.2, 0.9, 0.4}
        // Value = 150    => color : {0.1, 0.2, 0.3, 0.9}
        auto piece = tf->pieces()[0];

        piece->set_clamped(true);
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.0, .0, .0, .0), piece->sample_linear(-120));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.0, .0, .0, .0), piece->sample_linear(200));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.9, .2, .3, .4), piece->sample_linear(-40.33));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .2, .3, .9), piece->sample_linear(150));
        ASSERT_COLOR_EQUALS(
            sight::data::transfer_function::color_t(.1, .2, .6, .65),
            piece->sample_linear(((150. - 3.) / 2.) + 3)
        );
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .9, .3, .4), piece->sample_linear(-0.2));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .2, .9, .4), piece->sample_linear(3));
        ASSERT_COLOR_EQUALS(
            sight::data::transfer_function::color_t(.1, .2, .75, .525),
            piece->sample_linear(((150. - 3.) / 4.) + 3)
        );

        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.0, .0, .0, .0), tf->sample_linear(-120));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.0, .0, .0, .0), tf->sample_linear(200));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.9, .2, .3, .4), tf->sample_linear(-40.33));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .2, .3, .9), tf->sample_linear(150));
        ASSERT_COLOR_EQUALS(
            sight::data::transfer_function::color_t(.1, .2, .6, .65),
            tf->sample_linear(((150. - 3.) / 2.) + 3)
        );
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .9, .3, .4), tf->sample_linear(-0.2));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .2, .9, .4), tf->sample_linear(3));
        ASSERT_COLOR_EQUALS(
            sight::data::transfer_function::color_t(.1, .2, .75, .525),
            tf->sample_linear(((150. - 3.) / 4.) + 3)
        );

        piece->set_interpolation_mode(sight::data::transfer_function::interpolation_mode::linear);
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.0, .0, .0, .0), piece->sample(-120));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.0, .0, .0, .0), piece->sample(200));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.9, .2, .3, .4), piece->sample(-40.33));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .2, .3, .9), piece->sample(150));
        ASSERT_COLOR_EQUALS(
            sight::data::transfer_function::color_t(.1, .2, .6, .65),
            piece->sample(((150. - 3.) / 2.) + 3)
        );
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .9, .3, .4), piece->sample(-0.2));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .2, .9, .4), piece->sample(3));
        ASSERT_COLOR_EQUALS(
            sight::data::transfer_function::color_t(.1, .2, .75, .525),
            piece->sample(((150. - 3.) / 4.) + 3)
        );

        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.0, .0, .0, .0), tf->sample(-120));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.0, .0, .0, .0), tf->sample(200));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.9, .2, .3, .4), tf->sample(-40.33));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .2, .3, .9), tf->sample(150));
        ASSERT_COLOR_EQUALS(
            sight::data::transfer_function::color_t(.1, .2, .6, .65),
            tf->sample(((150. - 3.) / 2.) + 3)
        );
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .9, .3, .4), tf->sample(-0.2));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .2, .9, .4), tf->sample(3));
        ASSERT_COLOR_EQUALS(
            sight::data::transfer_function::color_t(.1, .2, .75, .525),
            tf->sample(((150. - 3.) / 4.) + 3)
        );

        piece->set_clamped(false);
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.9, .2, .3, .4), piece->sample_linear(-120));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .2, .3, .9), piece->sample_linear(200));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.9, .2, .3, .4), piece->sample_linear(-40.33));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .2, .3, .9), piece->sample_linear(150));
        ASSERT_COLOR_EQUALS(
            sight::data::transfer_function::color_t(.1, .2, .6, .65),
            piece->sample_linear(((150. - 3.) / 2.) + 3)
        );
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .9, .3, .4), piece->sample_linear(-0.2));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .2, .9, .4), piece->sample_linear(3));
        ASSERT_COLOR_EQUALS(
            sight::data::transfer_function::color_t(.1, .2, .75, .525),
            piece->sample_linear(((150. - 3.) / 4.) + 3)
        );

        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.9, .2, .3, .4), tf->sample_linear(-120));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .2, .3, .9), tf->sample_linear(200));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.9, .2, .3, .4), tf->sample_linear(-40.33));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .2, .3, .9), tf->sample_linear(150));
        ASSERT_COLOR_EQUALS(
            sight::data::transfer_function::color_t(.1, .2, .6, .65),
            tf->sample_linear(((150. - 3.) / 2.) + 3)
        );
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .9, .3, .4), tf->sample_linear(-0.2));
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(.1, .2, .9, .4), tf->sample_linear(3));
        ASSERT_COLOR_EQUALS(
            sight::data::transfer_function::color_t(.1, .2, .75, .525),
            tf->sample_linear(((150. - 3.) / 4.) + 3)
        );
    }

//------------------------------------------------------------------------------

    TEST_CASE("nearest_color")
    {
        sight::data::transfer_function::sptr tf = create_tf_color();

        // Value = -40.33 => color : {0.9, 0.2, 0.3, 0.4}
        // Value = -0.2  => color : {0.1, 0.9, 0.3, 0.4}
        // Value = 3     => color : {0.1, 0.2, 0.9, 0.4}
        // Value = 150   => color : {0.1, 0.2, 0.3, 0.9}

        auto piece = tf->pieces()[0];

        piece->set_clamped(true);
        CHECK(sight::data::transfer_function::color_t(.0, .0, .0, .0) == piece->sample_nearest(-120));
        CHECK(sight::data::transfer_function::color_t(.0, .0, .0, .0) == piece->sample_nearest(200));
        CHECK(sight::data::transfer_function::color_t(.9, .2, .3, .4) == piece->sample_nearest(-40.33));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .3, .9) == piece->sample_nearest(150));
        CHECK(
            sight::data::transfer_function::color_t(
                .1,
                .2,
                .3,
                .9
            ) == piece->sample_nearest(((150. - 3.) / 2.) + 3 + .1)
        );
        CHECK(sight::data::transfer_function::color_t(.1, .9, .3, .4) == piece->sample_nearest(-0.2));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .9, .4) == piece->sample_nearest(3));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .9, .4) == piece->sample_nearest(((150. - 3.) / 4.) + 3));

        CHECK(sight::data::transfer_function::color_t(.0, .0, .0, .0) == tf->sample_nearest(-120));
        CHECK(sight::data::transfer_function::color_t(.0, .0, .0, .0) == tf->sample_nearest(200));
        CHECK(sight::data::transfer_function::color_t(.9, .2, .3, .4) == tf->sample_nearest(-40.33));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .3, .9) == tf->sample_nearest(150));
        CHECK(
            sight::data::transfer_function::color_t(
                .1,
                .2,
                .3,
                .9
            ) == tf->sample_nearest(((150. - 3.) / 2.) + 3 + .1)
        );
        CHECK(sight::data::transfer_function::color_t(.1, .9, .3, .4) == tf->sample_nearest(-0.2));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .9, .4) == tf->sample_nearest(3));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .9, .4) == tf->sample_nearest(((150. - 3.) / 4.) + 3));

        piece->set_interpolation_mode(sight::data::transfer_function::interpolation_mode::nearest);
        CHECK(sight::data::transfer_function::color_t(.0, .0, .0, .0) == piece->sample(-120));
        CHECK(sight::data::transfer_function::color_t(.0, .0, .0, .0) == piece->sample(200));
        CHECK(sight::data::transfer_function::color_t(.9, .2, .3, .4) == piece->sample(-40.33));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .3, .9) == piece->sample(150));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .3, .9) == piece->sample(((150. - 3.) / 2.) + 3 + 0.1));
        CHECK(sight::data::transfer_function::color_t(.1, .9, .3, .4) == piece->sample(-0.2));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .9, .4) == piece->sample(3));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .9, .4) == piece->sample(((150. - 3.) / 4.) + 3));

        CHECK(sight::data::transfer_function::color_t(.0, .0, .0, .0) == tf->sample(-120));
        CHECK(sight::data::transfer_function::color_t(.0, .0, .0, .0) == tf->sample(200));
        CHECK(sight::data::transfer_function::color_t(.9, .2, .3, .4) == tf->sample(-40.33));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .3, .9) == tf->sample(150));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .3, .9) == tf->sample(((150. - 3.) / 2.) + 3 + 0.1));
        CHECK(sight::data::transfer_function::color_t(.1, .9, .3, .4) == tf->sample(-0.2));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .9, .4) == tf->sample(3));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .9, .4) == tf->sample(((150. - 3.) / 4.) + 3));

        piece->set_clamped(false);
        CHECK(sight::data::transfer_function::color_t(.9, .2, .3, .4) == piece->sample_nearest(-120));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .3, .9) == piece->sample_nearest(200));
        CHECK(sight::data::transfer_function::color_t(.9, .2, .3, .4) == piece->sample_nearest(-40.33));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .3, .9) == piece->sample_nearest(150));
        CHECK(
            sight::data::transfer_function::color_t(
                .1,
                .2,
                .3,
                .9
            ) == piece->sample_nearest(((150. - 3.) / 2.) + 3 + .1)
        );
        CHECK(sight::data::transfer_function::color_t(.1, .9, .3, .4) == piece->sample_nearest(-0.2));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .9, .4) == piece->sample_nearest(3));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .9, .4) == piece->sample_nearest(((150. - 3.) / 4.) + 3));

        CHECK(sight::data::transfer_function::color_t(.9, .2, .3, .4) == tf->sample_nearest(-120));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .3, .9) == tf->sample_nearest(200));
        CHECK(sight::data::transfer_function::color_t(.9, .2, .3, .4) == tf->sample_nearest(-40.33));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .3, .9) == tf->sample_nearest(150));
        CHECK(
            sight::data::transfer_function::color_t(
                .1,
                .2,
                .3,
                .9
            ) == tf->sample_nearest(((150. - 3.) / 2.) + 3 + .1)
        );
        CHECK(sight::data::transfer_function::color_t(.1, .9, .3, .4) == tf->sample_nearest(-0.2));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .9, .4) == tf->sample_nearest(3));
        CHECK(sight::data::transfer_function::color_t(.1, .2, .9, .4) == tf->sample_nearest(((150. - 3.) / 4.) + 3));
    }

//------------------------------------------------------------------------------

    TEST_CASE("map_values")
    {
        const auto tf_data = std::make_shared<sight::data::transfer_function_piece>();

        tf_data->set_clamped(false);
        tf_data->set_window_min_max({-200, 300.});
        tf_data->insert({0., {0., 0., 0., 0.}});
        tf_data->insert({1., {1., 1., 1., 1.}});

        CHECK_EQ(-200., tf_data->map_value_to_window(0.));
        CHECK_EQ(50., tf_data->map_value_to_window(0.5));
        CHECK_EQ(300., tf_data->map_value_to_window(1.));

        CHECK_EQ(0., tf_data->map_value_from_window(-200.));
        CHECK_EQ(.5, tf_data->map_value_from_window(50.));
        CHECK_EQ(1., tf_data->map_value_from_window(300.));

        CHECK(sight::data::transfer_function::color_t(.0, .0, .0, .0) == tf_data->sample_nearest(-200));
        CHECK(sight::data::transfer_function::color_t(.0, .0, .0, .0) == tf_data->sample_nearest(0));
        CHECK(sight::data::transfer_function::color_t(1.0, 1.0, 1.0, 1.0) == tf_data->sample_nearest(55));
        CHECK(sight::data::transfer_function::color_t(1.0, 1.0, 1.0, 1.0) == tf_data->sample_nearest(100));

        // Insert a point that changes the maximum value
        tf_data->insert({2., {1., 1., 0., 1.}});

        CHECK_EQ(-200., tf_data->map_value_to_window(0.));
        CHECK_EQ(-75., tf_data->map_value_to_window(0.5));
        CHECK_EQ(50., tf_data->map_value_to_window(1.));
        CHECK_EQ(300., tf_data->map_value_to_window(2.));

        CHECK_EQ(0., tf_data->map_value_from_window(-200.));
        CHECK_EQ(.5, tf_data->map_value_from_window(-75.));
        CHECK_EQ(1., tf_data->map_value_from_window(50.));
        CHECK_EQ(2., tf_data->map_value_from_window(300.));

        CHECK(sight::data::transfer_function::color_t(.0, .0, .0, .0) == tf_data->sample_nearest(-200));
        CHECK(sight::data::transfer_function::color_t(1.0, 1.0, 1.0, 1.0) == tf_data->sample_nearest(0));
        CHECK(sight::data::transfer_function::color_t(1.0, 1.0, .5, 1.0) == tf_data->sample_linear(175));
        CHECK(sight::data::transfer_function::color_t(1.0, 1.0, 0., 1.0) == tf_data->sample_nearest(300.));
    }

//------------------------------------------------------------------------------

    TEST_CASE("piecewise_function")
    {
        auto tf = std::make_shared<sight::data::transfer_function>();

        auto piece_0 = std::make_shared<sight::data::transfer_function_piece>();

        CHECK(tf->empty());

        auto& pieces = tf->pieces();
        pieces.emplace_back(piece_0);

        piece_0->set_clamped(false);
        piece_0->set_window_min_max({-10, 0.});
        piece_0->insert({-10.0, {0.0, 0.0, 0.0, 0.0}}); // 1
        piece_0->insert({-5.0, {1.0, 0.3, 0.0, 1.0}});  // 2
        piece_0->insert({0.0, {0.0, 0.0, 1.0, 0.4}});   // 3

        CHECK_EQ(std::size_t(3), piece_0->size());
        CHECK_EQ(-5., piece_0->level());
        CHECK_EQ(10., piece_0->window());

        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(0.0, 0.0, 0.0, 0.0), piece_0->find(-10.)->second);
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(1.0, 0.3, 0.0, 1.0), piece_0->find(-5.)->second);
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(0.0, 0.0, 1.0, 0.4), piece_0->find(0)->second);

        piece_0->set_clamped(true);

        CHECK_EQ(std::size_t(3), piece_0->size());
        CHECK_EQ(-5., piece_0->level());
        CHECK_EQ(10.0, piece_0->window());

        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(0.0, 0.0, 0.0, 0.0), piece_0->find(-10.)->second);
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(1.0, 0.3, 0.0, 1.0), piece_0->find(-5.)->second);
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(0.0, 0.0, 1.0, 0.4), piece_0->find(0)->second);

        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(0.0, 0.0, 0.0, 0.0), piece_0->begin()->second);
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(0.0, 0.0, 1.0, 0.4), piece_0->rbegin()->second);

        auto piece_1 = std::make_shared<sight::data::transfer_function_piece>();
        pieces.emplace_back(piece_1);

        piece_1->set_clamped(false);
        piece_1->set_window_min_max({0, 100.});
        piece_1->insert({0.0, {0.0, 0.6, 0.0, 0.5}});   // 4
        piece_1->insert({50.0, {1.0, 1.0, 0.0, 1.0}});  // 5
        piece_1->insert({100.0, {1.0, 0.0, 1.0, 0.5}}); // 6

        CHECK_EQ(std::size_t(3), piece_1->size());

        tf->fit_window();
        CHECK_EQ(45.0, doctest::Approx(tf->level()).epsilon(EPSILON));
        CHECK_EQ(110.0, doctest::Approx(tf->window()).epsilon(EPSILON));

        // Yields 1 and 4 because the second tf is unclamped
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(0.0, 0.6, 0.0, 0.5), tf->sample(-10.));
        // Yields 2 and 4 because the second tf is unclamped
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(1.0, 0.6, 0.0, 1.0), tf->sample(-5.));
        // Yields 3 and exactly 4
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(0.0, 0.6, 0.8, 0.5), tf->sample(0.));
        // Yields exactly 5
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(1.0, 1.0, 0.0, 1.0), tf->sample(50.));
        // Yields exactly 6
        ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(1.0, 0.0, 1.0, 0.5), tf->sample(100.));

        // Test windowing
        tf->set_level(200);
        CHECK_EQ(200.0, doctest::Approx(tf->level()).epsilon(EPSILON));
        CHECK_EQ(110.0, doctest::Approx(tf->window()).epsilon(EPSILON));
        CHECK_EQ(150.0, doctest::Approx(pieces[0]->level()).epsilon(EPSILON));
        CHECK_EQ(10.0, doctest::Approx(pieces[0]->window()).epsilon(EPSILON));
        CHECK_EQ(205.0, doctest::Approx(pieces[1]->level()).epsilon(EPSILON));
        CHECK_EQ(100.0, doctest::Approx(pieces[1]->window()).epsilon(EPSILON));

        tf->set_window(55);
        CHECK_EQ(200.0, doctest::Approx(tf->level()).epsilon(EPSILON));
        CHECK_EQ(55.0, doctest::Approx(tf->window()).epsilon(EPSILON));
        CHECK_EQ(150.0, doctest::Approx(pieces[0]->level()).epsilon(EPSILON));
        CHECK_EQ(5.0, doctest::Approx(pieces[0]->window()).epsilon(EPSILON));
        CHECK_EQ(205.0, doctest::Approx(pieces[1]->level()).epsilon(EPSILON));
        CHECK_EQ(50.0, doctest::Approx(pieces[1]->window()).epsilon(EPSILON));
    }

//------------------------------------------------------------------------------

    TEST_CASE("equality")
    {
        auto function1 = std::make_shared<sight::data::transfer_function>();
        auto function2 = std::make_shared<sight::data::transfer_function>();

        CHECK(*function1 == *function2);
        CHECK(!(*function1 != *function2));

        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
            function1->op; \
            CHECK_MESSAGE( \
                *function1 != *function2, \
                "Transfer functions should be different when using " #op " on the first one" \
            ); \
            CHECK_MESSAGE( \
                !(*function1 == *function2), \
                "Transfer functions should be different when using " #op " on the first one" \
            ); \
            function2->op; \
            CHECK_MESSAGE( \
                *function1 == *function2, \
                "Transfer functions should be equal when using " #op " on both" \
            ); \
            CHECK_MESSAGE( \
                !(*function1 != *function2), \
                "Transfer functions should be equal when using " #op " on both" \
            );

        // transfer_function::setWindow and transfer_function::setLevel aren't tested here, as the behavior is
        // counterintuitive: The equality of the TransferFunction isn't changed by setWindow et setLevel if the list of
        // transfer_function_piece is empty. Whether this is the correct behavior or not is still to be determined.

        TEST(set_name("3"));
        TEST(set_background_color({4, 5, 6, 7}));
        TEST(pieces().push_back(std::make_shared<sight::data::transfer_function_piece>()));

    #undef TEST
    }

//------------------------------------------------------------------------------

    TEST_CASE("merge")
    {
        auto function_0      = std::make_shared<sight::data::transfer_function>();
        auto function_1      = std::make_shared<sight::data::transfer_function>();
        auto function_backup = std::make_shared<sight::data::transfer_function>();

        auto piece_0 = std::make_shared<sight::data::transfer_function_piece>();
        {
            {
                piece_0->set_clamped(false);
                piece_0->set_window_min_max({-10, 0.});
                piece_0->insert({-10.0, {0.0, 0.0, 0.0, 0.0}});
                piece_0->insert({-5.0, {1.0, 0.3, 0.0, 1.0}});
                piece_0->insert({0.0, {0.0, 0.0, 1.0, 0.4}});
                function_0->pieces().push_back(piece_0);
            }

            // Copy this piece
            {
                auto a_piece_copy = std::make_shared<sight::data::transfer_function_piece>();
                *a_piece_copy = *piece_0;
                function_1->pieces().push_back(a_piece_copy);
            }

            // The same as the snippet above, but for function_backup
            {
                auto another_piece_copy = std::make_shared<sight::data::transfer_function_piece>();
                *another_piece_copy = *piece_0;
                function_backup->pieces().push_back(another_piece_copy);
            }
        }

        auto piece_1 = std::make_shared<sight::data::transfer_function_piece>();
        {
            piece_1->set_clamped(false);
            piece_1->set_window_min_max({0, 100.});
            piece_1->insert({0.0, {0.0, 0.6, 0.0, 0.5}});
            piece_1->insert({50.0, {1.0, 1.0, 0.0, 1.0}});
            piece_1->insert({100.0, {1.0, 0.0, 1.0, 0.5}});

            function_1->pieces().push_back(piece_1);
        }

        // 1: Test that attempting to merge one TF into itself doesn't do anything
        {
            CHECK(*function_0 == *function_backup);
            sight::data::transfer_function::merge(*function_0, *function_0);
            CHECK(*function_0 == *function_backup);
        }

        // 2: Add another piece to the second TF, and confirm that only new pieces were added
        {
            sight::data::transfer_function::merge(*function_0, *function_1);
            CHECK(*function_0 == *function_1);
            CHECK(*function_0 != *function_backup);
            CHECK(function_0->pieces().size() == 2);

            // Check that the information was correctly forwarded (no piece got modified in the process)
            for(const auto& piece : function_0->pieces())
            {
                CHECK((*piece == *piece_0 || *piece == *piece_1));
            }
        }
    }
} // TEST_SUITE("sight::data::transfer_function")
