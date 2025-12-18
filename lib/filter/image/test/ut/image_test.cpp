/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <core/type.hpp>

#include <filter/image/image.hpp>

#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

#include <sstream>

//------------------------------------------------------------------------------

static void test_roi_apply(const sight::core::type _image_type, const sight::core::type _roi_type)
{
    const std::string message = "Test failed with types : img : " + _image_type.name() + ", roi : " + _roi_type.name();

    sight::data::image::sptr image_ref;
    auto img_roi_applied = std::make_shared<sight::data::image>();
    auto roi             = std::make_shared<sight::data::image>();

    // generate a random image
    sight::utest_data::generator::image::generate_random_image(img_roi_applied, _image_type);

    const sight::data::image::size_t size               = img_roi_applied->size();
    const sight::data::image::spacing_t spacing         = img_roi_applied->spacing();
    const sight::data::image::origin_t origin           = img_roi_applied->origin();
    const sight::data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};

    sight::utest_data::generator::image::generate_image(
        roi,
        size,
        spacing,
        origin,
        orientation,
        _roi_type,
        sight::data::image::pixel_format_t::gray_scale
    );

    image_ref = sight::data::object::copy(img_roi_applied);

    // fill roi with random values
    sight::utest_data::generator::image::randomize_image(roi);

    const auto dump_lock = roi->dump_lock();

    auto begin      = roi->begin();
    const auto end  = roi->end();
    const auto part = std::int64_t((end - begin) / 3);

    // keep random values in 1/3 of the image (else fill with 0)
    std::fill(begin, begin + part, std::int8_t(0));
    std::fill(end - part, end, std::int8_t(0));

    // apply roi and check
    sight::filter::image::apply_roi(img_roi_applied, roi);

    CHECK_MESSAGE(
        sight::filter::image::is_roi_applied(image_ref, roi, img_roi_applied),
        message
    );
}

