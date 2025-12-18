/************************************************************************
 *
 * Copyright (C) 2017-2025 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/matrix4.hpp>

#include <filter/image/resampler.hpp>

#include <utest_data/generator/image.hpp>
#include <utest_data/image.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::filter::image::resampler")
{
//------------------------------------------------------------------------------

    TEST_CASE("identity")
    {
        const sight::data::image::size_t size = {{32, 32, 32}};

        // TODO: make it work with an anisotropic spacing.
        const sight::data::image::spacing_t spacing         = {0.5, 0.5, 0.5};
        const sight::data::image::origin_t origin           = {0., 0., 0.};
        const sight::data::image::orientation_t orientation = {1, 0, 0, 0, 1, 0, 0, 0, 1};
        const sight::core::type type                        = sight::core::type::INT16;

        auto image_in = std::make_shared<sight::data::image>();

        sight::utest_data::generator::image::generate_image(
            image_in,
            size,
            spacing,
            origin,
            orientation,
            type,
            sight::data::image::gray_scale
        );

        sight::utest_data::generator::image::randomize_image(image_in);

        auto image_out = std::make_shared<sight::data::image>();

        // Identity.
        sight::data::matrix4::sptr id_mat = std::make_shared<sight::data::matrix4>();

        sight::filter::image::resampler::resample(
            sight::data::image::csptr(image_in),
            image_out,
            sight::data::matrix4::csptr(id_mat),
            std::make_tuple(
                image_in->size(),
                image_in->origin(),
                image_in->orientation(),
                image_in->spacing(),
                sight::filter::image::interpolation_t::LINEAR
            )
        );

        CHECK(image_out->size() == size);
        CHECK(image_out->spacing() == spacing);
        CHECK(image_out->type() == type);

        const auto in_dump_lock  = image_in->dump_lock();
        const auto out_dump_lock = image_out->dump_lock();

        for(std::size_t i = 0 ; i < size[0] ; ++i)
        {
            for(std::size_t j = 0 ; j < size[1] ; ++j)
            {
                for(std::size_t k = 0 ; k < size[2] ; ++k)
                {
                    const std::int16_t value_in  = image_in->at<std::int16_t>(i, j, k);
                    const std::int16_t value_out = image_out->at<std::int16_t>(i, j, k);

                    // The image shouldn't change.
                    std::string msg = std::to_string(value_in) + " " + std::to_string(value_out);
                    CHECK_MESSAGE(value_in == value_out, msg);
                }
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("translate")
    {
        // Generate a simple image with a white cube at its center.
        const sight::data::image::size_t size               = {16, 16, 16};
        const sight::data::image::spacing_t spacing         = {1., 1., 1.};
        const sight::data::image::origin_t origin           = {0., 0., 0.};
        const sight::data::image::orientation_t orientation = {0, 1, 0, -1, 0, 0, 0, 0, 1};
        const sight::core::type type                        = sight::core::type::UINT8;

        auto image_in  = std::make_shared<sight::data::image>();
        auto image_out = std::make_shared<sight::data::image>();

        sight::utest_data::generator::image::generate_image(
            image_in,
            size,
            spacing,
            origin,
            orientation,
            type,
            sight::data::image::gray_scale
        );

        std::uint8_t value = 255;

        const auto in_dump_lock = image_in->dump_lock();

        // Draw a tiny 2x2 cube at the center
        image_in->at<std::uint8_t>(7, 7, 7) = value;
        image_in->at<std::uint8_t>(7, 7, 8) = value;
        image_in->at<std::uint8_t>(7, 8, 7) = value;
        image_in->at<std::uint8_t>(7, 8, 8) = value;
        image_in->at<std::uint8_t>(8, 7, 7) = value;
        image_in->at<std::uint8_t>(8, 7, 8) = value;
        image_in->at<std::uint8_t>(8, 8, 7) = value;
        image_in->at<std::uint8_t>(8, 8, 8) = value;

        // 5 mm translation along the x axis.
        sight::data::matrix4::sptr trans_mat = std::make_shared<sight::data::matrix4>();
        (*trans_mat)(0, 3) = 5;

        sight::filter::image::resampler::resample(
            sight::data::image::csptr(image_in),
            image_out,
            sight::data::matrix4::csptr(trans_mat)
        );

        const auto dump_lock = image_out->dump_lock();

        for(std::size_t i = 0 ; i < size[0] ; ++i)
        {
            for(std::size_t j = 0 ; j < size[1] ; ++j)
            {
                for(std::size_t k = 0 ; k < size[2] ; ++k)
                {
                    const uint8_t value_out = image_out->at<std::uint8_t>(i, j, k);

                    if((j >= 2 && j <= 3) && (i >= 7 && i <= 8) && (k >= 7 && k <= 8))
                    {
                        CHECK_EQ(static_cast<std::uint8_t>(255), value_out);
                    }
                    else
                    {
                        CHECK_EQ(static_cast<std::uint8_t>(0), value_out);
                    }
                }
            }
        }

        // Check if size and spacing are the same as the input.
        CHECK(image_out->size() == size);
        CHECK(image_out->spacing() == spacing);
    }

//------------------------------------------------------------------------------

    TEST_CASE("rotate")
    {
        // FIXME: compute to appropriate matrix to rotate a face from negative Z to negative X.
        // It was disabled before conversion to doctest
        const sight::data::image::size_t size               = {8, 8, 8};
        const sight::data::image::spacing_t spacing         = {0.25, 0.25, 0.25};
        const sight::data::image::origin_t origin           = {0., 0., 0.};
        const sight::data::image::orientation_t orientation = {1., 0., 0., 0., 1., 0., 0., 0., 1.};
        const sight::core::type type                        = sight::core::type::FLOAT;

        auto image_in  = std::make_shared<sight::data::image>();
        auto image_out = std::make_shared<sight::data::image>();

        sight::utest_data::generator::image::generate_image(
            image_in,
            size,
            spacing,
            origin,
            orientation,
            type,
            sight::data::image::gray_scale
        );

        const float value = 1.F;

        const auto dump_lock = image_in->dump_lock();

        // draw the back Z face.
        for(std::size_t i = 0 ; i < size[0] ; ++i)
        {
            for(std::size_t j = 0 ; j < size[1] ; ++j)
            {
                image_in->at<float>(i, j, 0) = value;
            }
        }

        sight::data::matrix4::sptr rot_mat = std::make_shared<sight::data::matrix4>();
        // 90° rotation along the Y axis.
        (*rot_mat)(0, 0) = 0;
        (*rot_mat)(0, 2) = 1;
        (*rot_mat)(2, 0) = -1;
        (*rot_mat)(2, 2) = 0;

        sight::filter::image::resampler::resample(
            sight::data::image::csptr(image_in),
            image_out,
            sight::data::matrix4::csptr(rot_mat)
        );

        const auto out_dump_lock = image_out->dump_lock();

        sight::utest_data::image::SIGHT_UTEST_DATA_IMAGE_DEBUG = true;
        sight::utest_data::image::cout_debug(*image_in, std::map<float, std::string> {{0.F, "O"}, {1.F, "X"}});
        sight::utest_data::image::cout_debug(*image_out, std::map<float, std::string> {{0.F, "O"}, {1.F, "X"}});

        for(std::size_t i = 0 ; i < size[0] ; ++i)
        {
            for(std::size_t j = 0 ; j < size[1] ; ++j)
            {
                for(std::size_t k = 0 ; k < size[2] ; ++k)
                {
                    const float value_out = image_out->at<float>(i, j, k);

                    const std::string msg = std::to_string(i) + " " + std::to_string(j) + " " + std::to_string(k);

                    if(i == 0)
                    {
                        // The negative Z face must be 'white'.
                        CHECK_MESSAGE(1.F == value_out, msg);
                    }
                    else
                    {
                        // Everything else should be 'black'.
                        CHECK_MESSAGE(0.F == value_out, msg);
                    }
                }
            }
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::filter::image::resampler")
