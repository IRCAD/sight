/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "frame_tl_test.hpp"

#include "cv_common.hpp"

#include <io/opencv/frame_tl.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::opencv::ut::frame_tl_test);

namespace sight::io::opencv::ut
{

//------------------------------------------------------------------------------

template<typename T>
static std::pair<data::frame_tl::sptr, SPTR(data::frame_tl::buffer_t)> gen_frame_tl(
    std::size_t _w,
    std::size_t _h,
    std::uint8_t _num_channels
)
{
    CPPUNIT_ASSERT(_num_channels == 1 || _num_channels == 3 || _num_channels == 4);

    const core::type type = core::type::get<T>();

    data::frame_tl::sptr frame_tl = std::make_shared<data::frame_tl>();
    frame_tl->init_pool_size(
        _w,
        _h,
        type,
        _num_channels == 1
        ? data::frame_tl::pixel_format::gray_scale
        : _num_channels == 3
        ? data::frame_tl::pixel_format::rgb
        : _num_channels == 4
        ? data::frame_tl::pixel_format::rgba
        : data::frame_tl::pixel_format::undefined
    );
    auto buffer = frame_tl->create_buffer(core::hires_clock::get_time_in_milli_sec());

    return std::make_pair(frame_tl, buffer);
}

//------------------------------------------------------------------------------

template<typename T>
static void compare_images(
    const cv::Mat& _cv_image,
    const data::frame_tl::buffer_t::element_t* _buffer,
    std::size_t _w,
    std::size_t _h,
    std::uint8_t _num_channels
)
{
    std::vector<cv::Mat> channels(_num_channels);
    cv::split(_cv_image, channels);

    CPPUNIT_ASSERT_EQUAL(2, _cv_image.dims);
    CPPUNIT_ASSERT_EQUAL(_w, static_cast<std::size_t>(_cv_image.size[1]));
    CPPUNIT_ASSERT_EQUAL(_h, static_cast<std::size_t>(_cv_image.size[0]));

    const T* image_buffer = reinterpret_cast<const T*>(_buffer);

    for(int j = 0 ; j < _cv_image.size[0] ; ++j)
    {
        for(int i = 0 ; i < _cv_image.size[1] ; ++i)
        {
            for(std::uint8_t c = 0 ; c < _num_channels ; ++c)
            {
                const std::size_t index = c
                                          + static_cast<std::size_t>(i) * _num_channels
                                          + static_cast<std::size_t>(j) * _num_channels * _w;
                CPPUNIT_ASSERT_EQUAL(image_buffer[index], channels[c].at<T>(j, i));
            }
        }
    }
}

//------------------------------------------------------------------------------

template<typename T>
static void test_move_to_cv(std::size_t _w, std::size_t _h, std::uint8_t _num_channels)
{
    const std::vector<T> image_buffer = gen_image_buffer<T>(_w, _h, 0, _num_channels);

    data::frame_tl::sptr frame_tl;
    SPTR(data::frame_tl::buffer_t) buffer;

    std::tie(frame_tl, buffer) = gen_frame_tl<T>(_w, _h, _num_channels);
    auto* elt_buffer = buffer->add_element(0);
    std::copy(image_buffer.begin(), image_buffer.end(), elt_buffer);

    {
        cv::Mat cv_image;
        io::opencv::frame_tl::move_to_cv(frame_tl, elt_buffer, cv_image);

        // Since we share the same buffer, compare the pointers
        CPPUNIT_ASSERT_EQUAL(static_cast<void*>(elt_buffer), static_cast<void*>(cv_image.data));

        compare_images<T>(cv_image, elt_buffer, _w, _h, _num_channels);
    }
    {
        cv::Mat cv_image2;
        cv_image2 = io::opencv::frame_tl::move_to_cv(frame_tl, elt_buffer);

        // Since we share the same buffer, compare the pointers
        CPPUNIT_ASSERT_EQUAL(static_cast<void*>(elt_buffer), static_cast<void*>(cv_image2.data));

        compare_images<T>(cv_image2, elt_buffer, _w, _h, _num_channels);
    }
}

//------------------------------------------------------------------------------

template<typename T>
static void test_copy_from_cv(std::size_t _w, std::size_t _h, std::uint8_t _num_channels)
{
    const std::vector<T> image_buffer = gen_image_buffer<T>(_w, _h, 0, _num_channels);
    const cv::Mat cv_image            = gen_cv_image<T>(image_buffer, _w, _h, 0, _num_channels);

    data::frame_tl::sptr frame_tl;
    SPTR(data::frame_tl::buffer_t) buffer;

    std::tie(frame_tl, buffer) = gen_frame_tl<T>(_w, _h, _num_channels);
    auto* elt_buffer = buffer->add_element(0);

    io::opencv::frame_tl::copy_from_cv(frame_tl, elt_buffer, cv_image);

    // Since we copy the buffer, ensure the pointers are different
    CPPUNIT_ASSERT(static_cast<void*>(elt_buffer) != static_cast<void*>(cv_image.data));

    compare_images<T>(cv_image, elt_buffer, _w, _h, _num_channels);
}

//------------------------------------------------------------------------------

template<typename T>
static void test_copy_to_cv(std::size_t _w, std::size_t _h, std::uint8_t _num_channels)
{
    const std::vector<T> image_buffer = gen_image_buffer<T>(_w, _h, 0, _num_channels);

    data::frame_tl::sptr frame_tl;
    SPTR(data::frame_tl::buffer_t) buffer;

    std::tie(frame_tl, buffer) = gen_frame_tl<T>(_w, _h, _num_channels);
    auto* elt_buffer = buffer->add_element(0);
    std::copy(image_buffer.begin(), image_buffer.end(), elt_buffer);

    cv::Mat cv_image;
    io::opencv::frame_tl::copy_to_cv(frame_tl, elt_buffer, cv_image);

    // Since we copy the buffer, ensure the pointers are different
    CPPUNIT_ASSERT(static_cast<void*>(elt_buffer) != static_cast<void*>(cv_image.data));

    compare_images<T>(cv_image, elt_buffer, _w, _h, _num_channels);
}

//------------------------------------------------------------------------------

void frame_tl_test::setUp()
{
}

//------------------------------------------------------------------------------

void frame_tl_test::tearDown()
{
}

//------------------------------------------------------------------------------

void frame_tl_test::move_to_cv()
{
    test_move_to_cv<std::uint8_t>(10, 2, 1);
    test_move_to_cv<std::uint8_t>(6, 12, 3);
    test_move_to_cv<std::uint8_t>(10, 7, 4);
}

//------------------------------------------------------------------------------

void frame_tl_test::copy_from_cv()
{
    test_copy_from_cv<std::uint8_t>(10, 7, 1);
    test_copy_from_cv<std::uint8_t>(10, 2, 1);
    test_copy_from_cv<std::uint8_t>(6, 12, 3);
    test_copy_from_cv<std::uint8_t>(10, 7, 4);
}

//------------------------------------------------------------------------------

void frame_tl_test::copy_to_cv()
{
    test_copy_to_cv<std::uint8_t>(10, 2, 1);
    test_copy_to_cv<std::uint8_t>(6, 12, 3);
    test_copy_to_cv<std::uint8_t>(10, 7, 4);
}

} // namespace sight::io::opencv::ut
