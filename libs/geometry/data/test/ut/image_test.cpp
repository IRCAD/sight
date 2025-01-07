/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <geometry/data/image.hpp>

#include <utest_data/generator/image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::geometry::data::ut::image_test);

namespace sight::geometry::data::ut
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

void image_test::transform_test()
{
    {
        auto image = std::make_shared<sight::data::image>();
        image->set_spacing({1.0, 1.0, 1.0});
        image->set_origin({0, 0, 0});
        image->set_orientation({1, 0, 0, 0, 1, 0, 0, 0, 1});

        const auto& image_to_world_trf = geometry::data::image_to_world_transform(*image);
        const glm::dmat4 expected_trf  = {
            1., 0., 0., 0.,
            0., 1., 0., 0.,
            0., 0., 1., 0.,
            0., 0., 0., 1.,
        };
        CPPUNIT_ASSERT(image_to_world_trf == expected_trf);

        const auto& world_to_image_trf = geometry::data::world_to_image_transform(*image);
        CPPUNIT_ASSERT(world_to_image_trf == expected_trf);
    }
    {
        auto image = std::make_shared<sight::data::image>();
        image->set_spacing({1.0, 0.2, 11.0});
        image->set_origin({12.2, 4, -2});
        image->set_orientation({0, 1, 0, -1, 0, 0, 0, 0, 1});

        const auto& image_to_world_trf = geometry::data::image_to_world_transform(*image);

        glm::dmat4 expected_trf = {
            0., -1.0, 0., 0.,
            0.2, 0.0, 0., 0.,
            0., 0., 11., 0.,
            12.2, 4., -2., 1.,
        };
        CPPUNIT_ASSERT(image_to_world_trf == expected_trf);

        const auto& world_to_image_trf = geometry::data::world_to_image_transform(*image);

        expected_trf = glm::inverse(expected_trf);

        for(int i = 0 ; i < 4 ; ++i)
        {
            for(int j = 0 ; j < 4 ; ++j)
            {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(world_to_image_trf[i][j], expected_trf[i][j], 1e-5);
            }
        }
    }
}

//------------------------------------------------------------------------------

