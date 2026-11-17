/************************************************************************
 *
 * Copyright (C) 2017-2026 IRCAD France
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

#include "cv_common.hpp"

#include <data/array.hpp>

#include <io/opencv/image.hpp>

#include <doctest/doctest.h>

//------------------------------------------------------------------------------

template<typename T>
static sight::data::image::sptr gen_image(
    const std::vector<T>& _image_buffer,
    std::size_t _w,
    std::size_t _h,
    std::size_t _d,
    std::uint8_t _num_channels
)
{
    sight::data::image::sptr image = std::make_shared<sight::data::image>();
    const auto dump_lock           = image->dump_lock();

    SIGHT_ASSERT("Width should be at least 1", _w >= 1);

    const sight::core::type image_type    = sight::core::type::get<T>();
    sight::data::image::size_t image_size = {0, 0, 0};
    image_size[0] = _w;
    if(_h > 0)
    {
        image_size[1] = _h;
    }

    if(_d > 0)
    {
        image_size[2] = _d;
    }

    enum sight::data::image::pixel_format_t format = sight::data::image::pixel_format_t::gray_scale;
    switch(_num_channels)
    {
        case 1:
            format = sight::data::image::pixel_format_t::gray_scale;
            break;

        case 2:
            format = sight::data::image::pixel_format_t::rg;
            break;

        case 3:
            format = sight::data::image::pixel_format_t::rgb;
            break;

        case 4:
            format = sight::data::image::pixel_format_t::rgba;
            break;

        default:
            SIGHT_FATAL("Unhandled OpenCV format");
    }

    image->resize(image_size, image_type, format);

    auto dst_buffer        = image->begin<std::uint8_t>();
    const auto* src_buffer = reinterpret_cast<const std::uint8_t*>(_image_buffer.data());
    std::copy(src_buffer, src_buffer + image->size_in_bytes(), dst_buffer);

    return image;
}

//------------------------------------------------------------------------------

template<typename T>
static void compare_images(
    const cv::Mat& _cv_image,
    const sight::data::image::csptr& _image,
    std::size_t _w,
    std::size_t _h,
    std::size_t _d,
    std::uint8_t _num_channels
)
{
    const auto dump_lock  = _image->dump_lock();
    const T* image_buffer = reinterpret_cast<const T*>(_image->buffer());

    std::vector<cv::Mat> channels(_num_channels);
    cv::split(_cv_image, channels);

    if(_d > 0)
    {
        CHECK_EQ(_image->num_dimensions(), static_cast<std::size_t>(_cv_image.dims));
        CHECK_EQ(_w, static_cast<std::size_t>(_cv_image.size[2]));
        CHECK_EQ(_h, static_cast<std::size_t>(_cv_image.size[1]));
        CHECK_EQ(_d, static_cast<std::size_t>(_cv_image.size[0]));
        for(int k = 0 ; k < _cv_image.size[0] ; ++k)
        {
            for(int j = 0 ; j < _cv_image.size[1] ; ++j)
            {
                for(int i = 0 ; i < _cv_image.size[2] ; ++i)
                {
                    for(std::uint8_t c = 0 ; c < _num_channels ; ++c)
                    {
                        const auto index = c
                                           + static_cast<std::size_t>(i) * _num_channels
                                           + static_cast<std::size_t>(j) * _num_channels * _w
                                           + static_cast<std::size_t>(k) * _num_channels * _w * _h;
                        CHECK_EQ(image_buffer[index], channels[c].at<T>(k, j, i));
                    }
                }
            }
        }
    }
    else if(_h > 0)
    {
        CHECK_EQ(_image->num_dimensions(), static_cast<std::size_t>(_cv_image.dims));
        CHECK_EQ(_w, static_cast<std::size_t>(_cv_image.size[1]));
        CHECK_EQ(_h, static_cast<std::size_t>(_cv_image.size[0]));

        for(int j = 0 ; j < _cv_image.size[0] ; ++j)
        {
            for(int i = 0 ; i < _cv_image.size[1] ; ++i)
            {
                for(std::uint8_t c = 0 ; c < _num_channels ; ++c)
                {
                    const std::size_t index = c
                                              + static_cast<std::size_t>(i) * _num_channels
                                              + static_cast<std::size_t>(j) * _num_channels * _w;
                    CHECK_EQ(image_buffer[index], channels[c].at<T>(j, i));
                }
            }
        }
    }
    else
    {
        CHECK_EQ(2, _cv_image.dims);
        CHECK_EQ(_w, static_cast<std::size_t>(_cv_image.size[1]));
        CHECK_EQ(1, _cv_image.size[0]);

        for(std::size_t i = 0 ; std::cmp_less(i, _cv_image.size[1]) ; ++i)
        {
            for(std::uint8_t c = 0 ; c < _num_channels ; ++c)
            {
                const auto index = c + i * _num_channels;
                CHECK_EQ(image_buffer[index], channels[c].at<T>(static_cast<int>(i)));
            }
        }
    }
}

//------------------------------------------------------------------------------

template<typename T>
static void test_move_to_cv(std::size_t _w, std::size_t _h, std::size_t _d, std::uint8_t _num_channels)
{
    const std::vector<T> image_buffer = sight::io::opencv::ut::gen_image_buffer<T>(_w, _h, _d, _num_channels);
    sight::data::image::sptr image    = gen_image<T>(image_buffer, _w, _h, _d, _num_channels);

    cv::Mat cv_image = sight::io::opencv::image::move_to_cv(image);

    // Since we share the same buffer, compare the pointers
    const auto dump_lock = image->dump_lock();
    CHECK_EQ(image->buffer(), static_cast<void*>(cv_image.data));

    compare_images<T>(cv_image, image, _w, _h, _d, _num_channels);
}

//------------------------------------------------------------------------------

template<typename T>
static void test_copy_from_cv(std::size_t _w, std::size_t _h, std::size_t _d, std::uint8_t _num_channels)
{
    const std::vector<T> image_buffer = sight::io::opencv::ut::gen_image_buffer<T>(_w, _h, _d, _num_channels);
    const cv::Mat cv_image            = sight::io::opencv::ut::gen_cv_image<T>(image_buffer, _w, _h, _d, _num_channels);

    sight::data::image::sptr image = std::make_shared<sight::data::image>();
    sight::io::opencv::image::copy_from_cv(*image.get(), cv_image);

    // Since we copy the buffer, ensure the pointers are different
    const auto dump_lock = image->dump_lock();
    CHECK(image->buffer() != static_cast<void*>(cv_image.data));

    compare_images<T>(cv_image, image, _w, _h, _d, _num_channels);
}

//------------------------------------------------------------------------------

template<typename T>
static void test_copy_to_cv(std::size_t _w, std::size_t _h, std::size_t _d, std::uint8_t _num_channels)
{
    const std::vector<T> image_buffer = sight::io::opencv::ut::gen_image_buffer<T>(_w, _h, _d, _num_channels);
    sight::data::image::csptr image   = gen_image<T>(image_buffer, _w, _h, _d, _num_channels);

    cv::Mat cv_image = sight::io::opencv::image::copy_to_cv(image);

    // Since we copy the buffer, ensure the pointers are different
    const auto dump_lock = image->dump_lock();
    CHECK(image->buffer() != static_cast<void*>(cv_image.data));

    compare_images<T>(cv_image, image, _w, _h, _d, _num_channels);
}

TEST_SUITE("sight::io::opencv::image")
{
//------------------------------------------------------------------------------

    TEST_CASE("move_to_cv")
    {
        test_move_to_cv<std::uint8_t>(10, 2, 30, 1);
        test_move_to_cv<std::uint8_t>(1, 20, 5, 2);
        test_move_to_cv<std::uint8_t>(6, 12, 0, 3);
        test_move_to_cv<std::uint8_t>(10, 7, 30, 4);

        test_move_to_cv<std::int8_t>(10, 0, 0, 1);
        test_move_to_cv<std::int8_t>(10, 20, 0, 2);
        test_move_to_cv<std::int8_t>(23, 20, 30, 3);
        test_move_to_cv<std::int8_t>(76, 2, 0, 4);

        test_move_to_cv<std::uint16_t>(10, 2, 30, 1);
        test_move_to_cv<std::uint16_t>(10, 20, 5, 2);
        test_move_to_cv<std::uint16_t>(6, 20, 0, 3);
        test_move_to_cv<std::uint16_t>(10, 7, 30, 4);

        test_move_to_cv<std::int16_t>(10, 0, 0, 1);
        test_move_to_cv<std::int16_t>(10, 20, 0, 2);
        test_move_to_cv<std::int16_t>(23, 20, 30, 3);
        test_move_to_cv<std::int16_t>(76, 2, 1, 4);

        test_move_to_cv<std::int32_t>(10, 32, 0, 1);
        test_move_to_cv<std::int32_t>(10, 20, 3, 2);
        test_move_to_cv<std::int32_t>(23, 20, 0, 3);
        test_move_to_cv<std::int32_t>(76, 2, 1, 4);

        test_move_to_cv<float>(10, 32, 0, 1);
        test_move_to_cv<float>(10, 20, 3, 2);
        test_move_to_cv<float>(90, 20, 1, 3);
        test_move_to_cv<float>(76, 2, 4, 4);

        test_move_to_cv<double>(12, 32, 0, 1);
        test_move_to_cv<double>(10, 1, 3, 2);
        test_move_to_cv<double>(23, 8, 0, 3);
        test_move_to_cv<double>(76, 2, 4, 4);
    }

//------------------------------------------------------------------------------

    TEST_CASE("copy_from_cv")
    {
        test_copy_from_cv<std::int16_t>(10, 0, 0, 1);
        test_copy_from_cv<std::uint8_t>(10, 2, 30, 1);
        test_copy_from_cv<std::uint8_t>(1, 20, 5, 2);
        test_copy_from_cv<std::uint8_t>(6, 12, 0, 3);
        test_copy_from_cv<std::uint8_t>(10, 7, 30, 4);

        test_copy_from_cv<std::int8_t>(10, 0, 0, 1);
        test_copy_from_cv<std::int8_t>(10, 20, 0, 2);
        test_copy_from_cv<std::int8_t>(23, 20, 30, 3);
        test_copy_from_cv<std::int8_t>(76, 2, 0, 4);

        test_copy_from_cv<std::uint16_t>(10, 1, 1, 1);
        test_copy_from_cv<std::uint16_t>(10, 2, 30, 1);
        test_copy_from_cv<std::uint16_t>(10, 20, 5, 2);
        test_copy_from_cv<std::uint16_t>(6, 20, 0, 3);
        test_copy_from_cv<std::uint16_t>(10, 7, 30, 4);

        test_copy_from_cv<std::int16_t>(10, 0, 0, 1);
        test_copy_from_cv<std::int16_t>(10, 20, 0, 2);
        test_copy_from_cv<std::int16_t>(23, 20, 30, 3);
        test_copy_from_cv<std::int16_t>(76, 2, 0, 4);

        test_copy_from_cv<std::int32_t>(10, 32, 0, 1);
        test_copy_from_cv<std::int32_t>(10, 20, 3, 2);
        test_copy_from_cv<std::int32_t>(23, 20, 0, 3);
        test_copy_from_cv<std::int32_t>(76, 2, 4, 4);

        test_copy_from_cv<float>(10, 32, 0, 1);
        test_copy_from_cv<float>(10, 20, 3, 2);
        test_copy_from_cv<float>(90, 20, 1, 3);
        test_copy_from_cv<float>(76, 2, 4, 4);

        test_copy_from_cv<double>(12, 32, 0, 1);
        test_copy_from_cv<double>(10, 1, 3, 2);
        test_copy_from_cv<double>(23, 8, 0, 3);
        test_copy_from_cv<double>(76, 2, 4, 4);
    }

//------------------------------------------------------------------------------

    TEST_CASE("copy_to_cv")
    {
        test_copy_to_cv<std::uint8_t>(10, 2, 30, 1);
        test_copy_to_cv<std::uint8_t>(1, 20, 5, 2);
        test_copy_to_cv<std::uint8_t>(6, 12, 0, 3);
        test_copy_to_cv<std::uint8_t>(10, 7, 30, 4);

        test_copy_to_cv<std::int8_t>(10, 0, 0, 1);
        test_copy_to_cv<std::int8_t>(10, 20, 0, 2);
        test_copy_to_cv<std::int8_t>(23, 20, 30, 3);
        test_copy_to_cv<std::int8_t>(76, 2, 0, 4);

        test_copy_to_cv<std::uint16_t>(10, 2, 30, 1);
        test_copy_to_cv<std::uint16_t>(10, 20, 5, 2);
        test_copy_to_cv<std::uint16_t>(6, 20, 0, 3);
        test_copy_to_cv<std::uint16_t>(10, 7, 30, 4);

        test_copy_to_cv<std::int16_t>(10, 0, 0, 1);
        test_copy_to_cv<std::int16_t>(10, 20, 0, 2);
        test_copy_to_cv<std::int16_t>(23, 20, 30, 3);
        test_copy_to_cv<std::int16_t>(76, 2, 0, 4);

        test_copy_to_cv<std::int32_t>(10, 32, 0, 1);
        test_copy_to_cv<std::int32_t>(10, 20, 3, 2);
        test_copy_to_cv<std::int32_t>(23, 20, 0, 3);
        test_copy_to_cv<std::int32_t>(76, 2, 4, 4);

        test_copy_to_cv<float>(10, 32, 0, 1);
        test_copy_to_cv<float>(10, 20, 3, 2);
        test_copy_to_cv<float>(90, 20, 1, 3);
        test_copy_to_cv<float>(76, 2, 4, 4);

        test_copy_to_cv<double>(12, 32, 0, 1);
        test_copy_to_cv<double>(10, 1, 3, 2);
        test_copy_to_cv<double>(23, 8, 0, 3);
        test_copy_to_cv<double>(76, 2, 4, 4);
    }

//------------------------------------------------------------------------------
}
