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

#include <core/memory/stream/in/raw.hpp>
#include <core/profiling.hpp>
#include <core/tools/system.hpp>

#include <data/image.hpp>
#include <data/iterator.hpp>
#include <data/reconstruction.hpp>

#include <utest_data/generator/image.hpp>

#include <exception>
#include <iostream>
#include <map>
#include <ostream>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::image_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void image_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void image_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void image_test::test_getter_setter()
{
    const data::image::spacing_t spacing         = {2.5, 2.6};
    const data::image::origin_t origin           = {2.7, 2.8};
    const data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
    const double windowcenter                    = 10.10;
    const double windowwidth                     = 11.34;

    // process
    data::image::sptr img1 = std::make_shared<data::image>();

    img1->set_spacing(spacing);
    img1->set_origin(origin);
    img1->set_orientation(orientation);
    img1->set_window_center({windowcenter});
    img1->set_window_width({windowwidth});

    // check
    CPPUNIT_ASSERT(img1->spacing() == spacing);
    CPPUNIT_ASSERT(img1->origin() == origin);
    CPPUNIT_ASSERT(img1->orientation() == orientation);
    CPPUNIT_ASSERT_EQUAL(windowcenter, img1->window_center().front());
    CPPUNIT_ASSERT_EQUAL(windowwidth, img1->window_width().front());
}

//------------------------------------------------------------------------------

void image_test::test_allocation()
{
    const core::type type              = core::type::INT16;
    const data::image::size_t img_size = {14, 15, 26};
    const std::size_t nb_elts          = 14LL * 15 * 26;
    const std::size_t size             = nb_elts * type.size();

    // process
    data::image::sptr img1 = std::make_shared<data::image>();

    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), img1->num_elements());

    img1->resize(img_size, type, data::image::pixel_format_t::gray_scale);
    CPPUNIT_ASSERT_EQUAL(size, img1->size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(nb_elts, img1->num_elements());
    CPPUNIT_ASSERT(img1->type() == type);

    data::image::sptr img3 = std::make_shared<data::image>();
    img3->resize(img_size, type, data::image::pixel_format_t::gray_scale);
    CPPUNIT_ASSERT_EQUAL(size, img3->size_in_bytes());
    CPPUNIT_ASSERT(img1->type() == type);
    CPPUNIT_ASSERT_EQUAL(data::image::pixel_format_t::gray_scale, img3->pixel_format());
}

//------------------------------------------------------------------------------

