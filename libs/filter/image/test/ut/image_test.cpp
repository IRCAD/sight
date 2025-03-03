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

#include <filter/image/image.hpp>

#include <utest_data/generator/image.hpp>

#include <sstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::image_test);

namespace sight::filter::image::ut
{

//------------------------------------------------------------------------------

void image_test::setUp()
{
}

//------------------------------------------------------------------------------

void image_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void test_roi_apply(const core::type _image_type, const core::type _roi_type)
{
    const std::string message = "Test failed with types : img : " + _image_type.name()
                                + ", roi : " + _roi_type.name();
    data::image::sptr image_ref;
    data::image::sptr img_roi_applied = std::make_shared<data::image>();
    data::image::sptr roi             = std::make_shared<data::image>();

    // generate a random image
    utest_data::generator::image::generate_random_image(img_roi_applied, _image_type);
    const data::image::size_t size               = img_roi_applied->size();
    const data::image::spacing_t spacing         = img_roi_applied->spacing();
    const data::image::origin_t origin           = img_roi_applied->origin();
    const data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};

    utest_data::generator::image::generate_image(
        roi,
        size,
        spacing,
        origin,
        orientation,
        _roi_type,
        data::image::pixel_format_t::gray_scale
    );

    image_ref = data::object::copy(img_roi_applied);

    // fill roi with random values
    utest_data::generator::image::randomize_image(roi);

    const auto dump_lock = roi->dump_lock();
    auto begin           = roi->begin();
    const auto end       = roi->end();
    const auto part      = std::int64_t((end - begin) / 3);

    // keep random values in 1/3 of the image (else fill with 0)
    std::fill(begin, begin + part, std::int8_t(0));
    std::fill(end - part, end, std::int8_t(0));

    // apply roi and check
    filter::image::apply_roi(img_roi_applied, roi);
    CPPUNIT_ASSERT_MESSAGE(message, filter::image::is_roi_applied(image_ref, roi, img_roi_applied));
}

//------------------------------------------------------------------------------

