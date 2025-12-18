/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include <viz/scene3d/helper/image.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::viz::scene3d::image")
{
    TEST_CASE("compute_bounding_box_from_mask")
    {
        sight::data::image::sptr mask = std::make_shared<sight::data::image>();

        const sight::core::type type          = sight::core::type::UINT8;
        const sight::data::image::size_t size = {10, 20, 30};

        mask->resize(size, type, sight::data::image::pixel_format_t::gray_scale);

        const auto lock = mask->dump_lock();
        std::fill(mask->begin(), mask->end(), std::uint8_t(0));

        const auto index = [&size](std::size_t _x, std::size_t _y, std::size_t _z)
                           {
                               return _x + _y * size[0] + _z * size[0] * size[1];
                           };

        mask->at<std::uint8_t>(index(2, 4, 4))   = std::uint8_t(255);
        mask->at<std::uint8_t>(index(2, 3, 23))  = std::uint8_t(255);
        mask->at<std::uint8_t>(index(7, 6, 7))   = std::uint8_t(255);
        mask->at<std::uint8_t>(index(8, 19, 22)) = std::uint8_t(255);

        auto clipping_box = sight::viz::scene3d::helper::image::compute_bounding_box_from_mask(mask);

        CHECK_EQ(2.F / static_cast<float>(size[0]), clipping_box.getMinimum()[0]);
        CHECK_EQ(3.F / static_cast<float>(size[1]), clipping_box.getMinimum()[1]);
        CHECK_EQ(4.F / static_cast<float>(size[2]), clipping_box.getMinimum()[2]);

        CHECK_EQ(8.F / static_cast<float>(size[0]), clipping_box.getMaximum()[0]);
        CHECK_EQ(19.F / static_cast<float>(size[1]), clipping_box.getMaximum()[1]);
        CHECK_EQ(23.F / static_cast<float>(size[2]), clipping_box.getMaximum()[2]);
    }
}
