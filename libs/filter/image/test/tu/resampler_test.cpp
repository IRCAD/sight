/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "resampler_test.hpp"

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/matrix4.hpp>

#include <filter/image/resampler.hpp>

#include <utest_data/generator/image.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::resampler_test);

namespace sight::filter::image::ut
{

//------------------------------------------------------------------------------

void resampler_test::setUp()
{
}

//------------------------------------------------------------------------------

void resampler_test::tearDown()
{
}

//------------------------------------------------------------------------------

void resampler_test::identityTest()
{
    const data::image::Size size = {{32, 32, 32}};

    // TODO: make it work with an anisotropic spacing.
    const data::image::Spacing spacing = {{0.5, 0.5, 0.5}};
    const data::image::Origin origin   = {{0., 0., 0.}};
    const core::type type              = core::type::INT16;

    data::image::sptr image_in = std::make_shared<data::image>();

    utest_data::generator::image::generateImage(image_in, size, spacing, origin, type, data::image::GRAY_SCALE);
    utest_data::generator::image::randomizeImage(image_in);

    data::image::sptr image_out = std::make_shared<data::image>();

    // Identity.
    data::matrix4::sptr id_mat = std::make_shared<data::matrix4>();

    filter::image::resampler::resample(
        data::image::csptr(image_in),
        image_out,
        data::matrix4::csptr(id_mat),
        std::make_tuple(image_in->size(), image_in->getOrigin(), image_in->getSpacing())
    );

    CPPUNIT_ASSERT(image_out->size() == size);
    CPPUNIT_ASSERT(image_out->getSpacing() == spacing);
    CPPUNIT_ASSERT(image_out->getType() == type);

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
                CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, value_in, value_out);
            }
        }
    }
}

//------------------------------------------------------------------------------

void resampler_test::translateTest()
{
    // Generate a simple image with a white cube at its center.
    const data::image::Size size       = {{16, 16, 16}};
    const data::image::Spacing spacing = {{1., 1., 1.}};
    const data::image::Origin origin   = {{0., 0., 0.}};
    const core::type type              = core::type::UINT8;

    data::image::sptr image_in  = std::make_shared<data::image>();
    data::image::sptr image_out = std::make_shared<data::image>();

    utest_data::generator::image::generateImage(image_in, size, spacing, origin, type, data::image::GRAY_SCALE);

    std::uint8_t value = 255;

    SPTR(data::image::buffer_t) buffer_value =
        data::helper::medical_image::get_pixel_in_image_space(image_in, value);

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
    data::matrix4::sptr trans_mat = std::make_shared<data::matrix4>();
    (*trans_mat)(0, 3) = 5;

    filter::image::resampler::resample(
        data::image::csptr(image_in),
        image_out,
        data::matrix4::csptr(trans_mat)
    );

    const auto dump_lock = image_out->dump_lock();

    for(std::size_t i = 0 ; i < size[0] ; ++i)
    {
        for(std::size_t j = 0 ; j < size[1] ; ++j)
        {
            for(std::size_t k = 0 ; k < size[2] ; ++k)
            {
                const uint8_t value_out = image_out->at<std::uint8_t>(i, j, k);

                if((i >= 2 && i <= 3) && (j >= 7 && j <= 8) && (k >= 7 && k <= 8))
                {
                    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(255), value_out);
                }
                else
                {
                    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), value_out);
                }
            }
        }
    }

    // Check if size and spacing are the same as the input.
    CPPUNIT_ASSERT(image_out->size() == size);
    CPPUNIT_ASSERT(image_out->getSpacing() == spacing);
}

//------------------------------------------------------------------------------

void resampler_test::rotateTest()
{
    const data::image::Size size       = {{64, 64, 64}};
    const data::image::Spacing spacing = {{1., 1., 1.}};
    const data::image::Origin origin   = {{0., 0., 0.}};
    const core::type type              = core::type::FLOAT;

    data::image::sptr image_in  = std::make_shared<data::image>();
    data::image::sptr image_out = std::make_shared<data::image>();

    utest_data::generator::image::generateImage(image_in, size, spacing, origin, type, data::image::GRAY_SCALE);

    const float value = 1.F;

    const auto dump_lock = image_in->dump_lock();

    // draw the back Z face.
    for(std::size_t i = 0 ; i < 64 ; ++i)
    {
        for(std::size_t j = 0 ; j < 64 ; ++j)
        {
            image_in->at<float>(i, j, 0) = value;
        }
    }

    // FIXME: compute to appropriate matrix to rotate a face from negative Z to negative X.

    data::matrix4::sptr rot_mat = std::make_shared<data::matrix4>();
    // 90° rotation along the Y axis.
    (*rot_mat)(0, 0) = 0;
    (*rot_mat)(0, 2) = 1;
    (*rot_mat)(2, 0) = -1;
    (*rot_mat)(2, 2) = 0;

    // 32 mm translation along the X axis.
    (*rot_mat)(0, 3) = double(size[0]) / 2.;

    filter::image::resampler::resample(
        data::image::csptr(image_in),
        image_out,
        data::matrix4::csptr(rot_mat)
    );

    const auto out_dump_lock = image_out->dump_lock();

    for(std::size_t i = 0 ; i < size[0] ; ++i)
    {
        for(std::size_t j = 0 ; j < size[1] ; ++j)
        {
            for(std::size_t k = 0 ; k < size[2] ; ++k)
            {
                const float value_out = image_out->at<float>(i, j, k);

                std::string msg = std::to_string(i) + " " + std::to_string(j) + " " + std::to_string(k);

                if(i == 0)
                {
                    // The negative Z face must be 'white'.
                    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 1.F, value_out);
                }
                else
                {
                    // Everything else should be 'black'.
                    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 0.F, value_out);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image::ut
