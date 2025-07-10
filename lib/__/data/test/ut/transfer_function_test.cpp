/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "transfer_function_test.hpp"

#include <data/color.hpp>
#include <data/string.hpp>
#include <data/transfer_function.hpp>

#include <glm/common.hpp>
#include <glm/gtc/epsilon.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::transfer_function_test);

static const double EPSILON = 1e-5;

// There might be some uncertainty when sampling, so we need to include an epsilon when testing equality
#define ASSERT_COLOR_EQUALS(c1, c2) \
        CPPUNIT_ASSERT(glm::all(glm::epsilonEqual(c1, c2, EPSILON)));

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void transfer_function_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void transfer_function_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void transfer_function_test::constructor_test()
{
    data::transfer_function::sptr tf = std::make_shared<data::transfer_function>();

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong level ", 0.0, tf->level());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong window", 2.0, tf->window());

    CPPUNIT_ASSERT_EQUAL(std::string(), tf->name());
    CPPUNIT_ASSERT(transfer_function::color_t() == tf->background_color());

    auto tf_data = tf->pieces().emplace_back(std::make_shared<data::transfer_function_piece>());

    CPPUNIT_ASSERT_EQUAL(transfer_function::interpolation_mode::linear, tf_data->get_interpolation_mode());
    CPPUNIT_ASSERT_EQUAL(true, tf_data->clamped());
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), tf_data->size());
}

//------------------------------------------------------------------------------
void transfer_function_test::default_tf_test()
{
    data::transfer_function::csptr tf = data::transfer_function::create_default_tf();

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong level ", 50.0, tf->level(), EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong window", 500.0, tf->window(), EPSILON);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong level ", 50.0, tf->pieces()[0]->level(), EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong window", 500.0, tf->pieces()[0]->window(), EPSILON);

    CPPUNIT_ASSERT_EQUAL(transfer_function::DEFAULT_TF_NAME, tf->name());
    CPPUNIT_ASSERT(transfer_function::color_t() == tf->background_color());

    const auto first_piece = tf->pieces().front();

    CPPUNIT_ASSERT_EQUAL(transfer_function::interpolation_mode::linear, first_piece->get_interpolation_mode());
    CPPUNIT_ASSERT_EQUAL(false, first_piece->clamped());
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), first_piece->size());
}

//------------------------------------------------------------------------------

void transfer_function_test::classic_get_set_test()
{
    data::transfer_function::sptr tf = data::transfer_function::create_default_tf();

    // Test getTFData()
    const auto tf_data                    = tf->pieces().front();
    auto itr                              = tf_data->cbegin();
    const transfer_function::value_t key1 = itr->first;
    const transfer_function::value_t key2 = (++itr)->first;

    auto itr_color                          = tf_data->cbegin();
    const transfer_function::color_t color1 = itr_color->second;
    const transfer_function::color_t color2 = (++itr_color)->second;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, key1, 1e-10);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, key2, 1e-10);

    const transfer_function::color_t expected_color1(0.0, 0.0, 0.0, 0.0);
    const transfer_function::color_t expected_color2(1.0, 1.0, 1.0, 1.0);
    CPPUNIT_ASSERT(expected_color1 == color1);
    CPPUNIT_ASSERT(expected_color2 == color2);
    CPPUNIT_ASSERT(expected_color1 == tf_data->find(key1)->second);

    // Test erase
    tf_data->erase(1.0);
    CPPUNIT_ASSERT_EQUAL((std::size_t) 1, tf_data->size());

    // Test clear()
    tf_data->clear();
    const std::size_t expected_cleared_size = 0;
    CPPUNIT_ASSERT_EQUAL(expected_cleared_size, tf_data->size());
}

//------------------------------------------------------------------------------