TEST_SUITE("sight::filter::image::image")
{
//------------------------------------------------------------------------------

    TEST_CASE("roi_apply")
    {
        test_roi_apply(sight::core::type::INT8, sight::core::type::INT8);
        test_roi_apply(sight::core::type::INT8, sight::core::type::INT16);
        test_roi_apply(sight::core::type::INT8, sight::core::type::INT32);
        test_roi_apply(sight::core::type::INT8, sight::core::type::UINT8);
        test_roi_apply(sight::core::type::INT8, sight::core::type::UINT16);
        test_roi_apply(sight::core::type::INT8, sight::core::type::UINT32);

        test_roi_apply(sight::core::type::INT16, sight::core::type::INT8);
        test_roi_apply(sight::core::type::INT16, sight::core::type::INT16);
        test_roi_apply(sight::core::type::INT16, sight::core::type::INT32);
        test_roi_apply(sight::core::type::INT16, sight::core::type::UINT8);
        test_roi_apply(sight::core::type::INT16, sight::core::type::UINT16);
        test_roi_apply(sight::core::type::INT16, sight::core::type::UINT32);

        test_roi_apply(sight::core::type::INT32, sight::core::type::INT8);
        test_roi_apply(sight::core::type::INT32, sight::core::type::INT16);
        test_roi_apply(sight::core::type::INT32, sight::core::type::INT32);
        test_roi_apply(sight::core::type::INT32, sight::core::type::UINT8);
        test_roi_apply(sight::core::type::INT32, sight::core::type::UINT16);
        test_roi_apply(sight::core::type::INT32, sight::core::type::UINT32);

        test_roi_apply(sight::core::type::UINT8, sight::core::type::INT8);
        test_roi_apply(sight::core::type::UINT8, sight::core::type::INT16);
        test_roi_apply(sight::core::type::UINT8, sight::core::type::INT32);
        test_roi_apply(sight::core::type::UINT8, sight::core::type::UINT8);
        test_roi_apply(sight::core::type::UINT8, sight::core::type::UINT16);
        test_roi_apply(sight::core::type::UINT8, sight::core::type::UINT32);

        test_roi_apply(sight::core::type::UINT16, sight::core::type::INT8);
        test_roi_apply(sight::core::type::UINT16, sight::core::type::INT16);
        test_roi_apply(sight::core::type::UINT16, sight::core::type::INT32);
        test_roi_apply(sight::core::type::UINT16, sight::core::type::UINT8);
        test_roi_apply(sight::core::type::UINT16, sight::core::type::UINT16);
        test_roi_apply(sight::core::type::UINT16, sight::core::type::UINT32);

        test_roi_apply(sight::core::type::UINT32, sight::core::type::INT8);
        test_roi_apply(sight::core::type::UINT32, sight::core::type::INT16);
        test_roi_apply(sight::core::type::UINT32, sight::core::type::INT32);
        test_roi_apply(sight::core::type::UINT32, sight::core::type::UINT8);
        test_roi_apply(sight::core::type::UINT32, sight::core::type::UINT16);
        test_roi_apply(sight::core::type::UINT32, sight::core::type::UINT32);

        // float and double disabled : randomization generate some <nan>
        // test_roi_apply(sight:: core::type::INT8, sight::core::type::FLOAT );
        // test_roi_apply(sight:: core::type::INT16 , sight::core::type::FLOAT );
        // test_roi_apply(sight:: core::type::INT32 , sight::core::type::FLOAT );
        // test_roi_apply(sight:: core::type::UINT8 , sight::core::type::FLOAT );
        // test_roi_apply(sight:: core::type::UINT16, sight::core::type::FLOAT );
        // test_roi_apply(sight:: core::type::UINT32, sight::core::type::FLOAT );
        // test_roi_apply(sight:: core::type::FLOAT , sight::core::type::INT8 );
        // test_roi_apply(sight:: core::type::FLOAT , sight::core::type::INT16 );
        // test_roi_apply(sight:: core::type::FLOAT , sight::core::type::INT32 );
        // test_roi_apply(sight:: core::type::FLOAT , sight::core::type::UINT8 );
        // test_roi_apply(sight:: core::type::FLOAT , sight::core::type::UINT16);
        // test_roi_apply(sight:: core::type::FLOAT , sight::core::type::UINT32);
        // test_roi_apply(sight:: core::type::FLOAT , sight::core::type::FLOAT );
        //
        //#ifndef DEBUG // double disabled from default dispatcher type list in debug mode
        // test_roi_apply(sight:: core::type::INT8, sight::core::type::DOUBLE);
        // test_roi_apply(sight:: core::type::INT16 , sight::core::type::DOUBLE);
        // test_roi_apply(sight:: core::type::INT32 , sight::core::type::DOUBLE);
        // test_roi_apply(sight:: core::type::UINT8 , sight::core::type::DOUBLE);
        // test_roi_apply(sight:: core::type::UINT16, sight::core::type::DOUBLE);
        // test_roi_apply(sight:: core::type::UINT32, sight::core::type::DOUBLE);
        // test_roi_apply(sight:: core::type::FLOAT , sight::core::type::DOUBLE);
        // test_roi_apply(sight:: core::type::DOUBLE, sight::core::type::INT8 );
        // test_roi_apply(sight:: core::type::DOUBLE, sight::core::type::INT16 );
        // test_roi_apply(sight:: core::type::DOUBLE, sight::core::type::INT32 );
        // test_roi_apply(sight:: core::type::DOUBLE, sight::core::type::UINT8 );
        // test_roi_apply(sight:: core::type::DOUBLE, sight::core::type::UINT16);
        // test_roi_apply(sight:: core::type::DOUBLE, sight::core::type::UINT32);
        // test_roi_apply(sight:: core::type::DOUBLE, sight::core::type::FLOAT );
        // test_roi_apply(sight:: core::type::DOUBLE, sight::core::type::DOUBLE);
        //#endif

        // 64 bit type not supported by dynamic_t/Dispatcher
        // test_roi_apply(sight:: core::type::UINT64, sight::core::type::DOUBLE);
        // test_roi_apply(sight:: core::type::INT64 , sight::core::type::DOUBLE);
        // test_roi_apply(sight:: core::type::DOUBLE, sight::core::type::INT64 );
        // test_roi_apply(sight:: core::type::DOUBLE, sight::core::type::UINT64);
        //
        // test_roi_apply(sight:: core::type::INT64 , sight::core::type::FLOAT );
        // test_roi_apply(sight:: core::type::UINT64, sight::core::type::FLOAT );
        // test_roi_apply(sight:: core::type::FLOAT , sight::core::type::INT64 );
        // test_roi_apply(sight:: core::type::FLOAT , sight::core::type::UINT64);
        //
        // test_roi_apply(sight:: core::type::INT8, sight::core::type::INT64 );
        // test_roi_apply(sight:: core::type::INT8, sight::core::type::UINT64);
        // test_roi_apply(sight:: core::type::INT16 , sight::core::type::INT64 );
        // test_roi_apply(sight:: core::type::INT16 , sight::core::type::UINT64);
        // test_roi_apply(sight:: core::type::INT32 , sight::core::type::INT64 );
        // test_roi_apply(sight:: core::type::INT32 , sight::core::type::UINT64);
        // test_roi_apply(sight:: core::type::INT64 , sight::core::type::INT8 );
        // test_roi_apply(sight:: core::type::INT64 , sight::core::type::INT16 );
        // test_roi_apply(sight:: core::type::INT64 , sight::core::type::INT32 );
        // test_roi_apply(sight:: core::type::INT64 , sight::core::type::INT64 );
        // test_roi_apply(sight:: core::type::INT64 , sight::core::type::UINT8 );
        // test_roi_apply(sight:: core::type::INT64 , sight::core::type::UINT16);
        // test_roi_apply(sight:: core::type::INT64 , sight::core::type::UINT32);
        // test_roi_apply(sight:: core::type::INT64 , sight::core::type::UINT64);
        // test_roi_apply(sight:: core::type::UINT8 , sight::core::type::INT64 );
        // test_roi_apply(sight:: core::type::UINT8 , sight::core::type::UINT64);
        // test_roi_apply(sight:: core::type::UINT16, sight::core::type::INT64 );
        // test_roi_apply(sight:: core::type::UINT16, sight::core::type::UINT64);
        // test_roi_apply(sight:: core::type::UINT32, sight::core::type::INT64 );
        // test_roi_apply(sight:: core::type::UINT32, sight::core::type::UINT64);
        // test_roi_apply(sight:: core::type::UINT64, sight::core::type::INT8 );
        // test_roi_apply(sight:: core::type::UINT64, sight::core::type::INT16 );
        // test_roi_apply(sight:: core::type::UINT64, sight::core::type::INT32 );
        // test_roi_apply(sight:: core::type::UINT64, sight::core::type::INT64 );
        // test_roi_apply(sight:: core::type::UINT64, sight::core::type::UINT8 );
        // test_roi_apply(sight:: core::type::UINT64, sight::core::type::UINT16);
        // test_roi_apply(sight:: core::type::UINT64, sight::core::type::UINT32);
        // test_roi_apply(sight:: core::type::UINT64, sight::core::type::UINT64);
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::filter::image::image")
