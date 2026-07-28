/************************************************************************
 *
 * Copyright (C) 2024-2026 IRCAD France
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

#include <core/type.hpp>

#include <data/image.hpp>
#include <data/matrix4.hpp>

#include <geometry/data/image.hpp>
#include <geometry/data/matrix4.hpp>

#include <service/op.hpp>

#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::module::filter::image::image_center")
{
    TEST_CASE("nominal")
    {
        auto image           = std::make_shared<sight::data::image>();
        const auto dump_lock = image->dump_lock();

        const sight::data::image::size_t size       = {13, 27, 90};
        const sight::data::image::spacing_t spacing = {1.4, 3.5, 0.5};
        const sight::data::image::origin_t origin   = {100., -208.2, 14.};

        // 45° rotation around X axis, 45° rotation around Y axis, 45° rotation around Z axis
        const sight::data::image::orientation_t orientation = {
            0.5000000, -0.5000000, 0.7071068,
            0.8535534, 0.1464466, -0.5000000,
            0.1464466, 0.8535534, 0.5000000
        };

        sight::utest_data::generator::image::generate_image(
            image,
            size,
            spacing,
            origin,
            orientation,
            sight::core::type::get<std::int16_t>(),
            sight::data::image::pixel_format_t::gray_scale
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
            static_cast<double>(size[0]) * spacing[0] / 2.0,
            static_cast<double>(size[1]) * spacing[1] / 2.0,
            static_cast<double>(size[2]) * spacing[2] / 2.0,
            1
        };

        // Compute expected world center
        const auto expected_center = expected_world_transform * expected_image_center;

        // Create service
        auto srv = sight::service::add("sight::module::filter::image::image_center");
        CHECK(srv->is_a("sight::module::filter::image::image_center"));
        CHECK(srv);

        srv->set_worker(sight::core::thread::get_default_worker());
        srv->set_input(image, "image");
        srv->set_inout(transform, "transform");
        srv->configure();
        srv->start().get();
        srv->update().get();

        // This should never be the case
        CHECK(!sight::geometry::data::is_identity(*transform));

        // Check back in image that we are at the center
        const auto world_center = transform->position();

        CHECK(expected_center[0] == doctest::Approx(world_center[0]).epsilon(1e-6));
        CHECK(expected_center[1] == doctest::Approx(world_center[1]).epsilon(1e-6));
        CHECK(expected_center[2] == doctest::Approx(world_center[2]).epsilon(1e-6));

        // Go back to image
        const auto image_position = sight::geometry::data::world_to_image(*image, world_center, true);

        // We use an epsilon of 1, because rounding "errors" is possible, with odd size
        CHECK(double(size[0]) / 2.0 == doctest::Approx(double(image_position[0])).epsilon(.5));
        CHECK(double(size[1]) / 2.0 == doctest::Approx(double(image_position[1])).epsilon(.5));
        CHECK(double(size[2]) / 2.0 == doctest::Approx(double(image_position[2])).epsilon(.5));

        srv->stop().get();
        sight::service::remove(srv);
    }
}