void transfer_function_test::shallow_and_deep_copy_test()
{
    const data::transfer_function::sptr tf = sight::data::ut::transfer_function_test::create_tf_color();
    tf->set_level(900.6);
    tf->set_window(-200.02);

    sight::data::ut::transfer_function_test::check_tf_color(tf);

    const data::transfer_function::sptr shallow_copy_tf = std::make_shared<data::transfer_function>();
    shallow_copy_tf->shallow_copy(tf);
    sight::data::ut::transfer_function_test::check_tf_color(shallow_copy_tf);
    CPPUNIT_ASSERT_EQUAL(tf->pieces()[0], shallow_copy_tf->pieces()[0]);

    const data::transfer_function::sptr deep_copy_tf = data::object::copy(tf);
    sight::data::ut::transfer_function_test::check_tf_color(deep_copy_tf);
    // Pointer addresses must be different
    CPPUNIT_ASSERT(tf->pieces()[0] != deep_copy_tf->pieces()[0]);
}

//------------------------------------------------------------------------------

data::transfer_function::sptr transfer_function_test::create_tf_color()
{
    data::transfer_function::sptr tf = std::make_shared<data::transfer_function>();

    tf->set_background_color(data::transfer_function::color_t(1.0, 0.3, 0.6, 0.1));
    tf->set_name("color_t");
    tf->set_window_min_max({-40.33, 150.});

    auto tf_data = tf->pieces().emplace_back(std::make_shared<data::transfer_function_piece>());
    tf_data->set_clamped(false);
    tf_data->set_interpolation_mode(data::transfer_function::interpolation_mode::nearest);
    tf_data->set_window_min_max({-40.33, 150.});

    tf_data->insert({-40.33, data::transfer_function::color_t(0.9, 0.2, 0.3, 0.4)});
    tf_data->insert({3, data::transfer_function::color_t(0.1, 0.2, 0.9, 0.4)}); // Invert point 3 <=> -0.2, for
    // tests
    tf_data->insert({-0.2, data::transfer_function::color_t(0.1, 0.9, 0.3, 0.4)});
    tf_data->insert({150, data::transfer_function::color_t(0.1, 0.2, 0.3, 0.9)});

    data::string::sptr my_string = std::make_shared<data::string>("fieldStringValue");
    tf->set_field("fieldStringKey", my_string);

    return tf;
}

//------------------------------------------------------------------------------

void transfer_function_test::check_tf_color(data::transfer_function::sptr _tf)
{
    CPPUNIT_ASSERT(data::transfer_function::color_t(1.0, 0.3, 0.6, 0.1) == _tf->background_color());
    CPPUNIT_ASSERT_EQUAL(std::string("color_t"), _tf->name());

    CPPUNIT_ASSERT_EQUAL(-200.02, _tf->window());
    CPPUNIT_ASSERT_EQUAL(900.6, _tf->level());

    const auto tf_data = _tf->pieces().front();
    CPPUNIT_ASSERT_EQUAL(false, tf_data->clamped());
    CPPUNIT_ASSERT_EQUAL(data::transfer_function::interpolation_mode::nearest, tf_data->get_interpolation_mode());

    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4), tf_data->size());
    CPPUNIT_ASSERT_EQUAL(-40.33, tf_data->min_max().first);
    CPPUNIT_ASSERT_EQUAL(150., tf_data->min_max().second);

    CPPUNIT_ASSERT(data::transfer_function::color_t(0.9, 0.2, 0.3, 0.4) == tf_data->find(-40.33)->second);
    CPPUNIT_ASSERT(data::transfer_function::color_t(0.1, 0.9, 0.3, 0.4) == tf_data->find(-0.2)->second);
    CPPUNIT_ASSERT(data::transfer_function::color_t(0.1, 0.2, 0.9, 0.4) == tf_data->find(3)->second);
    CPPUNIT_ASSERT(data::transfer_function::color_t(0.1, 0.2, 0.3, 0.9) == tf_data->find(150)->second);

    CPPUNIT_ASSERT_EQUAL(
        std::string("fieldStringValue"),
        std::dynamic_pointer_cast<data::string>(_tf->get_field("fieldStringKey"))->value()
    );
}

