/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include "image_center_test.hpp"

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/matrix4.hpp>

#include <geometry/data/matrix4.hpp>

#include <service/op.hpp>

#include <utest_data/generator/image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::filter::image::ut::image_center_test);

namespace sight::module::filter::image::ut
{

//------------------------------------------------------------------------------

void image_center_test::setUp()
{
}

//------------------------------------------------------------------------------

void image_center_test::tearDown()
{
}

//------------------------------------------------------------------------------

void image_center_test::nominal_test()
{
    auto image           = std::make_shared<sight::data::image>();
    const auto dump_lock = image->dump_lock();

    const data::image::size_t size       = {13, 27, 90};
    const data::image::spacing_t spacing = {1.4, 3.5, 0.5};
    const data::image::origin_t origin   = {100., -208.2, 14.};

    // 45° rotation around X axis, 45° rotation around Y axis, 45° rotation around Z axis
    const data::image::orientation_t orientation = {
        0.5000000, -0.5000000, 0.7071068,
        0.8535534, 0.1464466, -0.5000000,
        0.1464466, 0.8535534, 0.5000000
    };

    utest_data::generator::image::generate_image(
        image,
        size,
        spacing,
        origin,
        orientation,
        core::type::get<std::int16_t>(),
        data::image::pixel_format_t::gray_scale
    );

    auto transform = std::make_shared<sight::data::matrix4>();

    // Expected world transform
    const glm::dmat4 expected_world_transform {
        orientation[0], orientation[3], orientation[6], 0,
        orientation[1], orientation[4], orientation[7], 0,
        orientation[2], orientation[5], orientation[8], 0,
        origin[0], origin[1], origin[2], 1
    };

    // Expected image center in mm
    const glm::dvec4 expected_image_center {
        double(size[0]) * spacing[0] / 2.0,
        double(size[1]) * spacing[1] / 2.0,
        double(size[2]) * spacing[2] / 2.0,
        1
    };

    // Compute expected world center
    const auto expected_center = expected_world_transform * expected_image_center;

    // Create service
    auto srv = sight::service::add("sight::module::filter::image::image_center");
    CPPUNIT_ASSERT(srv->is_a("sight::module::filter::image::image_center"));
    CPPUNIT_ASSERT(srv);

    srv->set_worker(sight::core::thread::get_default_worker());
    srv->set_input(image, "image");
    srv->set_inout(transform, "transform");
    srv->configure();
    srv->start().wait();
    srv->update().wait();

    // This should never be the case
    CPPUNIT_ASSERT(!geometry::data::is_identity(*transform));

    // Check back in image that we are at the center
    const auto world_center = transform->position();

    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_center[0], world_center[0], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_center[1], world_center[1], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_center[2], world_center[2], 1e-6);

    // Go back to image
    const auto image_position = image->world_to_image(world_center, true);

    // We use an epsilon of 1, because rounding "errors" is possible, with odd size
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(size[0]) / 2.0, double(image_position[0]), .5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(size[1]) / 2.0, double(image_position[1]), .5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(size[2]) / 2.0, double(image_position[2]), .5);

    srv->stop().wait();
    sight::service::remove(srv);
}

} // namespace sight::module::filter::image::ut
