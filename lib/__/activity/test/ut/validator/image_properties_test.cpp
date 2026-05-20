/************************************************************************
 *
 * Copyright (C) 2016-2026 IRCAD France
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

#include <activity/extension/activity.hpp>
#include <activity/validator/base.hpp>

#include <core/runtime/extension.hpp>
#include <core/runtime/module.hpp>

#include <data/activity.hpp>
#include <data/image_series.hpp>
#include <data/map.hpp>
#include <data/model_series.hpp>
#include <data/validator/base.hpp>
#include <data/vector.hpp>

#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::activity::image_properties")
{
//------------------------------------------------------------------------------

    static sight::data::image_series::sptr image_to_image_series(const sight::data::object::sptr& _obj)
    {
        auto img = std::dynamic_pointer_cast<sight::data::image>(_obj);
        auto res = std::make_shared<sight::data::image_series>();
        res->resize(img->size(), img->type(), img->pixel_format());
        res->set_spacing(img->spacing());
        res->set_origin(img->origin());
        return res;
    }

//------------------------------------------------------------------------------

    TEST_CASE("properties")
    {
        const auto validator = sight::data::validator::factory::make("sight::activity::validator::image_properties");
        CHECK(validator);

        {
            sight::data::image::sptr img1 = std::make_shared<sight::data::image>();
            sight::data::image::sptr img2 = std::make_shared<sight::data::image>();
            sight::utest_data::generator::image::generate_random_image(img1, sight::core::type::UINT8);
            sight::utest_data::generator::image::generate_random_image(img2, sight::core::type::UINT8);

            sight::data::vector::sptr vector = std::make_shared<sight::data::vector>();
            vector->push_back(img1);
            vector->push_back(img2);

            sight::data::validator::return_t validation;

            validation = validator->validate(vector);
            CHECK_EQ(false, validation.first);

            sight::data::map::sptr map = std::make_shared<sight::data::map>();
            (*map)["img1"] = img1;
            (*map)["img2"] = img2;

            validation = validator->validate(map);
            CHECK_EQ(false, validation.first);

            auto series_vector = std::make_shared<sight::data::vector>();
            std::ranges::transform(*vector, std::back_inserter(*series_vector), image_to_image_series);
            validation = validator->validate(series_vector);
            CHECK_EQ(false, validation.first);
        }

        {
            sight::data::image::sptr img1 = std::make_shared<sight::data::image>();
            sight::data::image::sptr img2 = std::make_shared<sight::data::image>();

            sight::utest_data::generator::image::generate_random_image(img1, sight::core::type::UINT8);

            sight::utest_data::generator::image::generate_image(
                img2,
                img1->size(),
                img1->spacing(),
                img1->origin(),
                img1->orientation(),
                img1->type(),
                sight::data::image::pixel_format_t::gray_scale
            );

            sight::data::vector::sptr vector = std::make_shared<sight::data::vector>();
            vector->push_back(img1);
            vector->push_back(img2);

            sight::activity::validator::return_t validation;

            validation = validator->validate(vector);
            CHECK_EQ(true, validation.first);

            sight::data::map::sptr map = std::make_shared<sight::data::map>();
            (*map)["img1"] = img1;
            (*map)["img2"] = img2;

            validation = validator->validate(map);
            CHECK_EQ(true, validation.first);

            auto series_vector = std::make_shared<sight::data::vector>();
            std::ranges::transform(*vector, std::back_inserter(*series_vector), image_to_image_series);
            validation = validator->validate(series_vector);
            CHECK_EQ(true, validation.first);
        }
    }

//------------------------------------------------------------------------------
} // end TEST_SUITE
