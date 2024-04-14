/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "line_drawer_test.hpp"

#include <core/type.hpp>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <filter/image/image_diff.hpp>
#include <filter/image/line_drawer.hpp>

#include <utest_data/generator/image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::line_drawer_test);

namespace sight::filter::image::ut
{

//------------------------------------------------------------------------------

void line_drawer_test::setUp()
{
}

//------------------------------------------------------------------------------

void line_drawer_test::tearDown()
{
}

//------------------------------------------------------------------------------

void line_drawer_test::circle_test()
{
    {
        const data::image::size_t size       = {{120, 120, 120}};
        const data::image::spacing_t spacing = {{1., 1., 1.}};
        const data::image::origin_t origin   = {{0., 0., 0.}};
        const core::type type                = core::type::INT16;

        const filter::image::line_drawer::coordinates_t point = {{20, 20, 20}};
        const double thickness                                = 0.0001;
        const std::int16_t value                              = 152;

        data::image::sptr image = std::make_shared<data::image>();

        utest_data::generator::image::generate_image(
            image,
            size,
            spacing,
            origin,
            type,
            data::image::pixel_format::gray_scale
        );

        const auto dump_lock = image->dump_lock();
        SPTR(data::image::buffer_t) val =
            data::helper::medical_image::get_pixel_in_image_space(image, value);

        filter::image::line_drawer drawer(image, nullptr);
        image_diff diff = drawer.draw(
            filter::image::bresenham_line::Orientation::z_axis,
            point,
            point,
            val.get(),
            thickness
        );

        const std::int16_t res_value = image->at<std::int16_t>(point[0], point[1], point[2]);

        CPPUNIT_ASSERT_EQUAL(value, res_value);
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), diff.num_elements());
        data::image::index_t index = (point[0] + point[1] * size[0] + point[2] * size[0] * size[1]);
        CPPUNIT_ASSERT_EQUAL(index, diff.get_element(0).m_index);
        CPPUNIT_ASSERT_EQUAL(std::int16_t(0), *reinterpret_cast<const std::int16_t*>(diff.get_element(0).m_old_value));
        CPPUNIT_ASSERT_EQUAL(value, *reinterpret_cast<const std::int16_t*>(diff.get_element(0).m_new_value));
    }

    {
        const data::image::size_t size       = {{120, 120, 120}};
        const data::image::spacing_t spacing = {{1., 1., 1.}};
        const data::image::origin_t origin   = {{0., 0., 0.}};
        const core::type type                = core::type::INT16;

        const filter::image::line_drawer::coordinates_t point = {{20, 20, 20}};

        const double thickness   = 5;
        const std::int16_t value = 152;

        data::image::sptr image = std::make_shared<data::image>();

        utest_data::generator::image::generate_image(
            image,
            size,
            spacing,
            origin,
            type,
            data::image::pixel_format::gray_scale
        );
        const auto dump_lock = image->dump_lock();

        SPTR(data::image::buffer_t) val =
            data::helper::medical_image::get_pixel_in_image_space(image, value);

        filter::image::line_drawer drawer(image, nullptr);
        drawer.draw(filter::image::bresenham_line::Orientation::z_axis, point, point, val.get(), thickness);

        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0], point[1], point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }

        {
            const std::int16_t res_value = image->at<std::int16_t>(29, 36, 54);
            CPPUNIT_ASSERT_EQUAL(std::int16_t(0), res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] - 2, point[1] - 2, point[2]);
            CPPUNIT_ASSERT_EQUAL(std::int16_t(0), res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] - 2, point[1] - 1, point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] - 2, point[1], point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] - 2, point[1] + 1, point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] - 2, point[1] + 2, point[2]);
            CPPUNIT_ASSERT_EQUAL(std::int16_t(0), res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] - 1, point[1] - 2, point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] - 1, point[1] - 1, point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] - 1, point[1], point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] - 1, point[1] + 1, point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] - 1, point[1] + 2, point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0], point[1] - 2, point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0], point[1] - 1, point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0], point[1] + 1, point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0], point[1] + 2, point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] + 1, point[1] - 2, point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] + 1, point[1] - 1, point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] + 1, point[1], point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] + 1, point[1] + 1, point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] + 1, point[1] + 2, point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] + 2, point[1] - 2, point[2]);
            CPPUNIT_ASSERT_EQUAL(std::int16_t(0), res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] + 2, point[1] - 1, point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] + 2, point[1], point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] + 2, point[1] + 1, point[2]);
            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }
        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0] + 2, point[1] + 2, point[2]);
            CPPUNIT_ASSERT_EQUAL(std::int16_t(0), res_value);
        }
    }
}