void image_test::roi_apply_test()
{
    test_roi_apply(core::type::INT8, core::type::INT8);
    test_roi_apply(core::type::INT8, core::type::INT16);
    test_roi_apply(core::type::INT8, core::type::INT32);
    test_roi_apply(core::type::INT8, core::type::UINT8);
    test_roi_apply(core::type::INT8, core::type::UINT16);
    test_roi_apply(core::type::INT8, core::type::UINT32);
    test_roi_apply(core::type::INT16, core::type::INT8);
    test_roi_apply(core::type::INT16, core::type::INT16);
    test_roi_apply(core::type::INT16, core::type::INT32);
    test_roi_apply(core::type::INT16, core::type::UINT8);
    test_roi_apply(core::type::INT16, core::type::UINT16);
    test_roi_apply(core::type::INT16, core::type::UINT32);
    test_roi_apply(core::type::INT32, core::type::INT8);
    test_roi_apply(core::type::INT32, core::type::INT16);
    test_roi_apply(core::type::INT32, core::type::INT32);
    test_roi_apply(core::type::INT32, core::type::UINT8);
    test_roi_apply(core::type::INT32, core::type::UINT16);
    test_roi_apply(core::type::INT32, core::type::UINT32);
    test_roi_apply(core::type::UINT8, core::type::INT8);
    test_roi_apply(core::type::UINT8, core::type::INT16);
    test_roi_apply(core::type::UINT8, core::type::INT32);
    test_roi_apply(core::type::UINT8, core::type::UINT8);
    test_roi_apply(core::type::UINT8, core::type::UINT16);
    test_roi_apply(core::type::UINT8, core::type::UINT32);
    test_roi_apply(core::type::UINT16, core::type::INT8);
    test_roi_apply(core::type::UINT16, core::type::INT16);
    test_roi_apply(core::type::UINT16, core::type::INT32);
    test_roi_apply(core::type::UINT16, core::type::UINT8);
    test_roi_apply(core::type::UINT16, core::type::UINT16);
    test_roi_apply(core::type::UINT16, core::type::UINT32);
    test_roi_apply(core::type::UINT32, core::type::INT8);
    test_roi_apply(core::type::UINT32, core::type::INT16);
    test_roi_apply(core::type::UINT32, core::type::INT32);
    test_roi_apply(core::type::UINT32, core::type::UINT8);
    test_roi_apply(core::type::UINT32, core::type::UINT16);
    test_roi_apply(core::type::UINT32, core::type::UINT32);

// float and double disabled : randomization generate some <nan>

//    TestRoiApply( "int8", "float" );
//    TestRoiApply( "int16" , "float" );
//    TestRoiApply( "int32" , "float" );
//    TestRoiApply( "uint8" , "float" );
//    TestRoiApply( "uint16", "float" );
//    TestRoiApply( "uint32", "float" );
//    TestRoiApply( "float" , "int8"  );
//    TestRoiApply( "float" , "int16" );
//    TestRoiApply( "float" , "int32" );
//    TestRoiApply( "float" , "uint8" );
//    TestRoiApply( "float" , "uint16");
//    TestRoiApply( "float" , "uint32");
//    TestRoiApply( "float" , "float" );
//
//#ifndef DEBUG // double disabled from default dispatcher type list in debug mode
//    TestRoiApply( "int8", "double");
//    TestRoiApply( "int16" , "double");
//    TestRoiApply( "int32" , "double");
//    TestRoiApply( "uint8" , "double");
//    TestRoiApply( "uint16", "double");
//    TestRoiApply( "uint32", "double");
//    TestRoiApply( "float" , "double");
//    TestRoiApply( "double", "int8"  );
//    TestRoiApply( "double", "int16" );
//    TestRoiApply( "double", "int32" );
//    TestRoiApply( "double", "uint8" );
//    TestRoiApply( "double", "uint16");
//    TestRoiApply( "double", "uint32");
//    TestRoiApply( "double", "float" );
//    TestRoiApply( "double", "double");
//#endif

    // 64 bit type not supported by dynamic_t/Dispatcher

    // TestRoiApply( "uint64", "double");
    // TestRoiApply( "int64" , "double");
    // TestRoiApply( "double", "int64" );
    // TestRoiApply( "double", "uint64");
    //
    // TestRoiApply( "int64" , "float" );
    // TestRoiApply( "uint64", "float" );
    // TestRoiApply( "float" , "int64" );
    // TestRoiApply( "float" , "uint64");
    //
    // TestRoiApply( "int8", "int64" );
    // TestRoiApply( "int8", "uint64");
    // TestRoiApply( "int16" , "int64" );
    // TestRoiApply( "int16" , "uint64");
    // TestRoiApply( "int32" , "int64" );
    // TestRoiApply( "int32" , "uint64");
    // TestRoiApply( "int64" , "int8"  );
    // TestRoiApply( "int64" , "int16" );
    // TestRoiApply( "int64" , "int32" );
    // TestRoiApply( "int64" , "int64" );
    // TestRoiApply( "int64" , "uint8" );
    // TestRoiApply( "int64" , "uint16");
    // TestRoiApply( "int64" , "uint32");
    // TestRoiApply( "int64" , "uint64");
    // TestRoiApply( "uint8" , "int64" );
    // TestRoiApply( "uint8" , "uint64");
    // TestRoiApply( "uint16", "int64" );
    // TestRoiApply( "uint16", "uint64");
    // TestRoiApply( "uint32", "int64" );
    // TestRoiApply( "uint32", "uint64");
    // TestRoiApply( "uint64", "int8"  );
    // TestRoiApply( "uint64", "int16" );
    // TestRoiApply( "uint64", "int32" );
    // TestRoiApply( "uint64", "int64" );
    // TestRoiApply( "uint64", "uint8" );
    // TestRoiApply( "uint64", "uint16");
    // TestRoiApply( "uint64", "uint32");
    // TestRoiApply( "uint64", "uint64");
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image::ut