void image_test::test_reallocation()
{
    const core::type typ_e1              = core::type::INT16;
    const core::type typ_e2              = core::type::INT64;
    const core::type typ_e3              = core::type::UINT8;
    const data::image::size_t img_siz_e1 = {10, 10, 10};
    const data::image::size_t img_siz_e2 = {20, 20, 20};
    const data::image::size_t img_siz_e3 = {5, 5, 5};
    const data::image::size_t img_siz_e5 = {0, 0, 0};
    const data::image::size_t img_siz_e6 = {42, 20};
    const std::size_t siz_e1             = 10LL * 10 * 10 * typ_e1.size();
    const std::size_t siz_e2             = 20LL * 20 * 20 * typ_e2.size();
    const std::size_t siz_e3             = 5LL * 5 * 5 * typ_e3.size();
    const std::size_t siz_e4             = 5LL * 5 * 5 * 4 * typ_e3.size();
    const std::size_t siz_e5             = 0;
    const std::size_t siz_e6             = 42LL * 20;
    const std::size_t siz_e7             = 42LL * 20 * 3;

    // process
    data::image::sptr img1 = std::make_shared<data::image>();

    const std::size_t resized1 = img1->resize(img_siz_e1, typ_e1, data::image::pixel_format_t::gray_scale);
    CPPUNIT_ASSERT_EQUAL(resized1, img1->size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(siz_e1, img1->size_in_bytes());

    const std::size_t resized2 = img1->resize(img_siz_e2, typ_e2, data::image::pixel_format_t::gray_scale);
    CPPUNIT_ASSERT_EQUAL(resized2, img1->size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(siz_e2, img1->size_in_bytes());

    const std::size_t resized3 = img1->resize(img_siz_e3, typ_e3, data::image::pixel_format_t::gray_scale);
    CPPUNIT_ASSERT_EQUAL(resized3, img1->size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(siz_e3, img1->size_in_bytes());

    const std::size_t resized4 = img1->resize(img_siz_e3, typ_e3, data::image::pixel_format_t::rgba);
    CPPUNIT_ASSERT_EQUAL(resized4, img1->size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(siz_e4, img1->size_in_bytes());

    const std::size_t resized5 = img1->resize(img_siz_e5, typ_e3, data::image::pixel_format_t::gray_scale);
    CPPUNIT_ASSERT_EQUAL(resized5, img1->size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(siz_e5, img1->size_in_bytes());

    const std::size_t resized6 = img1->resize(img_siz_e6, typ_e3, data::image::pixel_format_t::gray_scale);
    CPPUNIT_ASSERT_EQUAL(resized6, img1->size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(siz_e6, img1->size_in_bytes());

    const std::size_t resized7 = img1->resize(img_siz_e6, typ_e3, data::image::pixel_format_t::rgb);
    CPPUNIT_ASSERT_EQUAL(resized7, img1->size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(siz_e7, img1->size_in_bytes());
}

//------------------------------------------------------------------------------

void image_test::test_set_get_pixel()
{
    data::image::sptr img = std::make_shared<data::image>();

    const core::type type          = core::type::INT16;
    const data::image::size_t size = {10, 20, 30};

    const auto allocated_size = img->resize(size, type, data::image::pixel_format_t::gray_scale);

    CPPUNIT_ASSERT_EQUAL(size[0] * size[1] * size[2] * 2, allocated_size);

    const auto lock     = img->dump_lock();
    auto iter           = img->begin<std::int16_t>();
    const auto iter_end = img->end<std::int16_t>();

    // test 1 : get pixel value
    std::int16_t count = 0;
    for( ; iter != iter_end ; ++iter)
    {
        *iter = count++;
    }

    data::image::csptr const_img = img;
    for(std::size_t x = 0 ; x < size[0] ; ++x)
    {
        for(std::size_t y = 0 ; y < size[1] ; ++y)
        {
            for(std::size_t z = 0 ; z < size[2] ; ++z)
            {
                const data::image::index_t index = x + y * size[0] + z * size[0] * size[1];
                const auto val                   = static_cast<std::int16_t>(index);
                CPPUNIT_ASSERT_EQUAL(val, img->at<std::int16_t>(x, y, z));
                CPPUNIT_ASSERT_EQUAL(val, const_img->at<std::int16_t>(x, y, z));
                CPPUNIT_ASSERT_EQUAL(val, img->at<std::int16_t>(index));
                CPPUNIT_ASSERT_EQUAL(val, const_img->at<std::int16_t>(index));
                CPPUNIT_ASSERT_EQUAL(val, *reinterpret_cast<std::int16_t*>(img->get_pixel(index)));
                CPPUNIT_ASSERT_EQUAL(val, *reinterpret_cast<const std::int16_t*>(const_img->get_pixel(index)));

                std::stringstream ss;
                ss << val;
                CPPUNIT_ASSERT_EQUAL(ss.str(), img->get_pixel_as_string(x, y, z));
            }
        }
    }

    // test 2 : set pixel value
    for(std::size_t x = 0 ; x < size[0] ; ++x)
    {
        for(std::size_t y = 0 ; y < size[1] ; ++y)
        {
            for(std::size_t z = 0 ; z < size[2] ; ++z)
            {
                const auto index = x + y * size[0] + z * size[0] * size[1];

                if(index % 2 == 0)
                {
                    const auto val = static_cast<std::int16_t>(index * 2);
                    img->at<std::int16_t>(index) = val;
                }
                else
                {
                    auto val = static_cast<std::int16_t>(index * 2);
                    img->set_pixel(index, reinterpret_cast<data::image::buffer_t*>(&val));
                }
            }
        }
    }

    count = 0;
    auto iter2       = img->begin<std::int16_t>();
    auto const_iter2 = const_img->begin<std::int16_t>();
    for( ; iter2 != iter_end ; ++iter2, ++const_iter2)
    {
        CPPUNIT_ASSERT_EQUAL(static_cast<std::int16_t>(count * 2), *iter2);
        CPPUNIT_ASSERT_EQUAL(static_cast<std::int16_t>(count++ * 2), *const_iter2);
    }

    data::image::csptr img2 = data::image::copy(img);

    const auto lock2 = img2->dump_lock();
    {
        auto iter_img2           = img2->begin<std::int16_t>();
        auto iter_img1           = img->begin<std::int16_t>();
        const auto iter_img2_end = img2->end<std::int16_t>();

        for( ; iter_img2 != iter_img2_end ; ++iter_img2, ++iter_img1)
        {
            CPPUNIT_ASSERT_EQUAL(*iter_img1, *iter_img2);
        }
    }
    std::fill(img->begin(), img->end(), std::int8_t(0));

    for(const auto& element : *img)
    {
        CPPUNIT_ASSERT_EQUAL(static_cast<char>(0), element);
    }

    auto iter3 = img->begin<std::int16_t>();
    for( ; iter3 != iter_end ; ++iter3)
    {
        CPPUNIT_ASSERT_EQUAL(static_cast<std::int16_t>(0), *iter3);
    }

    std::copy(img2->begin(), img2->end(), img->begin());

    {
        auto iter_img1           = img->begin<std::int16_t>();
        auto iter_img2           = img2->begin<std::int16_t>();
        const auto iter_img2_end = img2->end<std::int16_t>();

        std::size_t i = 0;
        for( ; iter_img2 != iter_img2_end ; ++iter_img2, ++iter_img1)
        {
            ++i;
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(i), *iter_img2, *iter_img1);
        }
    }
}

//------------------------------------------------------------------------------

void image_test::test_set_get_pixel_rgba()
{
    data::image::sptr img = std::make_shared<data::image>();

    const core::type type          = core::type::UINT8;
    const data::image::size_t size = {6, 5, 3};

    const auto allocated_size = img->resize(size, type, data::image::pixel_format_t::rgba);

    CPPUNIT_ASSERT_EQUAL(size[0] * size[1] * size[2] * 4, allocated_size);

    const auto lock     = img->dump_lock();
    auto iter           = img->begin<std::uint8_t>();
    const auto iter_end = img->end<std::uint8_t>();

    // test 1 : get pixel value
    std::uint8_t count = 0;
    for( ; iter != iter_end ; ++iter)
    {
        *iter = count++;
    }

    data::image::csptr const_img = img;
    for(std::size_t x = 0 ; x < size[0] ; ++x)
    {
        for(std::size_t y = 0 ; y < size[1] ; ++y)
        {
            for(std::size_t z = 0 ; z < size[2] ; ++z)
            {
                for(std::size_t c = 0 ; c < img->num_components() ; ++c)
                {
                    const data::image::index_t index = c + 4 * (x + y * size[0] + z * size[0] * size[1]);
                    const auto val                   = static_cast<std::uint8_t>(index);
                    CPPUNIT_ASSERT_EQUAL(val, img->at<std::uint8_t>(x, y, z, c));
                    CPPUNIT_ASSERT_EQUAL(val, const_img->at<std::uint8_t>(x, y, z, c));
                }

                const data::image::index_t index = x + y * size[0] + z * size[0] * size[1];
                const std::array val             = {static_cast<std::uint8_t>(4 * index),
                                                    static_cast<std::uint8_t>(4 * index + 1),
                                                    static_cast<std::uint8_t>(4 * index + 2),
                                                    static_cast<std::uint8_t>(4 * index + 3)
                };
                CPPUNIT_ASSERT_EQUAL(val[0], img->at<std::uint8_t>(index));
                CPPUNIT_ASSERT_EQUAL(val[0], img->at<data::iterator::rgba>(index).r);
                CPPUNIT_ASSERT_EQUAL(val[1], img->at<data::iterator::rgba>(index).g);
                CPPUNIT_ASSERT_EQUAL(val[2], img->at<data::iterator::rgba>(index).b);
                CPPUNIT_ASSERT_EQUAL(val[3], img->at<data::iterator::rgba>(index).a);
                CPPUNIT_ASSERT_EQUAL(val[0], const_img->at<std::uint8_t>(index));
                CPPUNIT_ASSERT_EQUAL(val[0], const_img->at<data::iterator::rgba>(index).r);
                CPPUNIT_ASSERT_EQUAL(val[1], const_img->at<data::iterator::rgba>(index).g);
                CPPUNIT_ASSERT_EQUAL(val[2], const_img->at<data::iterator::rgba>(index).b);
                CPPUNIT_ASSERT_EQUAL(val[3], const_img->at<data::iterator::rgba>(index).a);
                CPPUNIT_ASSERT_EQUAL(val[0], *reinterpret_cast<std::uint8_t*>(img->get_pixel(index)));
                CPPUNIT_ASSERT_EQUAL(val[0], *reinterpret_cast<const std::uint8_t*>(const_img->get_pixel(index)));
            }
        }
    }

    // test 2 : set pixel value
    for(std::size_t z = 0 ; z < size[2] ; ++z)
    {
        for(std::size_t y = 0 ; y < size[1] ; ++y)
        {
            for(std::size_t x = 0 ; x < size[0] ; ++x)
            {
                const auto index = x + y * size[0] + z * size[0] * size[1];

                if(index % 2 == 0)
                {
                    for(std::size_t c = 0 ; c < img->num_components() ; ++c)
                    {
                        const auto val = static_cast<std::uint8_t>((index * 4 + c) * 2);
                        img->at<std::uint8_t>(x, y, z, c) = val;
                    }
                }
                else
                {
                    std::array val = {static_cast<std::uint8_t>(index * 4 * 2),
                                      static_cast<std::uint8_t>((index * 4 + 1) * 2),
                                      static_cast<std::uint8_t>((index * 4 + 2) * 2),
                                      static_cast<std::uint8_t>((index * 4 + 3) * 2)
                    };
                    img->set_pixel(
                        index,
                        reinterpret_cast<data::image::buffer_t*>(&val)
                    );
                }
            }
        }
    }

    count = 0;
    auto iter2       = img->begin<std::uint8_t>();
    auto const_iter2 = const_img->begin<std::uint8_t>();
    for( ; iter2 != iter_end ; ++iter2, ++const_iter2)
    {
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(count * 2), *iter2);
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(count++ * 2), *const_iter2);
    }

    data::image::csptr img2 = data::image::copy(img);

    const auto lock2 = img2->dump_lock();
    {
        auto iter_img2           = img2->begin<data::iterator::rgba>();
        auto iter_img1           = img->begin<data::iterator::rgba>();
        const auto iter_img2_end = img2->end<data::iterator::rgba>();

        for( ; iter_img2 != iter_img2_end ; ++iter_img2, ++iter_img1)
        {
            CPPUNIT_ASSERT_EQUAL(iter_img1->r, iter_img2->r);
            CPPUNIT_ASSERT_EQUAL(iter_img1->g, iter_img2->g);
            CPPUNIT_ASSERT_EQUAL(iter_img1->b, iter_img2->b);
            CPPUNIT_ASSERT_EQUAL(iter_img1->a, iter_img2->a);
        }
    }
    std::fill(img->begin(), img->end(), std::int8_t(0));

    for(const auto& element : *img)
    {
        CPPUNIT_ASSERT_EQUAL(static_cast<char>(0), element);
    }

    auto iter3 = img->begin<std::uint8_t>();
    for( ; iter3 != iter_end ; ++iter3)
    {
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), *iter3);
    }

    std::copy(img2->begin(), img2->end(), img->begin());

    {
        auto iter_img1           = img->begin<std::uint8_t>();
        auto iter_img2           = img2->begin<std::uint8_t>();
        const auto iter_img2_end = img2->end<std::uint8_t>();

        std::size_t i = 0;
        for( ; iter_img2 != iter_img2_end ; ++iter_img2, ++iter_img1)
        {
            ++i;
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(i), *iter_img2, *iter_img1);
        }
    }
}

//------------------------------------------------------------------------------

void image_test::test_iterator()
{
    data::image::sptr img = std::make_shared<data::image>();

    const core::type type          = core::type::INT16;
    const data::image::size_t size = {10, 20, 15};

    const auto allocated_size = img->resize(size, type, data::image::pixel_format_t::gray_scale);

    CPPUNIT_ASSERT_EQUAL(size[0] * size[1] * size[2] * 2, allocated_size);
    CPPUNIT_ASSERT_EQUAL(size[0] * size[1] * size[2], img->num_elements());

    {
        CPPUNIT_ASSERT_THROW(img->buffer(), core::exception);
        CPPUNIT_ASSERT_THROW(img->begin(), core::exception);
    }

    const auto lock = img->dump_lock();
    {
        CPPUNIT_ASSERT_NO_THROW(img->buffer());
        CPPUNIT_ASSERT_NO_THROW(img->begin());

        // check default raw int8 iterator
        auto itr     = img->begin();
        auto itr_end = img->end();

        CPPUNIT_ASSERT_EQUAL(static_cast<std::ptrdiff_t>(allocated_size), itr_end - itr);
    }

    {
        // check raw int16 iterator
        const auto itr     = img->begin<std::int16_t>();
        const auto itr_end = img->end<std::int16_t>();

        CPPUNIT_ASSERT_EQUAL(static_cast<std::ptrdiff_t>(img->num_elements()), itr_end - itr);
    }

    const auto allocated_size2 = img->resize(size, type, data::image::pixel_format_t::rgb);

    CPPUNIT_ASSERT_EQUAL(size[0] * size[1] * size[2] * 3 * 2, allocated_size2);
    CPPUNIT_ASSERT_EQUAL(size[0] * size[1] * size[2] * 3, img->num_elements());

    {
        // check raw int16 iterator
        const auto itr     = img->begin<std::int16_t>();
        const auto itr_end = img->end<std::int16_t>();

        CPPUNIT_ASSERT_EQUAL(static_cast<std::ptrdiff_t>(img->num_elements()), itr_end - itr);
    }

    {
        // check RGB int8 iterator
        const auto itr     = img->begin<data::iterator::rgb>();
        const auto itr_end = img->end<data::iterator::rgb>();

        CPPUNIT_ASSERT_EQUAL(static_cast<std::ptrdiff_t>(img->num_elements() / 3 * 2), itr_end - itr);
    }

    {
        // check Raw int64 iterator
        const auto itr     = img->begin<std::int64_t>();
        const auto itr_end = img->end<std::int64_t>();

        CPPUNIT_ASSERT_EQUAL(static_cast<std::ptrdiff_t>(img->size_in_bytes() / 8), itr_end - itr);
    }
}

//------------------------------------------------------------------------------

void image_test::test_rgb_iterator()
{
    data::image::sptr img = std::make_shared<data::image>();

    const core::type type          = core::type::UINT8;
    const data::image::size_t size = {10, 20, 15};

    const auto allocated_size = img->resize(size, type, data::image::pixel_format_t::rgb);

    CPPUNIT_ASSERT_EQUAL(size[0] * size[1] * size[2] * 3, allocated_size);

    CPPUNIT_ASSERT_EQUAL(size[0] * size[1] * size[2] * 3, img->num_elements());

    const auto lock = img->dump_lock();

    const data::iterator::rgb value = {18, 12, 68};
    std::fill(img->begin<data::iterator::rgb>(), img->end<data::iterator::rgb>(), value);

    std::uint8_t count = 0;

    auto iter_rgb           = img->begin<data::iterator::rgb>();
    const auto iter_end_rgb = img->end<data::iterator::rgb>();

    for( ; iter_rgb != iter_end_rgb ; ++iter_rgb)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff[" + std::to_string(count) + "].r", value.r, iter_rgb->r);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff[" + std::to_string(count) + "].g", value.g, iter_rgb->g);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff[" + std::to_string(count) + "].b", value.b, iter_rgb->b);
        ++count;
    }

    count = 0;
    auto iter_rgb2 = img->begin<data::iterator::rgb>();
    for( ; iter_rgb2 != iter_end_rgb ; ++iter_rgb2)
    {
        iter_rgb2->r = count++;
        iter_rgb2->g = count++;
        iter_rgb2->b = count++;
    }

    count = 0;
    for(auto& rgb3 : img->range<data::iterator::rgb>())
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "buff[" + std::to_string(count) + "].r",
            static_cast<std::uint8_t>(3 * count),
            rgb3.r
        );
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "buff[" + std::to_string(count) + "].g",
            static_cast<std::uint8_t>(3 * count + 1),
            rgb3.g
        );
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "buff[" + std::to_string(count) + "].b",
            static_cast<std::uint8_t>(3 * count + 2),
            rgb3.b
        );
        ++count;
    }

    auto begin_iter_rgb   = img->begin<data::iterator::rgb>();
    auto current_iter_rgb = img->begin<data::iterator::rgb>();

    CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(0), current_iter_rgb->r);
    CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(1), current_iter_rgb->g);
    CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(2), current_iter_rgb->b);

    CPPUNIT_ASSERT_EQUAL(current_iter_rgb->r, begin_iter_rgb[0].r);
    CPPUNIT_ASSERT_EQUAL(current_iter_rgb->g, begin_iter_rgb[0].g);
    CPPUNIT_ASSERT_EQUAL(current_iter_rgb->b, begin_iter_rgb[0].b);

    ++current_iter_rgb;
    CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(3), current_iter_rgb->r);
    CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(4), current_iter_rgb->g);
    CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(5), current_iter_rgb->b);

    CPPUNIT_ASSERT_EQUAL(current_iter_rgb->r, begin_iter_rgb[1].r);
    CPPUNIT_ASSERT_EQUAL(current_iter_rgb->g, begin_iter_rgb[1].g);
    CPPUNIT_ASSERT_EQUAL(current_iter_rgb->b, begin_iter_rgb[1].b);

    ++current_iter_rgb;
    CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(6), current_iter_rgb->r);
    CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(7), current_iter_rgb->g);
    CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(8), current_iter_rgb->b);

    CPPUNIT_ASSERT_EQUAL(current_iter_rgb->r, begin_iter_rgb[2].r);
    CPPUNIT_ASSERT_EQUAL(current_iter_rgb->g, begin_iter_rgb[2].g);
    CPPUNIT_ASSERT_EQUAL(current_iter_rgb->b, begin_iter_rgb[2].b);
}

