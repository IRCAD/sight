/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
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
#include <data/composite.hpp>
#include <data/image_series.hpp>
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

static data::image_series::sptr imageToImageSeries(const data::object::sptr& obj)
{
    auto img = std::dynamic_pointer_cast<data::image>(obj);
    auto res = std::make_shared<data::image_series>();
    res->resize(img->size(), img->getType(), img->getPixelFormat());
    res->setSpacing(img->getSpacing());
    res->setOrigin(img->getOrigin());
    return res;
}

//------------------------------------------------------------------------------

void image_properties_test::propertiesTest()
{
    activity::validator::base::sptr validator =
        activity::validator::factory::make("sight::activity::validator::image_properties");
    CPPUNIT_ASSERT(validator);

    activity::validator::object::sptr objValidator = std::dynamic_pointer_cast<activity::validator::object>(validator);
    CPPUNIT_ASSERT(objValidator);

    {
        data::image::sptr img1 = std::make_shared<data::image>();
        data::image::sptr img2 = std::make_shared<data::image>();
        utest_data::generator::image::generateRandomImage(img1, core::type::UINT8);
        utest_data::generator::image::generateRandomImage(img2, core::type::UINT8);

        data::vector::sptr vector = std::make_shared<data::vector>();
        vector->push_back(img1);
        vector->push_back(img2);

        activity::validator::return_t validation;

        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL(false, validation.first);

        data::composite::sptr composite = std::make_shared<data::composite>();
        (*composite)["img1"] = img1;
        (*composite)["img2"] = img2;

        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL(false, validation.first);

        auto seriesVector = std::make_shared<data::vector>();
        std::ranges::transform(*vector, std::back_inserter(*seriesVector), imageToImageSeries);
        validation = objValidator->validate(activity::extension::activity_info {}, seriesVector);
        CPPUNIT_ASSERT_EQUAL(false, validation.first);
    }

    {
        data::image::sptr img1 = std::make_shared<data::image>();
        data::image::sptr img2 = std::make_shared<data::image>();

        utest_data::generator::image::generateRandomImage(img1, core::type::UINT8);

        utest_data::generator::image::generateImage(
            img2,
            img1->size(),
            img1->getSpacing(),
            img1->getOrigin(),
            img1->getType(),
            data::image::PixelFormat::GRAY_SCALE
        );

        data::vector::sptr vector = std::make_shared<data::vector>();
        vector->push_back(img1);
        vector->push_back(img2);

        activity::validator::return_t validation;

        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL(true, validation.first);

        data::composite::sptr composite = std::make_shared<data::composite>();
        (*composite)["img1"] = img1;
        (*composite)["img2"] = img2;

        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL(true, validation.first);

        auto seriesVector = std::make_shared<data::vector>();
        std::ranges::transform(*vector, std::back_inserter(*seriesVector), imageToImageSeries);
        validation = objValidator->validate(activity::extension::activity_info {}, seriesVector);
        CPPUNIT_ASSERT_EQUAL(true, validation.first);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::activity::ut
