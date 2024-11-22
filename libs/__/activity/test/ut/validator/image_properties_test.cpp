/************************************************************************
 *
 * Copyright (C) 2016-2024 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include "image_properties_test.hpp"

#include <activity/extension/activity.hpp>
#include <activity/validator/base.hpp>
#include <activity/validator/object.hpp>

#include <core/runtime/extension.hpp>
#include <core/runtime/module.hpp>

#include <data/activity.hpp>
#include <data/image_series.hpp>
#include <data/map.hpp>
#include <data/model_series.hpp>
#include <data/vector.hpp>

#include <utest_data/generator/image.hpp>

#include <cstdint>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::activity::ut::image_properties_test);

namespace sight::activity::ut
{

//------------------------------------------------------------------------------

void image_properties_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void image_properties_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

static data::image_series::sptr image_to_image_series(const data::object::sptr& _obj)
{
    auto img = std::dynamic_pointer_cast<data::image>(_obj);
    auto res = std::make_shared<data::image_series>();
    res->resize(img->size(), img->type(), img->pixel_format());
    res->set_spacing(img->spacing());
    res->set_origin(img->origin());
    return res;
}

//------------------------------------------------------------------------------

void image_properties_test::properties_test()
{
    activity::validator::base::sptr validator =
        activity::validator::factory::make("sight::activity::validator::image_properties");
    CPPUNIT_ASSERT(validator);

    activity::validator::object::sptr obj_validator = std::dynamic_pointer_cast<activity::validator::object>(validator);
    CPPUNIT_ASSERT(obj_validator);

    {
        data::image::sptr img1 = std::make_shared<data::image>();
        data::image::sptr img2 = std::make_shared<data::image>();
        utest_data::generator::image::generate_random_image(img1, core::type::UINT8);
        utest_data::generator::image::generate_random_image(img2, core::type::UINT8);

        data::vector::sptr vector = std::make_shared<data::vector>();
        vector->push_back(img1);
        vector->push_back(img2);

        activity::validator::return_t validation;

        validation = obj_validator->validate(vector);
        CPPUNIT_ASSERT_EQUAL(false, validation.first);

        data::map::sptr map = std::make_shared<data::map>();
        (*map)["img1"] = img1;
        (*map)["img2"] = img2;

        validation = obj_validator->validate(map);
        CPPUNIT_ASSERT_EQUAL(false, validation.first);

        auto series_vector = std::make_shared<data::vector>();
        std::ranges::transform(*vector, std::back_inserter(*series_vector), image_to_image_series);
        validation = obj_validator->validate(activity::extension::activity_info {}, series_vector);
        CPPUNIT_ASSERT_EQUAL(false, validation.first);
    }

    {
        data::image::sptr img1 = std::make_shared<data::image>();
        data::image::sptr img2 = std::make_shared<data::image>();

        utest_data::generator::image::generate_random_image(img1, core::type::UINT8);

        utest_data::generator::image::generate_image(
            img2,
            img1->size(),
            img1->spacing(),
            img1->origin(),
            img1->type(),
            data::image::pixel_format_t::gray_scale
        );

        data::vector::sptr vector = std::make_shared<data::vector>();
        vector->push_back(img1);
        vector->push_back(img2);

        activity::validator::return_t validation;

        validation = obj_validator->validate(vector);
        CPPUNIT_ASSERT_EQUAL(true, validation.first);

        data::map::sptr map = std::make_shared<data::map>();
        (*map)["img1"] = img1;
        (*map)["img2"] = img2;

        validation = obj_validator->validate(map);
        CPPUNIT_ASSERT_EQUAL(true, validation.first);

        auto series_vector = std::make_shared<data::vector>();
        std::ranges::transform(*vector, std::back_inserter(*series_vector), image_to_image_series);
        validation = obj_validator->validate(activity::extension::activity_info {}, series_vector);
        CPPUNIT_ASSERT_EQUAL(true, validation.first);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::activity::ut