//------------------------------------------------------------------------------

void image_test::test_bgr_iterator()
{
    data::image::sptr img = std::make_shared<data::image>();

    const core::type type          = core::type::UINT8;
    const data::image::size_t size = {10, 20};

    const auto allocated_size = img->resize(size, type, data::image::pixel_format_t::bgr);

    CPPUNIT_ASSERT_EQUAL(size[0] * size[1] * 3, allocated_size);

    CPPUNIT_ASSERT_EQUAL(size[0] * size[1] * 3, img->num_elements());

    const auto lock = img->dump_lock();

    auto iter           = img->begin<std::uint8_t>();
    const auto iter_end = img->end<std::uint8_t>();

    std::uint8_t count = 0;
    for( ; iter != iter_end ; ++iter)
    {
        *iter = count++;
    }

    count = 0;

    auto iter2           = img->begin<data::iterator::bgr>();
    const auto iter_end2 = img->end<data::iterator::bgr>();
    for( ; iter2 != iter_end2 ; ++iter2)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "buff[" + std::to_string(count) + "].b",
            static_cast<std::uint8_t>(3 * count),
            iter2->b
        );
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "buff[" + std::to_string(count) + "].g",
            static_cast<std::uint8_t>(3 * count + 1),
            iter2->g
        );
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "buff[" + std::to_string(count) + "].r",
            static_cast<std::uint8_t>(3 * count + 2),
            iter2->r
        );
        ++count;
    }
}