//------------------------------------------------------------------------------

void line_drawer_test::ellipse_test()
{
    {
        const data::image::size_t size       = {{120, 120, 120}};
        const data::image::spacing_t spacing = {{1., 4., 1.}};
        const data::image::origin_t origin   = {{0., 0., 0.}};
        const core::type type                = core::type::INT16;

        const filter::image::line_drawer::coordinates_t point = {{50, 50, 50}};

        const double thickness   = 10;
        const std::int16_t value = 152;

        data::image::sptr image = std::make_shared<data::image>();

        utest_data::generator::image::generate_image(
            image,
            size,
            spacing,
            origin,
            type,
            data::image::pixel_format::gray_scale
        );

        const auto dump_lock = image->dump_lock();
        SPTR(data::image::buffer_t) val =
            data::helper::medical_image::get_pixel_in_image_space(image, value);

        filter::image::line_drawer drawer(image, nullptr);
        image_diff diff = drawer.draw(
            filter::image::bresenham_line::Orientation::z_axis,
            point,
            point,
            val.get(),
            thickness
        );

        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0], point[1], point[2]);

            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }

        {
            const double square_radius = (thickness / 2.) * (thickness / 2.);
            const std::array p         = {static_cast<double>(point[0]),
                                          static_cast<double>(point[1]),
                                          static_cast<double>(point[2])
            };

            std::size_t diff_index = 0;

            for(std::size_t j = 0 ; j < size[1] ; ++j)
            {
                for(std::size_t i = 0 ; i < size[0] ; ++i)
                {
                    const std::int16_t res_value = image->at<std::int16_t>(i, j, point[2]);

                    const double x    = (static_cast<double>(i) - p[0]) * spacing[0];
                    const double y    = (static_cast<double>(j) - p[1]) * spacing[1];
                    const double dist = x * x + y * y;
                    if(dist <= square_radius)
                    {
                        image_diff::element_t elt = diff.get_element(diff_index);

                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "p[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                            value,
                            res_value
                        );
                        CPPUNIT_ASSERT(diff_index != diff.num_elements());
                        const data::image::index_t index = i + j * size[0] + point[2] * size[0] * size[1];

                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "p[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                            index,
                            elt.m_index
                        );

                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "p[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                            value,
                            *reinterpret_cast<const std::int16_t*>(elt.m_new_value)
                        );

                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "p[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                            std::int16_t(0),
                            *reinterpret_cast<const std::int16_t*>(elt.m_old_value)
                        );
                        ++diff_index;
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "p[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                            std::int16_t(0),
                            res_value
                        );
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void line_drawer_test::border_test()
{
    {
        const data::image::size_t size       = {{50, 50, 50}};
        const data::image::spacing_t spacing = {{2., 4., 8.}};
        const data::image::origin_t origin   = {{0., 0., 0.}};
        const core::type type                = core::type::INT16;

        const filter::image::line_drawer::coordinates_t point = {{45, 3, 20}};

        const double thickness   = 15;
        const std::int16_t value = 1952;

        data::image::sptr image = std::make_shared<data::image>();

        utest_data::generator::image::generate_image(
            image,
            size,
            spacing,
            origin,
            type,
            data::image::pixel_format::gray_scale
        );

        const auto dump_lock = image->dump_lock();
        SPTR(data::image::buffer_t) val =
            data::helper::medical_image::get_pixel_in_image_space(image, value);

        filter::image::line_drawer drawer(image, nullptr);
        drawer.draw(filter::image::bresenham_line::Orientation::z_axis, point, point, val.get(), thickness);

        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0], point[1], point[2]);

            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }

        {
            const double square_radius = (thickness / 2.) * (thickness / 2.);
            const std::array p         = {static_cast<double>(point[0]),
                                          static_cast<double>(point[1]),
                                          static_cast<double>(point[2])
            };
            for(std::size_t i = 0 ; i < size[0] ; ++i)
            {
                for(std::size_t j = 0 ; j < size[1] ; ++j)
                {
                    const std::int16_t res_value = image->at<std::int16_t>(i, j, point[2]);

                    const double x    = (static_cast<double>(i) - p[0]) * spacing[0];
                    const double y    = (static_cast<double>(j) - p[1]) * spacing[1];
                    const double dist = x * x + y * y;

                    if(dist <= square_radius)
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "p[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                            value,
                            res_value
                        );
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "p[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                            std::int16_t(0),
                            res_value
                        );
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void line_drawer_test::roi_test()
{
    {
        const data::image::size_t size       = {{150, 150, 150}};
        const data::image::spacing_t spacing = {{2., 4., 8.}};
        const data::image::origin_t origin   = {{0., 0., 0.}};
        const core::type type                = core::type::INT16;

        const filter::image::line_drawer::coordinates_t point = {{45, 45, 40}};

        const data::helper::medical_image::orientation_t orientation =
            data::helper::medical_image::z_axis;
        const double thickness   = 15;
        const std::int16_t value = 1952;

        data::image::sptr image     = std::make_shared<data::image>();
        data::image::sptr roi_image = std::make_shared<data::image>();

        utest_data::generator::image::generate_image(
            image,
            size,
            spacing,
            origin,
            type,
            data::image::pixel_format::gray_scale
        );
        utest_data::generator::image::generate_image(
            roi_image,
            size,
            spacing,
            origin,
            type,
            data::image::pixel_format::gray_scale
        );

        const auto roi_dump_lock = roi_image->dump_lock();

        // draw a cube in ROI
        const data::image::size_t roi_begin = {{25, 25, 25}};
        const data::image::size_t roi_end   = {{50, 50, 50}};
        const std::int16_t roi_value        = 1;

        SPTR(data::image::buffer_t) roi_val =
            data::helper::medical_image::get_pixel_in_image_space(roi_image, roi_value);

        for(std::size_t i = roi_begin[0] ; i < roi_end[0] ; ++i)
        {
            for(std::size_t j = roi_begin[1] ; j < roi_end[1] ; ++j)
            {
                for(std::size_t k = roi_begin[2] ; k < roi_end[2] ; ++k)
                {
                    data::image::index_t index = i + j * size[0] + k * size[0] * size[1];
                    roi_image->set_pixel(index, roi_val.get());
                }
            }
        }

        const auto dump_lock = image->dump_lock();
        SPTR(data::image::buffer_t) val =
            data::helper::medical_image::get_pixel_in_image_space(image, value);

        filter::image::line_drawer drawer(image, roi_image);
        drawer.draw(orientation, point, point, val.get(), thickness);

        {
            const std::int16_t res_value = image->at<std::int16_t>(point[0], point[1], point[2]);

            CPPUNIT_ASSERT_EQUAL(value, res_value);
        }

        {
            const double square_radius = (thickness / 2.) * (thickness / 2.);
            const std::array p         = {static_cast<double>(point[0]),
                                          static_cast<double>(point[1]),
                                          static_cast<double>(point[2])
            };
            for(std::size_t i = 0 ; i < size[0] ; ++i)
            {
                for(std::size_t j = 0 ; j < size[1] ; ++j)
                {
                    const std::int16_t res_value = image->at<std::int16_t>(i, j, point[2]);

                    const double x    = (static_cast<double>(i) - p[0]) * spacing[0];
                    const double y    = (static_cast<double>(j) - p[1]) * spacing[1];
                    const double dist = x * x + y * y;

                    if(dist <= square_radius
                       && i >= roi_begin[0] && i <= roi_end[0]
                       && j >= roi_begin[1] && j <= roi_end[1])
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "p[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                            value,
                            res_value
                        );
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "p[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                            std::int16_t(0),
                            res_value
                        );
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image::ut
