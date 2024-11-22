/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "flip_test.hpp"

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <filter/image/flipper.hpp>

#include <utest_data/generator/image.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::flip_test);

namespace sight::filter::image::ut
{

//------------------------------------------------------------------------------

void flip_test::flip_along_x_axis_test()
{
    const data::image::size_t size                = {{3, 3, 3}};
    const data::image::spacing_t spacing          = {{0.1, 0.4, 1.6}};
    const data::image::origin_t origin            = {{0., 0., 0.}};
    const core::type type                         = core::type::UINT8;
    const enum data::image::pixel_format_t format = data::image::gray_scale;
    std::array<bool, 3> flip_axes {true, false, false};

    data::image::sptr image_in  = std::make_shared<data::image>();
    data::image::sptr image_out = std::make_shared<data::image>();

    utest_data::generator::image::generate_image(image_in, size, spacing, origin, type, format, 0);

    const auto in_dump_lock = image_in->dump_lock();
    filter::image::flipper::flip(image_in, image_out, flip_axes);
    const auto out_dump_lock = image_out->dump_lock();

    for(std::size_t i = 0 ; i < size[0] ; ++i)
    {
        for(std::size_t j = 0 ; j < size[1] ; ++j)
        {
            for(std::size_t k = 0 ; k < size[2] ; ++k)
            {
                const uint8_t value_in  = image_in->at<std::uint8_t>(i, j, k);
                const uint8_t value_out = image_out->at<std::uint8_t>(size[0] - i - 1, j, k);

                // Static cast to get proper printing of the value (in int and not char) on stdout
                CPPUNIT_ASSERT_EQUAL(static_cast<int>(value_in), static_cast<int>(value_out));
            }
        }
    }
}

//------------------------------------------------------------------------------

void flip_test::flip_along_y_axis_test()
{
    const data::image::size_t size                = {{3, 3, 3}};
    const data::image::spacing_t spacing          = {{0.5, 0.5, 0.5}};
    const data::image::origin_t origin            = {{8., 4., 2.}};
    const core::type type                         = core::type::UINT8;
    const enum data::image::pixel_format_t format = data::image::gray_scale;
    std::array<bool, 3> flip_axes {false, true, false};

    data::image::sptr image_in  = std::make_shared<data::image>();
    data::image::sptr image_out = std::make_shared<data::image>();

    utest_data::generator::image::generate_image(image_in, size, spacing, origin, type, format, 0);

    const auto in_dump_lock = image_in->dump_lock();
    filter::image::flipper::flip(image_in, image_out, flip_axes);
    const auto out_dump_lock = image_out->dump_lock();

    for(std::size_t i = 0 ; i < size[0] ; ++i)
    {
        for(std::size_t j = 0 ; j < size[1] ; ++j)
        {
            for(std::size_t k = 0 ; k < size[2] ; ++k)
            {
                const uint8_t value_in  = image_in->at<std::uint8_t>(i, j, k);
                const uint8_t value_out = image_out->at<std::uint8_t>(i, size[1] - j - 1, k);

                // Static cast to get proper printing of the value (in int and not char) on stdout
                CPPUNIT_ASSERT_EQUAL(static_cast<int>(value_in), static_cast<int>(value_out));
            }
        }
    }
}

//------------------------------------------------------------------------------

void flip_test::flip_along_z_axis_test()
{
    const data::image::size_t size                = {{3, 3, 3}};
    const data::image::spacing_t spacing          = {{2.0, 2.0, 2.0}};
    const data::image::origin_t origin            = {{0., 0., 0.}};
    const core::type type                         = core::type::UINT8;
    const enum data::image::pixel_format_t format = data::image::gray_scale;
    std::array<bool, 3> flip_axes {false, false, true};

    data::image::sptr image_in  = std::make_shared<data::image>();
    data::image::sptr image_out = std::make_shared<data::image>();

    utest_data::generator::image::generate_image(image_in, size, spacing, origin, type, format, 0);

    const auto in_dump_lock = image_in->dump_lock();
    filter::image::flipper::flip(image_in, image_out, flip_axes);
    const auto out_dump_lock = image_out->dump_lock();

    for(std::size_t i = 0 ; i < size[0] ; ++i)
    {
        for(std::size_t j = 0 ; j < size[1] ; ++j)
        {
            for(std::size_t k = 0 ; k < size[2] ; ++k)
            {
                const uint8_t value_in  = image_in->at<std::uint8_t>(i, j, k);
                const uint8_t value_out = image_out->at<std::uint8_t>(i, j, size[2] - k - 1);

                // Static cast to get proper printing of the value (in int and not char) on stdout
                CPPUNIT_ASSERT_EQUAL(static_cast<int>(value_in), static_cast<int>(value_out));
            }
        }
    }
}

//------------------------------------------------------------------------------

void flip_test::flip_along_multiple_axes_test()
{
    {
        const data::image::size_t size                = {{3, 3, 3}};
        const data::image::spacing_t spacing          = {{0.5, 0.5, 0.5}};
        const data::image::origin_t origin            = {{0., 0., 0.}};
        const core::type type                         = core::type::UINT8;
        const enum data::image::pixel_format_t format = data::image::gray_scale;
        std::array<bool, 3> flip_axes {true, true, false};

        data::image::sptr image_in  = std::make_shared<data::image>();
        data::image::sptr image_out = std::make_shared<data::image>();

        utest_data::generator::image::generate_image(image_in, size, spacing, origin, type, format, 0);

        const auto in_dump_lock = image_in->dump_lock();
        filter::image::flipper::flip(image_in, image_out, flip_axes);
        const auto out_dump_lock = image_out->dump_lock();

        for(std::size_t i = 0 ; i < size[0] ; ++i)
        {
            for(std::size_t j = 0 ; j < size[1] ; ++j)
            {
                for(std::size_t k = 0 ; k < size[2] ; ++k)
                {
                    const uint8_t value_in  = image_in->at<std::uint8_t>(i, j, k);
                    const uint8_t value_out = image_out->at<std::uint8_t>(size[0] - i - 1, size[1] - j - 1, k);

                    // Static cast to get proper printing of the value (in int and not char) on stdout
                    CPPUNIT_ASSERT_EQUAL(static_cast<int>(value_in), static_cast<int>(value_out));
                }
            }
        }
    }

    {
        const data::image::size_t size                = {{3, 3, 3}};
        const data::image::spacing_t spacing          = {{0.5, 0.5, 0.5}};
        const data::image::origin_t origin            = {{0., 0., 0.}};
        const core::type type                         = core::type::UINT8;
        const enum data::image::pixel_format_t format = data::image::gray_scale;
        std::array<bool, 3> flip_axes {true, true, true};

        data::image::sptr image_in  = std::make_shared<data::image>();
        data::image::sptr image_out = std::make_shared<data::image>();

        utest_data::generator::image::generate_image(image_in, size, spacing, origin, type, format, 0);

        const auto in_dump_lock = image_in->dump_lock();
        filter::image::flipper::flip(image_in, image_out, flip_axes);
        const auto out_dump_lock = image_out->dump_lock();

        for(std::size_t i = 0 ; i < size[0] ; ++i)
        {
            for(std::size_t j = 0 ; j < size[1] ; ++j)
            {
                for(std::size_t k = 0 ; k < size[2] ; ++k)
                {
                    const uint8_t value_in  = image_in->at<std::uint8_t>(i, j, k);
                    const uint8_t value_out = image_out->at<std::uint8_t>(
                        size[0] - i - 1,
                        size[1] - j - 1,
                        size[2] - k - 1
                    );

                    // Static cast to get proper printing of the value (in int and not char) on stdout
                    CPPUNIT_ASSERT_EQUAL(static_cast<int>(value_in), static_cast<int>(value_out));
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void flip_test::flip_empty_image_test()
{
    const data::image::size_t size                = {0, 0, 0};
    const data::image::spacing_t spacing          = {0., 0., 0.};
    const data::image::origin_t origin            = {0., 0., 0.};
    const core::type type                         = core::type::UINT8;
    const enum data::image::pixel_format_t format = data::image::gray_scale;
    std::array<bool, 3> flip_axes {false, true, false};

    data::image::sptr image_in  = std::make_shared<data::image>();
    data::image::sptr image_out = std::make_shared<data::image>();

    utest_data::generator::image::generate_image(image_in, size, spacing, origin, type, format, 0);

    const auto in_dump_lock = image_in->dump_lock();
    filter::image::flipper::flip(image_in, image_out, flip_axes);
    const auto out_dump_lock = image_out->dump_lock();

    const data::image::size_t image_size     = image_in->size();
    const data::image::size_t image_out_size = image_out->size();

    CPPUNIT_ASSERT_EQUAL(image_size[0], image_out_size[0]);
    CPPUNIT_ASSERT_EQUAL(image_size[1], image_out_size[1]);
    CPPUNIT_ASSERT_EQUAL(image_size[2], image_out_size[2]);
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image::ut