//------------------------------------------------------------------------------

void image_test::test_bgra_iterator()
{
    data::image::sptr img = std::make_shared<data::image>();

    const core::type type          = core::type::UINT8;
    const data::image::size_t size = {10, 20};

    const auto allocated_size = img->resize(size, type, data::image::pixel_format_t::bgra);

    CPPUNIT_ASSERT_EQUAL(size[0] * size[1] * 4, allocated_size);

    CPPUNIT_ASSERT_EQUAL(size[0] * size[1] * 4, img->num_elements());

    const auto lock = img->dump_lock();

    auto iter           = img->begin<std::uint8_t>();
    const auto iter_end = img->end<std::uint8_t>();

    std::uint8_t count = 0;
    for( ; iter != iter_end ; ++iter)
    {
        *iter = count++;
    }

    count = 0;

    auto iter2           = img->begin<data::iterator::bgra>();
    const auto iter_end2 = img->end<data::iterator::bgra>();
    for( ; iter2 != iter_end2 ; ++iter2)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "buff[" + std::to_string(count) + "].b",
            static_cast<std::uint8_t>(4 * count),
            iter2->b
        );
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "buff[" + std::to_string(count) + "].g",
            static_cast<std::uint8_t>(4 * count + 1),
            iter2->g
        );
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "buff[" + std::to_string(count) + "].r",
            static_cast<std::uint8_t>(4 * count + 2),
            iter2->r
        );
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "buff[" + std::to_string(count) + "].a",
            static_cast<std::uint8_t>(4 * count + 3),
            iter2->a
        );
        ++count;
    }
}