//------------------------------------------------------------------------------

void transfer_function_test::set_tf_data_test()
{
    data::transfer_function::sptr tf = sight::data::ut::transfer_function_test::create_tf_color();
    tf->set_level(900.6);
    tf->set_window(-200.02);

    auto piece = tf->pieces()[0];
    data::transfer_function::data_t tf_data;
    tf_data[-40.33] = data::transfer_function::color_t(0.9, 0.2, 0.3, 0.4);
    tf_data[3]      = data::transfer_function::color_t(0.1, 0.2, 0.9, 0.4);
    tf_data[-0.2]   = data::transfer_function::color_t(0.1, 0.9, 0.3, 0.4);
    tf_data[150]    = data::transfer_function::color_t(0.1, 0.2, 0.3, 0.9);
    std::copy(tf_data.begin(), tf_data.end(), inserter(*piece));

    sight::data::ut::transfer_function_test::check_tf_color(tf);
}

//------------------------------------------------------------------------------

void transfer_function_test::linear_color_test()
{
    data::transfer_function::sptr tf = sight::data::ut::transfer_function_test::create_tf_color();

    // Value = -40.33 => color : {0.9, 0.2, 0.3, 0.4}
    // Value = -0.2   => color : {0.1, 0.9, 0.3, 0.4}
    // Value = 3      => color : {0.1, 0.2, 0.9, 0.4}
    // Value = 150    => color : {0.1, 0.2, 0.3, 0.9}
    auto piece = tf->pieces()[0];

    piece->set_clamped(true);
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.0, .0, .0, .0), piece->sample_linear(-120));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.0, .0, .0, .0), piece->sample_linear(200));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.9, .2, .3, .4), piece->sample_linear(-40.33));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .3, .9), piece->sample_linear(150));
    ASSERT_COLOR_EQUALS(
        data::transfer_function::color_t(.1, .2, .6, .65),
        piece->sample_linear(((150. - 3.) / 2.) + 3)
    );
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .9, .3, .4), piece->sample_linear(-0.2));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .9, .4), piece->sample_linear(3));
    ASSERT_COLOR_EQUALS(
        data::transfer_function::color_t(.1, .2, .75, .525),
        piece->sample_linear(((150. - 3.) / 4.) + 3)
    );

    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.0, .0, .0, .0), tf->sample_linear(-120));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.0, .0, .0, .0), tf->sample_linear(200));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.9, .2, .3, .4), tf->sample_linear(-40.33));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .3, .9), tf->sample_linear(150));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .6, .65), tf->sample_linear(((150. - 3.) / 2.) + 3));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .9, .3, .4), tf->sample_linear(-0.2));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .9, .4), tf->sample_linear(3));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .75, .525), tf->sample_linear(((150. - 3.) / 4.) + 3));

    piece->set_interpolation_mode(transfer_function::interpolation_mode::linear);
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.0, .0, .0, .0), piece->sample(-120));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.0, .0, .0, .0), piece->sample(200));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.9, .2, .3, .4), piece->sample(-40.33));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .3, .9), piece->sample(150));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .6, .65), piece->sample(((150. - 3.) / 2.) + 3));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .9, .3, .4), piece->sample(-0.2));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .9, .4), piece->sample(3));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .75, .525), piece->sample(((150. - 3.) / 4.) + 3));

    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.0, .0, .0, .0), tf->sample(-120));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.0, .0, .0, .0), tf->sample(200));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.9, .2, .3, .4), tf->sample(-40.33));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .3, .9), tf->sample(150));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .6, .65), tf->sample(((150. - 3.) / 2.) + 3));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .9, .3, .4), tf->sample(-0.2));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .9, .4), tf->sample(3));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .75, .525), tf->sample(((150. - 3.) / 4.) + 3));

    piece->set_clamped(false);
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.9, .2, .3, .4), piece->sample_linear(-120));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .3, .9), piece->sample_linear(200));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.9, .2, .3, .4), piece->sample_linear(-40.33));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .3, .9), piece->sample_linear(150));
    ASSERT_COLOR_EQUALS(
        data::transfer_function::color_t(.1, .2, .6, .65),
        piece->sample_linear(((150. - 3.) / 2.) + 3)
    );
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .9, .3, .4), piece->sample_linear(-0.2));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .9, .4), piece->sample_linear(3));
    ASSERT_COLOR_EQUALS(
        data::transfer_function::color_t(.1, .2, .75, .525),
        piece->sample_linear(((150. - 3.) / 4.) + 3)
    );

    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.9, .2, .3, .4), tf->sample_linear(-120));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .3, .9), tf->sample_linear(200));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.9, .2, .3, .4), tf->sample_linear(-40.33));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .3, .9), tf->sample_linear(150));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .6, .65), tf->sample_linear(((150. - 3.) / 2.) + 3));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .9, .3, .4), tf->sample_linear(-0.2));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .9, .4), tf->sample_linear(3));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(.1, .2, .75, .525), tf->sample_linear(((150. - 3.) / 4.) + 3));
}

