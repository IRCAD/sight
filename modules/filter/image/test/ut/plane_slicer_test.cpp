/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "plane_slicer_test.hpp"

#include <core/runtime/runtime.hpp>

#include <data/image.hpp>
#include <data/matrix4.hpp>
#include <data/point_list.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::filter::image::ut::plane_slicer_test);

namespace sight::module::filter::image::ut
{

//------------------------------------------------------------------------------

void plane_slicer_test::setUp()
{
    // Set up context before running a test.
    core::runtime::init();
    auto module = core::runtime::load_module(std::string("sight::module::filter::image"));
}

//------------------------------------------------------------------------------

void plane_slicer_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void plane_slicer_test::basic()
{
    const std::uint8_t value_1 = 4;
    const std::uint8_t value_2 = 12;
    auto image                 = std::make_shared<sight::data::image>();
    {
        const auto dump_lock = image->dump_lock();

        const data::image::size_t size               = {4, 2, 5};
        const data::image::spacing_t spacing         = {3.0, 0.25, 0.5};
        const data::image::origin_t origin           = {100., -200., 20.};
        const data::image::orientation_t orientation = {1, 0, 0, 0, 1, 0, 0, 0, 1};

        image->resize(size, core::type::get<std::uint8_t>(), data::image::pixel_format_t::gray_scale);
        image->set_spacing(spacing);
        image->set_origin(origin);
        image->set_orientation(orientation);

        std::memset(image->buffer(), value_1, image->size_in_bytes());

        const auto voxel = [&](std::size_t _x, std::size_t _y, std::size_t _z)
                           {
                               return _x + _y * size[0] + _z * size[0] * size[1];
                           };

        for(std::size_t z = 0 ; z < size[2] ; ++z)
        {
            image->set_pixel(voxel(0, 1, z), reinterpret_cast<const data::image::buffer_t*>(&value_2));
            image->set_pixel(voxel(1, 1, z), reinterpret_cast<const data::image::buffer_t*>(&value_2));
            image->set_pixel(voxel(2, 1, z), reinterpret_cast<const data::image::buffer_t*>(&value_2));
            image->set_pixel(voxel(3, 1, z), reinterpret_cast<const data::image::buffer_t*>(&value_2));
        }
    }

    // Create service
    sight::service::base::sptr srv = sight::service::add("sight::module::filter::image::plane_slicer");
    CPPUNIT_ASSERT(srv->is_a("sight::module::filter::image::plane_slicer"));
    CPPUNIT_ASSERT(srv);

    auto axes  = std::make_shared<sight::data::matrix4>();
    auto slice = std::make_shared<sight::data::image>();

    srv->set_worker(sight::core::thread::get_default_worker());
    srv->set_input(image, "image");
    srv->set_input(axes, "axes");
    srv->set_inout(slice, "slice");
    srv->configure();
    srv->start().wait();

    // Slice outside the image
    {
        srv->update().wait();
        auto spacing = slice->spacing();
        CPPUNIT_ASSERT_EQUAL(3.0, spacing[0]);
        CPPUNIT_ASSERT_EQUAL(0.25, spacing[1]);

        auto size = slice->size();
        CPPUNIT_ASSERT_EQUAL(std::size_t(4), size[0]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), size[1]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

        auto origin = slice->origin();
        CPPUNIT_ASSERT_EQUAL(0., origin[0]);
        CPPUNIT_ASSERT_EQUAL(0., origin[1]);
        CPPUNIT_ASSERT_EQUAL(0., origin[2]);

        {
            const auto slice_lock = slice->dump_lock();
            for(std::size_t x = 0 ; x < size[0] ; ++x)
            {
                for(std::size_t y = 0 ; y < size[1] ; ++y)
                {
                    const auto index             = x + y * size[0];
                    const std::uint8_t value_out = slice->at<std::uint8_t>(index);
                    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), value_out);
                }
            }
        }
    }

    // Slice inside the image, in Z axis
    {
        (*axes)[3]  = 100.;
        (*axes)[7]  = -200.;
        (*axes)[11] = 20.;

        srv->update().wait();
        auto spacing = slice->spacing();
        CPPUNIT_ASSERT_EQUAL(3.0, spacing[0]);
        CPPUNIT_ASSERT_EQUAL(0.25, spacing[1]);

        auto size = slice->size();
        CPPUNIT_ASSERT_EQUAL(std::size_t(4), size[0]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), size[1]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

        auto origin = slice->origin();
        CPPUNIT_ASSERT_EQUAL(100., origin[0]);
        CPPUNIT_ASSERT_EQUAL(-200., origin[1]);
        CPPUNIT_ASSERT_EQUAL(20., origin[2]);

        {
            const auto slice_lock = slice->dump_lock();
            CPPUNIT_ASSERT_EQUAL(value_1, slice->at<std::uint8_t>(0));
            CPPUNIT_ASSERT_EQUAL(value_1, slice->at<std::uint8_t>(1));
            CPPUNIT_ASSERT_EQUAL(value_1, slice->at<std::uint8_t>(2));
            CPPUNIT_ASSERT_EQUAL(value_1, slice->at<std::uint8_t>(3));
            CPPUNIT_ASSERT_EQUAL(value_2, slice->at<std::uint8_t>(4));
            CPPUNIT_ASSERT_EQUAL(value_2, slice->at<std::uint8_t>(5));
            CPPUNIT_ASSERT_EQUAL(value_2, slice->at<std::uint8_t>(6));
            CPPUNIT_ASSERT_EQUAL(value_2, slice->at<std::uint8_t>(7));
        }
    }
    {
        (*axes)[3]  = 100.;
        (*axes)[7]  = -200.25;
        (*axes)[11] = 20.;

        srv->update().wait();
        auto spacing = slice->spacing();
        CPPUNIT_ASSERT_EQUAL(3.0, spacing[0]);
        CPPUNIT_ASSERT_EQUAL(0.25, spacing[1]);

        auto size = slice->size();
        CPPUNIT_ASSERT_EQUAL(std::size_t(4), size[0]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), size[1]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

        auto origin = slice->origin();
        CPPUNIT_ASSERT_EQUAL(100., origin[0]);
        CPPUNIT_ASSERT_EQUAL(-200.25, origin[1]);
        CPPUNIT_ASSERT_EQUAL(20., origin[2]);

        {
            const auto slice_lock = slice->dump_lock();
            CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), slice->at<std::uint8_t>(0));
            CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), slice->at<std::uint8_t>(1));
            CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), slice->at<std::uint8_t>(2));
            CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), slice->at<std::uint8_t>(3));
            CPPUNIT_ASSERT_EQUAL(value_1, slice->at<std::uint8_t>(4));
            CPPUNIT_ASSERT_EQUAL(value_1, slice->at<std::uint8_t>(5));
            CPPUNIT_ASSERT_EQUAL(value_1, slice->at<std::uint8_t>(6));
            CPPUNIT_ASSERT_EQUAL(value_1, slice->at<std::uint8_t>(7));
        }
    }

    // Slice inside the image, in Y axis, lower layer
    {
        (*axes)[0] = 1.;
        (*axes)[1] = 0.;
        (*axes)[2] = 0.;

        (*axes)[4] = 0.;
        (*axes)[5] = 0.;
        (*axes)[6] = -1.;

        (*axes)[8]  = 0.;
        (*axes)[9]  = 1.;
        (*axes)[10] = 0.;

        (*axes)[3]  = 100.;
        (*axes)[7]  = -200.;
        (*axes)[11] = 20.;

        srv->update().wait();
        auto spacing = slice->spacing();
        CPPUNIT_ASSERT_EQUAL(3.0, spacing[0]);
        CPPUNIT_ASSERT_EQUAL(0.5, spacing[1]);

        auto size = slice->size();
        CPPUNIT_ASSERT_EQUAL(std::size_t(4), size[0]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(5), size[1]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

        auto origin = slice->origin();
        CPPUNIT_ASSERT_EQUAL(100., origin[0]);
        CPPUNIT_ASSERT_EQUAL(-200., origin[1]);
        CPPUNIT_ASSERT_EQUAL(20., origin[2]);

        {
            const auto slice_lock = slice->dump_lock();

            for(std::size_t i = 0 ; i < 20 ; ++i)
            {
                CPPUNIT_ASSERT_EQUAL(value_1, slice->at<std::uint8_t>(i));
            }
        }
    }

    // Slice inside the image, in Y axis, upper layer
    {
        (*axes)[3]  = 100.;
        (*axes)[7]  = -199.75;
        (*axes)[11] = 20.0;

        srv->update().wait();
        auto spacing = slice->spacing();
        CPPUNIT_ASSERT_EQUAL(3.0, spacing[0]);
        CPPUNIT_ASSERT_EQUAL(0.5, spacing[1]);

        auto size = slice->size();
        CPPUNIT_ASSERT_EQUAL(std::size_t(4), size[0]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(5), size[1]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

        auto origin = slice->origin();
        CPPUNIT_ASSERT_EQUAL(100., origin[0]);
        CPPUNIT_ASSERT_EQUAL(-199.75, origin[1]);
        CPPUNIT_ASSERT_EQUAL(20.0, origin[2]);

        {
            const auto slice_lock = slice->dump_lock();
            for(std::size_t i = 0 ; i < 20 ; ++i)
            {
                CPPUNIT_ASSERT_EQUAL(value_2, slice->at<std::uint8_t>(i));
            }
        }
    }

    // Slice inside the image, in Y axis, upper layer reached with the offset
    {
        (*axes)[3]  = 100.;
        (*axes)[7]  = -200.;
        (*axes)[11] = 20.0;

        auto offset = std::make_shared<sight::data::matrix4>();
        (*offset)[11] = -.25;
        srv->set_input(offset, "offset");

        srv->update().wait();
        auto spacing = slice->spacing();
        CPPUNIT_ASSERT_EQUAL(3.0, spacing[0]);
        CPPUNIT_ASSERT_EQUAL(0.5, spacing[1]);

        auto size = slice->size();
        CPPUNIT_ASSERT_EQUAL(std::size_t(4), size[0]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(5), size[1]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

        auto origin = slice->origin();
        CPPUNIT_ASSERT_EQUAL(100., origin[0]);
        CPPUNIT_ASSERT_EQUAL(-199.75, origin[1]);
        CPPUNIT_ASSERT_EQUAL(20.0, origin[2]);

        {
            const auto slice_lock = slice->dump_lock();
            for(std::size_t i = 0 ; i < 20 ; ++i)
            {
                CPPUNIT_ASSERT_EQUAL(value_2, slice->at<std::uint8_t>(i));
            }
        }
    }

    srv->stop().wait();
    sight::service::remove(srv);
}

} // namespace sight::module::filter::image::ut