//------------------------------------------------------------------------------

void image_test::test_rgba_iterator()
{
    data::image::sptr img = std::make_shared<data::image>();

    const core::type type          = core::type::UINT16;
    const data::image::size_t size = {10, 20, 15};

    const auto allocated_size = img->resize(size, type, data::image::pixel_format_t::rgba);
    CPPUNIT_ASSERT_EQUAL(size[0] * size[1] * size[2] * 4 * 2, allocated_size);
    CPPUNIT_ASSERT_EQUAL(size[0] * size[1] * size[2] * 4, img->num_elements());

    const auto lock = img->dump_lock();

    auto iter           = img->begin<data::iterator::rgba16>();
    const auto iter_end = img->end<data::iterator::rgba16>();

    std::uint16_t count = 0;
    std::for_each(iter, iter_end, [&count](auto& _x){_x.r = count++; _x.g = count++; _x.b = count++; _x.a = count++;});

    auto iter2           = img->begin<std::uint16_t>();
    const auto iter_end2 = img->end<std::uint16_t>();
    for(count = 0 ; iter2 != iter_end2 ; ++iter2)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff[" + std::to_string(count) + "].v", count, *iter2);
        ++count;
    }

    auto iter3           = img->begin<data::iterator::rgba16>();
    const auto iter3_end = img->end<data::iterator::rgba16>();

    for(count = 0 ; iter3 != iter3_end ; ++iter3)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "buff[" + std::to_string(count) + "].r",
            static_cast<std::uint16_t>(4 * count),
            (*iter3).r
        );
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "buff[" + std::to_string(count) + "].g",
            static_cast<std::uint16_t>(4 * count + 1),
            (*iter3).g
        );
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "buff[" + std::to_string(count) + "].b",
            static_cast<std::uint16_t>(4 * count + 2),
            (*iter3).b
        );
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "buff[" + std::to_string(count) + "].a",
            static_cast<std::uint16_t>(4 * count + 3),
            (*iter3).a
        );
        ++count;
    }
}

//------------------------------------------------------------------------------

void image_test::benchmark_iterator()
{
    data::image::sptr img = std::make_shared<data::image>();

    const core::type type              = core::type::UINT16;
    constexpr data::image::size_t size = {200, 100, 100};

    const auto allocated_size = img->resize(size, type, data::image::pixel_format_t::rgba);
    {
        const auto img_dump_lock = img->dump_lock();
        utest_data::generator::image::randomize_image(img);
    }

    CPPUNIT_ASSERT_EQUAL(size[0] * size[1] * size[2] * 4 * type.size(), allocated_size);
    CPPUNIT_ASSERT_EQUAL(size[0] * size[1] * size[2] * 4, img->num_elements());
    CPPUNIT_ASSERT_EQUAL(size[0] * size[1] * size[2] * 4 * 2, img->size_in_bytes());

    data::image::sptr img2 = std::make_shared<data::image>();
    img2->copy_information(img);
    img2->resize(img2->size(), img2->type(), img2->pixel_format());

    CPPUNIT_ASSERT_EQUAL(size[0] * size[1] * size[2] * 4, img2->num_elements());
    CPPUNIT_ASSERT_EQUAL(size[0] * size[1] * size[2] * 4 * 2, img2->size_in_bytes());

    const auto img_dump_lock  = img->dump_lock();
    const auto img_dump_lock2 = img2->dump_lock();
    auto begin1               = img->begin<data::iterator::rgba16>();
    auto end1                 = img->end<data::iterator::rgba16>();
    auto begin2               = img2->begin<data::iterator::rgba16>();
    auto end2                 = img2->end<data::iterator::rgba16>();

    constexpr auto new_size = size[0] * size[1] * size[2];
    std::vector<data::iterator::rgba16> std_vector_image;
    std::vector<data::iterator::rgba16> std_vector_image2;

    std_vector_image.resize(new_size);
    std_vector_image2.resize(new_size);
    {
        std::copy(img->begin<data::iterator::rgba16>(), img->end<data::iterator::rgba16>(), std_vector_image.begin());
    }

    CPPUNIT_ASSERT_EQUAL(new_size, std_vector_image.size());
    CPPUNIT_ASSERT_EQUAL(new_size, std_vector_image2.size());

    // Here the std::vector will always be faster than our iterator because the STL internally optimizes the loop
    // iteration with a memmove instead. We can't achieve this on our own, we don't have access to std::__niter
    {
        FW_PROFILE("std::copy - ImageIterator");
        std::copy(begin1, end1, begin2);
    }
    {
        FW_PROFILE("std::copy - std::vector");
        std::copy(std_vector_image.cbegin(), std_vector_image.cend(), std_vector_image2.begin());
    }

    // Here is the real test, we expect to have comparable timings with the STL when have a real loop on both sides
    auto fn = [](auto& _x){_x.r = _x.r + 2; _x.g = _x.g + 8; _x.a = _x.a + 2; _x.b = _x.b + 7;};
    {
        FW_PROFILE("std::for_each - ImageIterator");
        std::for_each(begin2, end2, fn);
    }
    {
        FW_PROFILE("std::for_each - std::vector");
        std::for_each(std_vector_image.begin(), std_vector_image.end(), fn);
    }

    // Here is the real test, we expect to have comparable timings with the STL when have a real loop on both sides

    auto begin3 = img2->begin<data::iterator::rgba16>();
    auto end3   = img2->end<data::iterator::rgba16>();

    auto fn2 = [](auto& _pix){_pix.r = _pix.r + 2; _pix.g = _pix.g + 8; _pix.a = _pix.a + 2; _pix.b = _pix.b + 7;};
    {
        FW_PROFILE("std::for_each - image_iterator");
        std::for_each(begin3, end3, fn2);
    }

    // {
    //     FW_PROFILE("std::fill - image_iterator");
    //     data::RGBA16 zero = {0, 0, 0, 0};
    //     std::fill(begin3, end3, zero);
    // }

    auto begin5 = img->cbegin<data::iterator::rgba16>();
    auto end5   = img->cend<data::iterator::rgba16>();

    {
        FW_PROFILE("std::copy - image_iterator");
        std::copy(begin5, end5, begin3);
    }
}