//------------------------------------------------------------------------------

void transfer_function_test::nearest_color_test()
{
    data::transfer_function::sptr tf = sight::data::ut::transfer_function_test::create_tf_color();

    // Value = -40.33 => color : {0.9, 0.2, 0.3, 0.4}
    // Value = -0.2  => color : {0.1, 0.9, 0.3, 0.4}
    // Value = 3     => color : {0.1, 0.2, 0.9, 0.4}
    // Value = 150   => color : {0.1, 0.2, 0.3, 0.9}

    auto piece = tf->pieces()[0];

    piece->set_clamped(true);
    CPPUNIT_ASSERT(data::transfer_function::color_t(.0, .0, .0, .0) == piece->sample_nearest(-120));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.0, .0, .0, .0) == piece->sample_nearest(200));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.9, .2, .3, .4) == piece->sample_nearest(-40.33));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .3, .9) == piece->sample_nearest(150));
    CPPUNIT_ASSERT(
        data::transfer_function::color_t(
            .1,
            .2,
            .3,
            .9
        ) == piece->sample_nearest(((150. - 3.) / 2.) + 3 + .1)
    );
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .9, .3, .4) == piece->sample_nearest(-0.2));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .9, .4) == piece->sample_nearest(3));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .9, .4) == piece->sample_nearest(((150. - 3.) / 4.) + 3));

    CPPUNIT_ASSERT(data::transfer_function::color_t(.0, .0, .0, .0) == tf->sample_nearest(-120));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.0, .0, .0, .0) == tf->sample_nearest(200));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.9, .2, .3, .4) == tf->sample_nearest(-40.33));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .3, .9) == tf->sample_nearest(150));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .3, .9) == tf->sample_nearest(((150. - 3.) / 2.) + 3 + .1));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .9, .3, .4) == tf->sample_nearest(-0.2));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .9, .4) == tf->sample_nearest(3));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .9, .4) == tf->sample_nearest(((150. - 3.) / 4.) + 3));

    piece->set_interpolation_mode(transfer_function::interpolation_mode::nearest);
    CPPUNIT_ASSERT(data::transfer_function::color_t(.0, .0, .0, .0) == piece->sample(-120));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.0, .0, .0, .0) == piece->sample(200));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.9, .2, .3, .4) == piece->sample(-40.33));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .3, .9) == piece->sample(150));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .3, .9) == piece->sample(((150. - 3.) / 2.) + 3 + 0.1));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .9, .3, .4) == piece->sample(-0.2));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .9, .4) == piece->sample(3));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .9, .4) == piece->sample(((150. - 3.) / 4.) + 3));

    CPPUNIT_ASSERT(data::transfer_function::color_t(.0, .0, .0, .0) == tf->sample(-120));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.0, .0, .0, .0) == tf->sample(200));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.9, .2, .3, .4) == tf->sample(-40.33));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .3, .9) == tf->sample(150));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .3, .9) == tf->sample(((150. - 3.) / 2.) + 3 + 0.1));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .9, .3, .4) == tf->sample(-0.2));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .9, .4) == tf->sample(3));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .9, .4) == tf->sample(((150. - 3.) / 4.) + 3));

    piece->set_clamped(false);
    CPPUNIT_ASSERT(data::transfer_function::color_t(.9, .2, .3, .4) == piece->sample_nearest(-120));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .3, .9) == piece->sample_nearest(200));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.9, .2, .3, .4) == piece->sample_nearest(-40.33));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .3, .9) == piece->sample_nearest(150));
    CPPUNIT_ASSERT(
        data::transfer_function::color_t(
            .1,
            .2,
            .3,
            .9
        ) == piece->sample_nearest(((150. - 3.) / 2.) + 3 + .1)
    );
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .9, .3, .4) == piece->sample_nearest(-0.2));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .9, .4) == piece->sample_nearest(3));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .9, .4) == piece->sample_nearest(((150. - 3.) / 4.) + 3));

    CPPUNIT_ASSERT(data::transfer_function::color_t(.9, .2, .3, .4) == tf->sample_nearest(-120));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .3, .9) == tf->sample_nearest(200));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.9, .2, .3, .4) == tf->sample_nearest(-40.33));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .3, .9) == tf->sample_nearest(150));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .3, .9) == tf->sample_nearest(((150. - 3.) / 2.) + 3 + .1));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .9, .3, .4) == tf->sample_nearest(-0.2));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .9, .4) == tf->sample_nearest(3));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.1, .2, .9, .4) == tf->sample_nearest(((150. - 3.) / 4.) + 3));
}

