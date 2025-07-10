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

#include "image_test.hpp"

#include <core/type.hpp>

#include <data/image.hpp>

#include <utest_data/generator/image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::utest_data::ut::image_test);

namespace sight::utest_data::ut
{

//------------------------------------------------------------------------------

void image_test::setUp()
{
}

//------------------------------------------------------------------------------

void image_test::tearDown()
{
}

//------------------------------------------------------------------------------

void image_test::generator_3d_test()
{
    data::image::sptr image                      = std::make_shared<data::image>();
    const data::image::size_t size               = {10, 15, 23};
    const data::image::spacing_t spacing         = {0.85, 2.6, 1.87};
    const data::image::origin_t origin           = {-45.6, 25.97, -53.9};
    const data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
    core::type type                              = core::type::INT32;

    utest_data::generator::image::generate_image(
        image,
        size,
        spacing,
        origin,
        orientation,
        type,
        data::image::pixel_format_t::gray_scale
    );

    CPPUNIT_ASSERT(image->size() == size);
    CPPUNIT_ASSERT(image->spacing() == spacing);
    CPPUNIT_ASSERT(image->origin() == origin);
    CPPUNIT_ASSERT_EQUAL(type, image->type());
}

//------------------------------------------------------------------------------

void image_test::generator_2d_test()
{
    data::image::sptr image                      = std::make_shared<data::image>();
    const data::image::size_t size               = {10, 15, 0};
    const data::image::spacing_t spacing         = {0.85, 2.6, 0.};
    const data::image::origin_t origin           = {-45.6, 25.97, 0.};
    const data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
    core::type type                              = core::type::INT16;

    utest_data::generator::image::generate_image(
        image,
        size,
        spacing,
        origin,
        orientation,
        type,
        data::image::pixel_format_t::gray_scale
    );
    CPPUNIT_ASSERT(image->size() == size);
    CPPUNIT_ASSERT(image->spacing() == spacing);
    CPPUNIT_ASSERT(image->origin() == origin);
    CPPUNIT_ASSERT_EQUAL(type, image->type());
}

//------------------------------------------------------------------------------

void image_test::deep_copy_test()
{
    data::image::sptr image = std::make_shared<data::image>();
    data::image::sptr image2;
    core::type type = core::type::INT32;
    utest_data::generator::image::generate_random_image(image, type);

    image2 = data::object::copy(image);
    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

inline static void stress_test_with_type(core::type _type, int _nb_test)
{
    for(int nb = 0 ; nb < _nb_test ; ++nb)
    {
        data::image::sptr image = std::make_shared<data::image>();
        utest_data::generator::image::generate_random_image(image, _type);

        data::image::sptr image2;
        image2 = data::object::copy(image);
        CPPUNIT_ASSERT(*image == *image2);
    }
}

//------------------------------------------------------------------------------

void image_test::stress_test()
{
    core::type type = core::type::INT8;
    stress_test_with_type(type, 10);

    type = core::type::UINT8;
    stress_test_with_type(type, 10);

    type = core::type::INT16;
    stress_test_with_type(type, 10);

    type = core::type::UINT16;
    stress_test_with_type(type, 10);

    type = core::type::INT32;
    stress_test_with_type(type, 10);

    type = core::type::UINT32;
    stress_test_with_type(type, 10);

    type = core::type::INT64;
    stress_test_with_type(type, 10);

    type = core::type::UINT64;
    stress_test_with_type(type, 10);

    type = core::type::FLOAT;
    stress_test_with_type(type, 10);

    type = core::type::DOUBLE;
    stress_test_with_type(type, 10);
}

//------------------------------------------------------------------------------

} // namespace sight::utest_data::ut