//------------------------------------------------------------------------------

void image_test::image_deep_copy()
{
    {
        const data::image::sptr img = std::make_shared<data::image>();
        const data::image::size_t size {32, 32, 32};
        const data::image::origin_t origin {0.2, 123.4, 999.666};
        const data::image::orientation_t orientation {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
        const data::image::spacing_t spacing {0.6, 0.6, 1.8};
        const auto type   = core::type::UINT8;
        const auto format = data::image::pixel_format_t::rgb;

        utest_data::generator::image::generate_image(img, size, spacing, origin, orientation, type, format, 0);

        const data::image::sptr img_copy = std::make_shared<data::image>();

        // Lock the imgCopy buffer to make sure the underlying array isn't deleted.
        // Attempting to delete a locked array raises an assert in
        // `core::memory::buffer_manager::unregisterBufferImpl()`.
        const auto img_copy_lock = img_copy->dump_lock();

        // Test a bit more image equality operator, which ensure the copy test is really working
        utest_data::generator::image::generate_image(img_copy, size, spacing, origin, orientation, type, format, 1);

        CPPUNIT_ASSERT(*img != *img_copy);

        img_copy->deep_copy(img);

        CPPUNIT_ASSERT(*img == *img_copy);
    }

    {
        const data::image::sptr img = std::make_shared<data::image>();
        const data::image::size_t size {156, 126, 0};
        const data::image::origin_t origin {1., 1., 0.};
        const data::image::orientation_t orientation {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
        const data::image::spacing_t spacing {10., 10., 0.};
        const auto type   = core::type::FLOAT;
        const auto format = data::image::pixel_format_t::gray_scale;

        utest_data::generator::image::generate_image(img, size, spacing, origin, orientation, type, format, 0);

        const data::image::sptr img_copy = std::make_shared<data::image>();

        CPPUNIT_ASSERT(*img != *img_copy);

        const auto img_copy_lock = img_copy->dump_lock();

        // Test a bit more image equality operator, which ensure the copy test is really working
        utest_data::generator::image::generate_image(img_copy, size, spacing, origin, orientation, type, format, 1);

        CPPUNIT_ASSERT(*img != *img_copy);

        img_copy->deep_copy(img);

        CPPUNIT_ASSERT(*img == *img_copy);
    }
}

//------------------------------------------------------------------------------

void image_test::empty_iterator_test()
{
    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generate_random_image(image, core::type::INT16);

    const auto dump_lock = image->dump_lock();

    auto iter      = image->begin<std::int16_t>();
    const auto end = image->end<std::int16_t>();

    data::image::iterator<std::int16_t> max_iter;

    std::int16_t max_value = *iter;

    for( ; iter != end ; ++iter)
    {
        if(*iter > max_value)
        {
            max_iter  = iter;
            max_value = *iter;
        }
    }

    CPPUNIT_ASSERT_EQUAL(*max_iter, max_value);
}

//------------------------------------------------------------------------------

void image_test::equality_test()
{
    static constexpr std::array s_WHITE_PIXEL {std::uint8_t(0), std::uint8_t(0), std::uint8_t(0)};
    static constexpr std::array s_BLACK_PIXEL {std::uint8_t(255), std::uint8_t(255), std::uint8_t(255)};
    auto image1 = std::make_shared<data::image>();
    auto image2 = std::make_shared<data::image>();

    CPPUNIT_ASSERT(*image1 == *image2 && !(*image1 != *image2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
            image1->op; \
            CPPUNIT_ASSERT_MESSAGE( \
                "Images should be different when applying " #op " to the first one", \
                *image1 != *image2 && !(*image1 == *image2) \
            ); \
            image2->op; \
            CPPUNIT_ASSERT_MESSAGE( \
                "Images should be equal when applying " #op " to both", \
                *image1 == *image2 && !(*image1 != *image2) \
            );

    TEST(set_spacing({1, 2, 3}));
    TEST(set_origin({4, 5, 6}));
    TEST(set_orientation({0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6}));
    TEST(set_window_center({7, 8, 9}));
    TEST(set_window_width({10, 11, 12}));
    image1->resize({1, 0, 0}, core::type::UINT8, data::image::pixel_format_t::rgb);
    CPPUNIT_ASSERT(*image1 != *image2 && !(*image1 == *image2));
    image2->resize({1, 0, 0}, core::type::UINT8, data::image::pixel_format_t::rgb);
    auto lock1 = image1->dump_lock();
    auto lock2 = image2->dump_lock();
    image1->set_pixel(0, s_WHITE_PIXEL.data());
    image2->set_pixel(0, s_WHITE_PIXEL.data());
    TEST(resize({1, 1, 0}, core::type::UINT8, data::image::pixel_format_t::rgb));
    TEST(resize({1, 1, 1}, core::type::UINT8, data::image::pixel_format_t::rgb));
    TEST(resize({0, 0, 0}, core::type::INT8, data::image::pixel_format_t::rgb));
    TEST(resize({0, 0, 0}, core::type::UINT8, data::image::pixel_format_t::bgr));
    TEST(set_pixel(0, s_BLACK_PIXEL.data()));

    #undef TEST
}

//------------------------------------------------------------------------------

void image_test::world_test()
{
    // Origin at (0, 0, 0) with no rotation
    // Image(0, 0, 0) -> world(0, 0, 0)
    // World(0, 0, 0) -> image(0, 0, 0)
    {
        auto image = std::make_shared<data::image>();
        image->set_spacing({1.0, 1.0, 1.0});
        image->set_origin({0, 0, 0});
        image->set_orientation({1, 0, 0, 0, 1, 0, 0, 0, 1});

        const auto& actual_world = image->image_to_world({0, 0, 0});
        const glm::dvec3 expected_world {0.0, 0.0, 0.0};
        CPPUNIT_ASSERT_EQUAL(expected_world[0], actual_world[0]);
        CPPUNIT_ASSERT_EQUAL(expected_world[1], actual_world[1]);
        CPPUNIT_ASSERT_EQUAL(expected_world[2], actual_world[2]);

        const auto actual_image = image->world_to_image({0, 0, 0});
        const glm::ivec3 expected_image {0, 0, 0};
        CPPUNIT_ASSERT_EQUAL(expected_image[0], actual_image[0]);
        CPPUNIT_ASSERT_EQUAL(expected_image[1], actual_image[1]);
        CPPUNIT_ASSERT_EQUAL(expected_image[2], actual_image[2]);
    }

    // Origin at (2, 3, 4) with no rotation
    // Image(0, 0, 0) -> world(2, 3, 4)
    // World(2, 3, 4) -> image(0, 0, 0)
    {
        auto image = std::make_shared<data::image>();
        image->set_spacing({1.0, 1.0, 1.0});
        image->set_origin({2.0, 3.0, 4.0});
        image->set_orientation({1, 0, 0, 0, 1, 0, 0, 0, 1});

        const auto& actual_world = image->image_to_world({0, 0, 0});
        const glm::dvec3 expected_world {2.0, 3.0, 4.0};
        CPPUNIT_ASSERT_EQUAL(expected_world[0], actual_world[0]);
        CPPUNIT_ASSERT_EQUAL(expected_world[1], actual_world[1]);
        CPPUNIT_ASSERT_EQUAL(expected_world[2], actual_world[2]);

        const auto& actual_image = image->world_to_image({2.0, 3.0, 4.0});
        const glm::ivec3 expected_image {0, 0, 0};
        CPPUNIT_ASSERT_EQUAL(expected_image[0], actual_image[0]);
        CPPUNIT_ASSERT_EQUAL(expected_image[1], actual_image[1]);
        CPPUNIT_ASSERT_EQUAL(expected_image[2], actual_image[2]);
    }

    // Origin at (2, 3, 4) with 90° X rotation
    // Image(0, 0, 0) -> world(2, 3, 4)
    // World(2, 3, 4) -> image(0, 0, 0)
    {
        auto image = std::make_shared<data::image>();

        image->set_spacing({1.0, 1.0, 1.0});
        image->set_origin({2.0, 3.0, 4.0});

        // 90° rotation around X axis
        image->set_orientation({1, 0, 0, 0, 0, -1, 0, 1, 0});

        const auto& actual_world = image->image_to_world({0, 0, 0});
        const glm::dvec3 expected_world {2.0, 3.0, 4.0};
        CPPUNIT_ASSERT_EQUAL(expected_world[0], actual_world[0]);
        CPPUNIT_ASSERT_EQUAL(expected_world[1], actual_world[1]);
        CPPUNIT_ASSERT_EQUAL(expected_world[2], actual_world[2]);

        const auto& actual_image = image->world_to_image({2.0, 3.0, 4.0});
        const glm::ivec3 expected_image {0, 0, 0};
        CPPUNIT_ASSERT_EQUAL(expected_image[0], actual_image[0]);
        CPPUNIT_ASSERT_EQUAL(expected_image[1], actual_image[1]);
        CPPUNIT_ASSERT_EQUAL(expected_image[2], actual_image[2]);
    }

    // Origin at (2, 3, 4) with 90° X rotation
    // Image(5, 6, 7) -> world(7, -4, 10)
    // World(5, 6, 7) -> image(3, 3, -3)
    {
        auto image = std::make_shared<data::image>();
        image->set_spacing({1.0, 1.0, 1.0});
        image->set_origin({2.0, 3.0, 4.0});

        // 90° rotation around X axis
        image->set_orientation({1, 0, 0, 0, 0, -1, 0, 1, 0});

        const auto& actual_world = image->image_to_world({5, 6, 7});
        const glm::dvec3 expected_world {7.0, -4.0, 10.0};
        CPPUNIT_ASSERT_EQUAL(expected_world[0], actual_world[0]);
        CPPUNIT_ASSERT_EQUAL(expected_world[1], actual_world[1]);
        CPPUNIT_ASSERT_EQUAL(expected_world[2], actual_world[2]);

        const auto& actual_image = image->world_to_image({5.0, 6.0, 7.0});
        const glm::ivec3 expected_image {3, 3, -3};
        CPPUNIT_ASSERT_EQUAL(expected_image[0], actual_image[0]);
        CPPUNIT_ASSERT_EQUAL(expected_image[1], actual_image[1]);
        CPPUNIT_ASSERT_EQUAL(expected_image[2], actual_image[2]);
    }

    // Rounding test
    {
        auto image = std::make_shared<data::image>();
        image->resize({567, 789, 39}, core::type::INT16, data::image::gray_scale);
        image->set_spacing({1.4, 0.8, 0.5});
        image->set_origin({-18.3, 54.0, 230.0});

        {
            const auto indices = image->world_to_image({16, 543.65, 456.}, true);
            const glm::ivec3 expected_indices {25, 612, 452};

            CPPUNIT_ASSERT_EQUAL(expected_indices[0], indices[0]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[1], indices[1]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[2], indices[2]);
        }

        {
            const auto indices = image->world_to_image({2., 34.4, 7.}, true);
            const glm::ivec3 expected_indices {15, -25, -446};

            CPPUNIT_ASSERT_EQUAL(expected_indices[0], indices[0]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[1], indices[1]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[2], indices[2]);
        }

        // 90° rotation around the x-axis
        image->set_orientation(
            {
                1.0000000, 0.0000000, 0.0000000,
                0.0000000, 0.0000000, -1.0000000,
                0.0000000, 1.0000000, 0.0000000
            });

        {
            const auto indices = image->world_to_image({16, 543.65, 456.}, true);
            const glm::ivec3 expected_indices {25, 283, -979};

            CPPUNIT_ASSERT_EQUAL(expected_indices[0], indices[0]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[1], indices[1]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[2], indices[2]);
        }

        {
            const auto indices = image->world_to_image({2., 34.4, 7.}, true);
            const glm::ivec3 expected_indices {15, -279, 39};

            CPPUNIT_ASSERT_EQUAL(expected_indices[0], indices[0]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[1], indices[1]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[2], indices[2]);
        }
    }

    // Clamp test
    {
        const data::image::size_t size {100, 110, 120};
        const data::image::spacing_t spacing {0.5, 0.6, 0.7};
        const data::image::origin_t origin {1, 2, 3};

        auto image = std::make_shared<data::image>();

        image->resize(size, core::type::INT8, data::image::gray_scale);
        image->set_spacing(spacing);
        image->set_origin(origin);

        // Lower bound
        {
            const auto indices = image->world_to_image(origin, false, true);
            const glm::ivec3 expected_indices {0, 0, 0};

            CPPUNIT_ASSERT_EQUAL(expected_indices[0], indices[0]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[1], indices[1]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[2], indices[2]);
        }

        {
            const auto indices = image->world_to_image(
                {
                    origin[0] + spacing[0],
                    origin[1] + spacing[1],
                    origin[2] + spacing[2]
                },
                false,
                true
            );

            const glm::ivec3 expected_indices {1, 1, 1};

            CPPUNIT_ASSERT_EQUAL(expected_indices[0], indices[0]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[1], indices[1]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[2], indices[2]);
        }

        {
            const auto indices = image->world_to_image(
                {
                    origin[0] - spacing[0],
                    origin[1] - spacing[1],
                    origin[2] - spacing[2]
                },
                false,
                true
            );

            const glm::ivec3 expected_indices {0, 0, 0};

            CPPUNIT_ASSERT_EQUAL(expected_indices[0], indices[0]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[1], indices[1]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[2], indices[2]);
        }

        {
            const auto indices = image->world_to_image(
                {
                    origin[0] - spacing[0],
                    origin[1] - spacing[1],
                    origin[2] - spacing[2]
                },
                false,
                false
            );

            const glm::ivec3 expected_indices {-1, -1, -1};

            CPPUNIT_ASSERT_EQUAL(expected_indices[0], indices[0]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[1], indices[1]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[2], indices[2]);
        }

        const double upper_bound_x = double(size[0]) * spacing[0] + origin[0];
        const double upper_bound_y = double(size[1]) * spacing[1] + origin[1];
        const double upper_bound_z = double(size[2]) * spacing[2] + origin[2];

        // Upper bound
        {
            const auto indices = image->world_to_image({upper_bound_x, upper_bound_y, upper_bound_z}, false, true);
            const glm::ivec3 expected_indices {
                std::int64_t(size[0]) - 1,
                std::int64_t(size[1]) - 1,
                std::int64_t(size[2]) - 1
            };

            CPPUNIT_ASSERT_EQUAL(expected_indices[0], indices[0]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[1], indices[1]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[2], indices[2]);
        }

        {
            const auto indices = image->world_to_image(
                {
                    upper_bound_x + spacing[0],
                    upper_bound_y + spacing[1],
                    upper_bound_z + spacing[2],
                },
                false,
                true
            );

            const glm::ivec3 expected_indices {
                std::int64_t(size[0]) - 1,
                std::int64_t(size[1]) - 1,
                std::int64_t(size[2]) - 1
            };

            CPPUNIT_ASSERT_EQUAL(expected_indices[0], indices[0]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[1], indices[1]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[2], indices[2]);
        }

        {
            const auto indices = image->world_to_image(
                {
                    upper_bound_x - 2 * spacing[0],
                    upper_bound_y - 2 * spacing[1],
                    upper_bound_z - 2 * spacing[2],
                },
                true,
                true
            );

            const glm::ivec3 expected_indices {
                std::int64_t(size[0]) - 2,
                std::int64_t(size[1]) - 2,
                std::int64_t(size[2]) - 2
            };

            CPPUNIT_ASSERT_EQUAL(expected_indices[0], indices[0]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[1], indices[1]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[2], indices[2]);
        }

        {
            const auto image_to_world = image->image_to_world(size);
            const auto indices        = image->world_to_image(image_to_world, false, false);

            const glm::ivec3 expected_indices {
                std::int64_t(size[0]),
                std::int64_t(size[1]),
                std::int64_t(size[2])
            };

            CPPUNIT_ASSERT_EQUAL(expected_indices[0], indices[0]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[1], indices[1]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[2], indices[2]);
        }
    }
}

} // namespace sight::data::ut