//------------------------------------------------------------------------------

void transfer_function_test::map_values_test()
{
    const auto tf_data = std::make_shared<data::transfer_function_piece>();

    tf_data->set_clamped(false);
    tf_data->set_window_min_max({-200, 300.});
    tf_data->insert({0., {0., 0., 0., 0.}});
    tf_data->insert({1., {1., 1., 1., 1.}});

    CPPUNIT_ASSERT_EQUAL(-200., tf_data->map_value_to_window(0.));
    CPPUNIT_ASSERT_EQUAL(50., tf_data->map_value_to_window(0.5));
    CPPUNIT_ASSERT_EQUAL(300., tf_data->map_value_to_window(1.));

    CPPUNIT_ASSERT_EQUAL(0., tf_data->map_value_from_window(-200.));
    CPPUNIT_ASSERT_EQUAL(.5, tf_data->map_value_from_window(50.));
    CPPUNIT_ASSERT_EQUAL(1., tf_data->map_value_from_window(300.));

    CPPUNIT_ASSERT(data::transfer_function::color_t(.0, .0, .0, .0) == tf_data->sample_nearest(-200));
    CPPUNIT_ASSERT(data::transfer_function::color_t(.0, .0, .0, .0) == tf_data->sample_nearest(0));
    CPPUNIT_ASSERT(data::transfer_function::color_t(1.0, 1.0, 1.0, 1.0) == tf_data->sample_nearest(55));
    CPPUNIT_ASSERT(data::transfer_function::color_t(1.0, 1.0, 1.0, 1.0) == tf_data->sample_nearest(100));

    // Insert a point that changes the maximum value
    tf_data->insert({2., {1., 1., 0., 1.}});

    CPPUNIT_ASSERT_EQUAL(-200., tf_data->map_value_to_window(0.));
    CPPUNIT_ASSERT_EQUAL(-75., tf_data->map_value_to_window(0.5));
    CPPUNIT_ASSERT_EQUAL(50., tf_data->map_value_to_window(1.));
    CPPUNIT_ASSERT_EQUAL(300., tf_data->map_value_to_window(2.));

    CPPUNIT_ASSERT_EQUAL(0., tf_data->map_value_from_window(-200.));
    CPPUNIT_ASSERT_EQUAL(.5, tf_data->map_value_from_window(-75.));
    CPPUNIT_ASSERT_EQUAL(1., tf_data->map_value_from_window(50.));
    CPPUNIT_ASSERT_EQUAL(2., tf_data->map_value_from_window(300.));

    CPPUNIT_ASSERT(data::transfer_function::color_t(.0, .0, .0, .0) == tf_data->sample_nearest(-200));
    CPPUNIT_ASSERT(data::transfer_function::color_t(1.0, 1.0, 1.0, 1.0) == tf_data->sample_nearest(0));
    CPPUNIT_ASSERT(data::transfer_function::color_t(1.0, 1.0, .5, 1.0) == tf_data->sample_linear(175));
    CPPUNIT_ASSERT(data::transfer_function::color_t(1.0, 1.0, 0., 1.0) == tf_data->sample_nearest(300.));
}

