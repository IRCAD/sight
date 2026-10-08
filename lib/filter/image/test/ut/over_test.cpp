/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <data/image.hpp>

#include <filter/image/over.hpp>

#include <doctest/doctest.h>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <vector>

//------------------------------------------------------------------------------

template<class PIXEL_TYPE>
static sight::data::image::sptr make_image(const std::vector<PIXEL_TYPE>& _pixels)
{
    auto image = std::make_shared<sight::data::image>();
    image->resize(
        {_pixels.size(), 1, 1},
        sight::core::type::get<PIXEL_TYPE>(),
        sight::data::image::pixel_format_t::gray_scale
    );
    image->set_spacing({1., 1., 1.});
    image->set_origin({0., 0., 0.});
    image->set_orientation({1., 0., 0., 0., 1., 0., 0., 0., 1.});

    const auto dump_lock = image->dump_lock();
    std::copy(_pixels.begin(), _pixels.end(), image->begin<PIXEL_TYPE>());
    return image;
}

//------------------------------------------------------------------------------

template<class PIXEL_TYPE>
static void check_pixels(const sight::data::image& _image, const std::vector<PIXEL_TYPE>& _expected)
{
    const auto dump_lock = _image.dump_lock();
    CHECK_EQ(_image.size()[0], _expected.size());
    CHECK(std::equal(_expected.begin(), _expected.end(), _image.begin<PIXEL_TYPE>()));
}

TEST_SUITE("sight::filter::image::over")
{
//-----------------------------------------------------------------------------

    TEST_CASE("ordered_nonzero_overlay")
    {
        const auto first  = make_image<std::uint8_t>({1, 2, 3, 0});
        const auto second = make_image<std::uint8_t>({0, 4, 0, 0});
        const auto third  = make_image<std::uint8_t>({5, 0, 6, 7});
        auto output       = std::make_shared<sight::data::image>();

        sight::filter::image::over({first, second, third}, *output);

        check_pixels(*output, std::vector<std::uint8_t> {5, 4, 6, 7});
    }

//-----------------------------------------------------------------------------

    TEST_CASE("single_input_is_copied")
    {
        const auto input = make_image<std::uint8_t>({1, 2, 3});
        auto output      = std::make_shared<sight::data::image>();

        sight::filter::image::over({input}, *output);

        check_pixels(*output, std::vector<std::uint8_t> {1, 2, 3});
        {
            const auto input_lock  = input->dump_lock();
            const auto output_lock = output->dump_lock();
            CHECK_NE(output->buffer(), input->buffer());
        }
    }

//-----------------------------------------------------------------------------

    TEST_CASE("reject_empty_inputs")
    {
        auto output = std::make_shared<sight::data::image>();
        const std::vector<sight::data::image::csptr> images;

        CHECK_THROWS(sight::filter::image::over(images, *output));
    }

//-----------------------------------------------------------------------------

    TEST_CASE("reject_incompatible_inputs")
    {
        const auto reference  = make_image<std::uint8_t>({1, 2, 3});
        const auto wrong_size = make_image<std::uint8_t>({1, 2});
        const auto wrong_type = make_image<std::uint16_t>({1, 2, 3});
        auto wrong_spacing    = make_image<std::uint8_t>({1, 2, 3});
        auto output           = std::make_shared<sight::data::image>();
        wrong_spacing->set_spacing({2., 1., 1.});

        CHECK_THROWS(sight::filter::image::over({reference, wrong_size}, *output));
        CHECK_THROWS(sight::filter::image::over({reference, wrong_type}, *output));
        CHECK_THROWS(sight::filter::image::over({reference, wrong_spacing}, *output));
    }

//-----------------------------------------------------------------------------
} // TEST_SUITE("sight::filter::image::over")