void image_test::transform_coordinates_test()
{
    // Origin at (0, 0, 0) with no rotation
    // Image(0, 0, 0) -> world(0, 0, 0)
    // World(0, 0, 0) -> image(0, 0, 0)
    {
        auto image = std::make_shared<sight::data::image>();
        image->set_spacing({1.0, 1.0, 1.0});
        image->set_origin({0, 0, 0});
        image->set_orientation({1, 0, 0, 0, 1, 0, 0, 0, 1});

        const auto& actual_world = geometry::data::image_to_world(*image, {0, 0, 0});
        const glm::dvec3 expected_world {0.0, 0.0, 0.0};
        CPPUNIT_ASSERT_EQUAL(expected_world[0], actual_world[0]);
        CPPUNIT_ASSERT_EQUAL(expected_world[1], actual_world[1]);
        CPPUNIT_ASSERT_EQUAL(expected_world[2], actual_world[2]);

        const auto actual_image = geometry::data::world_to_image(*image, {0, 0, 0});
        const glm::ivec3 expected_image {0, 0, 0};
        CPPUNIT_ASSERT_EQUAL(expected_image[0], actual_image[0]);
        CPPUNIT_ASSERT_EQUAL(expected_image[1], actual_image[1]);
        CPPUNIT_ASSERT_EQUAL(expected_image[2], actual_image[2]);
    }

    // Origin at (2, 3, 4) with no rotation
    // Image(0, 0, 0) -> world(2, 3, 4)
    // World(2, 3, 4) -> image(0, 0, 0)
    {
        auto image = std::make_shared<sight::data::image>();
        image->set_spacing({1.0, 1.0, 1.0});
        image->set_origin({2.0, 3.0, 4.0});
        image->set_orientation({1, 0, 0, 0, 1, 0, 0, 0, 1});

        const auto& actual_world = geometry::data::image_to_world(*image, {0, 0, 0});
        const glm::dvec3 expected_world {2.0, 3.0, 4.0};
        CPPUNIT_ASSERT_EQUAL(expected_world[0], actual_world[0]);
        CPPUNIT_ASSERT_EQUAL(expected_world[1], actual_world[1]);
        CPPUNIT_ASSERT_EQUAL(expected_world[2], actual_world[2]);

        const auto& actual_image = geometry::data::world_to_image(*image, {2.0, 3.0, 4.0});
        const glm::ivec3 expected_image {0, 0, 0};
        CPPUNIT_ASSERT_EQUAL(expected_image[0], actual_image[0]);
        CPPUNIT_ASSERT_EQUAL(expected_image[1], actual_image[1]);
        CPPUNIT_ASSERT_EQUAL(expected_image[2], actual_image[2]);
    }

    // Origin at (2, 3, 4) with 90° X rotation
    // Image(0, 0, 0) -> world(2, 3, 4)
    // World(2, 3, 4) -> image(0, 0, 0)
    {
        auto image = std::make_shared<sight::data::image>();

        image->set_spacing({1.0, 1.0, 1.0});
        image->set_origin({2.0, 3.0, 4.0});

        // 90° rotation around X axis
        image->set_orientation({1, 0, 0, 0, 0, -1, 0, 1, 0});

        const auto& actual_world = geometry::data::image_to_world(*image, {0, 0, 0});
        const glm::dvec3 expected_world {2.0, 3.0, 4.0};
        CPPUNIT_ASSERT_EQUAL(expected_world[0], actual_world[0]);
        CPPUNIT_ASSERT_EQUAL(expected_world[1], actual_world[1]);
        CPPUNIT_ASSERT_EQUAL(expected_world[2], actual_world[2]);

        const auto& actual_image = geometry::data::world_to_image(*image, {2.0, 3.0, 4.0});
        const glm::ivec3 expected_image {0, 0, 0};
        CPPUNIT_ASSERT_EQUAL(expected_image[0], actual_image[0]);
        CPPUNIT_ASSERT_EQUAL(expected_image[1], actual_image[1]);
        CPPUNIT_ASSERT_EQUAL(expected_image[2], actual_image[2]);
    }

    // Origin at (2, 3, 4) with 90° X rotation
    // Image(5, 6, 7) -> world(7, -4, 10)
    // World(5, 6, 7) -> image(3, 3, -3)
    {
        auto image = std::make_shared<sight::data::image>();
        image->set_spacing({1.0, 1.0, 1.0});
        image->set_origin({2.0, 3.0, 4.0});

        // 90° rotation around X axis
        image->set_orientation({1, 0, 0, 0, 0, -1, 0, 1, 0});

        const auto& actual_world = geometry::data::image_to_world(*image, {5, 6, 7});
        const glm::dvec3 expected_world {7.0, -4.0, 10.0};
        CPPUNIT_ASSERT_EQUAL(expected_world[0], actual_world[0]);
        CPPUNIT_ASSERT_EQUAL(expected_world[1], actual_world[1]);
        CPPUNIT_ASSERT_EQUAL(expected_world[2], actual_world[2]);

        const auto& actual_image = geometry::data::world_to_image(*image, {5.0, 6.0, 7.0});
        const glm::ivec3 expected_image {3, 3, -3};
        CPPUNIT_ASSERT_EQUAL(expected_image[0], actual_image[0]);
        CPPUNIT_ASSERT_EQUAL(expected_image[1], actual_image[1]);
        CPPUNIT_ASSERT_EQUAL(expected_image[2], actual_image[2]);
    }

    // Rounding test
    {
        auto image = std::make_shared<sight::data::image>();
        image->resize({567, 789, 39}, core::type::INT16, sight::data::image::gray_scale);
        image->set_spacing({1.4, 0.8, 0.5});
        image->set_origin({-18.3, 54.0, 230.0});

        {
            const auto indices = geometry::data::world_to_image(*image, {16, 543.65, 456.}, true);
            const glm::ivec3 expected_indices {25, 612, 452};

            CPPUNIT_ASSERT_EQUAL(expected_indices[0], indices[0]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[1], indices[1]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[2], indices[2]);
        }

        {
            const auto indices = geometry::data::world_to_image(*image, {2., 34.4, 7.}, true);
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
            const auto indices = geometry::data::world_to_image(*image, {16, 543.65, 456.}, true);
            const glm::ivec3 expected_indices {25, 283, -979};

            CPPUNIT_ASSERT_EQUAL(expected_indices[0], indices[0]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[1], indices[1]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[2], indices[2]);
        }

        {
            const auto indices = geometry::data::world_to_image(*image, {2., 34.4, 7.}, true);
            const glm::ivec3 expected_indices {15, -279, 39};

            CPPUNIT_ASSERT_EQUAL(expected_indices[0], indices[0]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[1], indices[1]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[2], indices[2]);
        }
    }

    // Clamp test
    {
        const sight::data::image::size_t size {100, 110, 120};
        const sight::data::image::spacing_t spacing {0.5, 0.6, 0.7};
        const sight::data::image::origin_t origin {1, 2, 3};

        auto image = std::make_shared<sight::data::image>();

        image->resize(size, core::type::INT8, sight::data::image::gray_scale);
        image->set_spacing(spacing);
        image->set_origin(origin);

        // Lower bound
        {
            const auto indices = geometry::data::world_to_image(*image, origin, false, true);
            const glm::ivec3 expected_indices {0, 0, 0};

            CPPUNIT_ASSERT_EQUAL(expected_indices[0], indices[0]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[1], indices[1]);
            CPPUNIT_ASSERT_EQUAL(expected_indices[2], indices[2]);
        }

        {
            const auto indices = geometry::data::world_to_image(
                *image,
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
            const auto indices = geometry::data::world_to_image(
                *image,
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
            const auto indices = geometry::data::world_to_image(
                *image,
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
            const auto indices = geometry::data::world_to_image(
                *image,
                {upper_bound_x, upper_bound_y, upper_bound_z},
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
            const auto indices = geometry::data::world_to_image(
                *image,
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
            const auto indices = geometry::data::world_to_image(
                *image,
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
            const auto image_to_world = geometry::data::image_to_world(*image, size);
            const auto indices        = geometry::data::world_to_image(*image, image_to_world, false, false);

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

//------------------------------------------------------------------------------

sight::data::image::sptr generate_image()
{
    auto image = std::make_shared<sight::data::image>();

    const sight::data::image::size_t size               = {256, 150, 100};
    const sight::data::image::spacing_t spacing         = {1., 1., 0.5};
    const sight::data::image::origin_t origin           = {0., 0., 0.};
    const sight::data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
    utest_data::generator::image::generate_image(
        image,
        size,
        spacing,
        origin,
        orientation,
        core::type::UINT8,
        sight::data::image::pixel_format_t::gray_scale
    );

    return image;
}

//------------------------------------------------------------------------------

void image_test::slice_index_fiducial_test()
{
    const auto image = generate_image();

    // No rotation
    image->set_orientation({1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0});
    const std::array<double, 3> point1 = {1.0, 2.0, 3.0};

    namespace  med_im_helper = sight::data::helper::medical_image;
    {
        auto axis = med_im_helper::axis_t::sagittal;

        auto index = get_fiducial_slice_index(*image, point1, axis);
        CPPUNIT_ASSERT_EQUAL(true, index.has_value());
        CPPUNIT_ASSERT_EQUAL(std::int64_t(1), index.value());
    }

    {
        auto axis = med_im_helper::axis_t::frontal;

        auto index = get_fiducial_slice_index(*image, point1, axis);
        CPPUNIT_ASSERT_EQUAL(true, index.has_value());
        CPPUNIT_ASSERT_EQUAL(std::int64_t(2), index.value());
    }

    {
        auto axis = med_im_helper::axis_t::axial;

        auto index = get_fiducial_slice_index(*image, point1, axis);
        CPPUNIT_ASSERT_EQUAL(true, index.has_value());
        CPPUNIT_ASSERT_EQUAL(std::int64_t(6), index.value());
    }

    // 90° rotation around X axis
    image->set_orientation({1, 0, 0, 0, 0, -1, 0, 1, 0});
    const std::array<double, 3> point2 {1.0, -3.0, 2.0};

    {
        auto axis = med_im_helper::axis_t::sagittal;

        auto index = get_fiducial_slice_index(*image, point2, axis);
        CPPUNIT_ASSERT_EQUAL(true, index.has_value());
        CPPUNIT_ASSERT_EQUAL(std::int64_t(1), index.value());
    }

    {
        auto axis = med_im_helper::axis_t::frontal;

        auto index = get_fiducial_slice_index(*image, point2, axis);
        CPPUNIT_ASSERT_EQUAL(true, index.has_value());
        CPPUNIT_ASSERT_EQUAL(std::int64_t(2), index.value());
    }

    {
        auto axis = med_im_helper::axis_t::axial;

        auto index = get_fiducial_slice_index(*image, point2, axis);
        CPPUNIT_ASSERT_EQUAL(true, index.has_value());
        CPPUNIT_ASSERT_EQUAL(std::int64_t(6), index.value());
    }
}

} // namespace sight::geometry::data::ut
