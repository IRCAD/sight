/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "image_conversion_test.hpp"

#include "helper.hpp"

#include <core/base.hpp>
#include <core/tools/random/generator.hpp>

#include <data/object.hpp>

#include <io/itk/itk.hpp>

#include <utest_data/generator/image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::itk::ut::image_conversion_test);

namespace sight::io::itk::ut
{

using core::tools::random::safe_rand;

//------------------------------------------------------------------------------

void image_conversion_test::setUp()
{
}

//------------------------------------------------------------------------------

void image_conversion_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void image_conversion_test::test_conversion()
{
    // create Image
    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generate_random_image(image, core::type::INT16);
    const data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
    image->set_orientation(orientation);

    using image_t = ::itk::Image<std::int16_t, 3>;
    image_t::Pointer itk_image = io::itk::move_to_itk<image_t>(image);

    data::image::sptr image2 = std::make_shared<data::image>();
    io::itk::move_from_itk<image_t>(itk_image, image2, false);

    io::itk::ut::helper::round_spacing(image);
    io::itk::ut::helper::round_spacing(image2);

    image2->set_window_center(image->window_center());
    image2->set_window_width(image->window_width());

    CPPUNIT_ASSERT(*image == *image2);

    data::image::sptr image3 = io::itk::move_from_itk<image_t>(itk_image, false);
    image3->set_window_center(image->window_center());
    image3->set_window_width(image->window_width());

    CPPUNIT_ASSERT(*image == *image3);
}

//------------------------------------------------------------------------------

void image_conversion_test::stress_test()
{
    stress_test_for_a_type<std::int8_t>();
    stress_test_for_a_type<std::uint8_t>();

    stress_test_for_a_type<std::int16_t>();
    stress_test_for_a_type<std::uint16_t>();

    stress_test_for_a_type<std::int32_t>();
    stress_test_for_a_type<std::uint32_t>();

    stress_test_for_a_type<std::int64_t>();
    stress_test_for_a_type<std::uint64_t>();

    stress_test_for_a_type<float>();
    stress_test_for_a_type<double>();
}

//------------------------------------------------------------------------------

void image_conversion_test::test_conversion_2d()
{
    // create Image
    data::image::sptr image  = std::make_shared<data::image>();
    data::image::size_t size = {
        static_cast<std::size_t>(safe_rand() % 100 + 2), static_cast<std::size_t>(safe_rand() % 100 + 2), 0
    };
    data::image::spacing_t spacing         = {(safe_rand() % 200 + 1) / 100., (safe_rand() % 200 + 1) / 100., 0.};
    data::image::origin_t origin           = {(safe_rand() % 200 - 100) / 3., (safe_rand() % 200 - 100) / 3., 0.};
    data::image::orientation_t orientation = {0.36F, 0.48F, 0.0F, -0.8F, 0.6F, 0.0F, 0.0F, 0.0F, 1.0F};
    core::type type                        = core::type::INT16;

    utest_data::generator::image::generate_image(
        image,
        size,
        spacing,
        origin,
        orientation,
        type,
        data::image::gray_scale,
        0
    );

    using image_t = ::itk::Image<std::int16_t, 2>;

    image_t::Pointer itk_image = io::itk::move_to_itk<image_t>(image);

    data::image::sptr image2       = std::make_shared<data::image>();
    bool image2_manages_his_buffer = false;
    io::itk::move_from_itk<image_t>(itk_image, image2, image2_manages_his_buffer);

    io::itk::ut::helper::round_spacing(image);
    io::itk::ut::helper::round_spacing(image2);

    image2->set_window_center(image->window_center());
    image2->set_window_width(image->window_width());

    CPPUNIT_ASSERT(*image == *image2);

    data::image::sptr image3 = io::itk::move_from_itk<image_t>(itk_image, false);
    image3->set_window_center(image->window_center());
    image3->set_window_width(image->window_width());

    CPPUNIT_ASSERT(*image == *image3);
}

//------------------------------------------------------------------------------

} // namespace sight::io::itk::ut