//------------------------------------------------------------------------------

void transfer_function_test::piecewise_function_test()
{
    auto tf = std::make_shared<data::transfer_function>();

    auto piece_0 = std::make_shared<data::transfer_function_piece>();

    CPPUNIT_ASSERT(tf->empty());

    auto& pieces = tf->pieces();
    pieces.emplace_back(piece_0);

    piece_0->set_clamped(false);
    piece_0->set_window_min_max({-10, 0.});
    piece_0->insert({-10.0, {0.0, 0.0, 0.0, 0.0}}); // 1
    piece_0->insert({-5.0, {1.0, 0.3, 0.0, 1.0}});  // 2
    piece_0->insert({0.0, {0.0, 0.0, 1.0, 0.4}});   // 3

    CPPUNIT_ASSERT_EQUAL(std::size_t(3), piece_0->size());
    CPPUNIT_ASSERT_EQUAL(-5., piece_0->level());
    CPPUNIT_ASSERT_EQUAL(10., piece_0->window());

    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0.0, 0.0, 0.0, 0.0), piece_0->find(-10.)->second);
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(1.0, 0.3, 0.0, 1.0), piece_0->find(-5.)->second);
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0.0, 0.0, 1.0, 0.4), piece_0->find(0)->second);

    piece_0->set_clamped(true);

    CPPUNIT_ASSERT_EQUAL(std::size_t(3), piece_0->size());
    CPPUNIT_ASSERT_EQUAL(-5., piece_0->level());
    CPPUNIT_ASSERT_EQUAL(10.0, piece_0->window());

    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0.0, 0.0, 0.0, 0.0), piece_0->find(-10.)->second);
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(1.0, 0.3, 0.0, 1.0), piece_0->find(-5.)->second);
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0.0, 0.0, 1.0, 0.4), piece_0->find(0)->second);

    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0.0, 0.0, 0.0, 0.0), piece_0->begin()->second);
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0.0, 0.0, 1.0, 0.4), piece_0->rbegin()->second);

    auto piece_1 = std::make_shared<data::transfer_function_piece>();
    pieces.emplace_back(piece_1);

    piece_1->set_clamped(false);
    piece_1->set_window_min_max({0, 100.});
    piece_1->insert({0.0, {0.0, 0.6, 0.0, 0.5}});   // 4
    piece_1->insert({50.0, {1.0, 1.0, 0.0, 1.0}});  // 5
    piece_1->insert({100.0, {1.0, 0.0, 1.0, 0.5}}); // 6

    CPPUNIT_ASSERT_EQUAL(std::size_t(3), piece_1->size());

    tf->fit_window();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(45.0, tf->level(), EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(110.0, tf->window(), EPSILON);

    // Yields 1 and 4 because the second tf is unclamped
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0.0, 0.6, 0.0, 0.5), tf->sample(-10.));
    // Yields 2 and 4 because the second tf is unclamped
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(1.0, 0.6, 0.0, 1.0), tf->sample(-5.));
    // Yields 3 and exactly 4
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0.0, 0.6, 0.8, 0.5), tf->sample(0.));
    // Yields exactly 5
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(1.0, 1.0, 0.0, 1.0), tf->sample(50.));
    // Yields exactly 6
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(1.0, 0.0, 1.0, 0.5), tf->sample(100.));

    // Test windowing
    tf->set_level(200);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(200.0, tf->level(), EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(110.0, tf->window(), EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(150.0, pieces[0]->level(), EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0, pieces[0]->window(), EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(205.0, pieces[1]->level(), EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(100.0, pieces[1]->window(), EPSILON);

    tf->set_window(55);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(200.0, tf->level(), EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(55.0, tf->window(), EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(150.0, pieces[0]->level(), EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0, pieces[0]->window(), EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(205.0, pieces[1]->level(), EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(50.0, pieces[1]->window(), EPSILON);
}

//------------------------------------------------------------------------------

void transfer_function_test::equality_test()
{
    auto function1 = std::make_shared<data::transfer_function>();
    auto function2 = std::make_shared<data::transfer_function>();

    CPPUNIT_ASSERT(*function1 == *function2 && !(*function1 != *function2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
            function1->op; \
            CPPUNIT_ASSERT_MESSAGE( \
                "Transfer functions should be different when using " #op " on the first one", \
                *function1 != *function2 && !(*function1 == *function2) \
            ); \
            function2->op; \
            CPPUNIT_ASSERT_MESSAGE( \
                "Transfer functions should be equal when using " #op " on both", \
                *function1 == *function2 && !(*function1 != *function2) \
            );

    // transfer_function::setWindow and transfer_function::setLevel aren't tested here, as the behavior is
    // counterintuitive: The equality of the TransferFunction isn't changed by setWindow et setLevel if the list of
    // transfer_function_piece is empty. Whether this is the correct behavior or not is still to be determined.

    TEST(set_name("3"));
    TEST(set_background_color({4, 5, 6, 7}));
    TEST(pieces().push_back(std::make_shared<data::transfer_function_piece>()));

    #undef TEST
}

//------------------------------------------------------------------------------

void transfer_function_test::merge_test()
{
    auto function_0      = std::make_shared<data::transfer_function>();
    auto function_1      = std::make_shared<data::transfer_function>();
    auto function_backup = std::make_shared<data::transfer_function>();

    auto piece_0 = std::make_shared<data::transfer_function_piece>();
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
            auto a_piece_copy = std::make_shared<data::transfer_function_piece>();
            *a_piece_copy = *piece_0;
            function_1->pieces().push_back(a_piece_copy);
        }

        // The same as the snippet above, but for function_backup
        {
            auto another_piece_copy = std::make_shared<data::transfer_function_piece>();
            *another_piece_copy = *piece_0;
            function_backup->pieces().push_back(another_piece_copy);
        }
    }

    auto piece_1 = std::make_shared<data::transfer_function_piece>();
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
        CPPUNIT_ASSERT_MESSAGE("Initialisation of the copy failed.", *function_0 == *function_backup);
        sight::data::transfer_function::merge(*function_0, *function_0);
        CPPUNIT_ASSERT_MESSAGE("Merging a TF into itself mustn't affect it.", *function_0 == *function_backup);
    }

    // 2: Add another piece to the second TF, and confirm that only new pieces were added
    {
        sight::data::transfer_function::merge(*function_0, *function_1);
        CPPUNIT_ASSERT_MESSAGE("Pieces should match.", *function_0 == *function_1);
        CPPUNIT_ASSERT_MESSAGE("No piece was inserted.", *function_0 != *function_backup);
        CPPUNIT_ASSERT_MESSAGE("Unexpected piece was added.", function_0->pieces().size() == 2);

        // Check that the information was correctly forwarded (no piece got modified in the process)
        for(const auto& piece : function_0->pieces())
        {
            CPPUNIT_ASSERT_MESSAGE("Pieces must not have been modified.", *piece == *piece_0 or * piece == *piece_1);
        }
    }
}

} // namespace sight::data::ut
