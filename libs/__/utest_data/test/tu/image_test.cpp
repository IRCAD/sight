/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

void image_test::generator3DTest()
{
    data::image::sptr image            = std::make_shared<data::image>();
    const data::image::Size size       = {10, 15, 23};
    const data::image::Spacing spacing = {0.85, 2.6, 1.87};
    const data::image::Origin origin   = {-45.6, 25.97, -53.9};
    core::type type                    = core::type::INT32;

    utest_data::generator::image::generateImage(
        image,
        size,
        spacing,
        origin,
        type,
        data::image::PixelFormat::GRAY_SCALE
    );

    CPPUNIT_ASSERT(image->size() == size);
    CPPUNIT_ASSERT(image->getSpacing() == spacing);
    CPPUNIT_ASSERT(image->getOrigin() == origin);
    CPPUNIT_ASSERT_EQUAL(type, image->getType());
}

//------------------------------------------------------------------------------

void image_test::generator2DTest()
{
    data::image::sptr image            = std::make_shared<data::image>();
    const data::image::Size size       = {10, 15, 0};
    const data::image::Spacing spacing = {0.85, 2.6, 0.};
    const data::image::Origin origin   = {-45.6, 25.97, 0.};
    core::type type                    = core::type::INT16;

    utest_data::generator::image::generateImage(
        image,
        size,
        spacing,
        origin,
        type,
        data::image::PixelFormat::GRAY_SCALE
    );
    CPPUNIT_ASSERT(image->size() == size);
    CPPUNIT_ASSERT(image->getSpacing() == spacing);
    CPPUNIT_ASSERT(image->getOrigin() == origin);
    CPPUNIT_ASSERT_EQUAL(type, image->getType());
}

//------------------------------------------------------------------------------

void image_test::deep_copyTest()
{
    data::image::sptr image = std::make_shared<data::image>();
    data::image::sptr image2;
    core::type type = core::type::INT32;
    utest_data::generator::image::generateRandomImage(image, type);

    image2 = data::object::copy(image);
    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

inline static void stressTestWithType(core::type type, int nbTest)
{
    for(int nb = 0 ; nb < nbTest ; ++nb)
    {
        data::image::sptr image = std::make_shared<data::image>();
        utest_data::generator::image::generateRandomImage(image, type);

        data::image::sptr image2;
        image2 = data::object::copy(image);
        CPPUNIT_ASSERT(*image == *image2);
    }
}

//------------------------------------------------------------------------------

void image_test::stressTest()
{
    core::type type = core::type::INT8;
    stressTestWithType(type, 10);

    type = core::type::UINT8;
    stressTestWithType(type, 10);

    type = core::type::INT16;
    stressTestWithType(type, 10);

    type = core::type::UINT16;
    stressTestWithType(type, 10);

    type = core::type::INT32;
    stressTestWithType(type, 10);

    type = core::type::UINT32;
    stressTestWithType(type, 10);

    type = core::type::INT64;
    stressTestWithType(type, 10);

    type = core::type::UINT64;
    stressTestWithType(type, 10);

    type = core::type::FLOAT;
    stressTestWithType(type, 10);

    type = core::type::DOUBLE;
    stressTestWithType(type, 10);
}

//------------------------------------------------------------------------------

} // namespace sight::utest_data::ut
