/************************************************************************
 *
 * Copyright (C) 2017-2025 IRCAD France
 * Copyright (C) 2017-2021 IHU Strasbourg
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

#include "point_list_test.hpp"

#include <core/exception.hpp>

#include <data/point.hpp>

#include <geometry/data/point_list.hpp>

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include <random>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::geometry::data::ut::point_list_test);

namespace sight::geometry::data::ut
{

//------------------------------------------------------------------------------

void point_list_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void point_list_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void point_list_test::compute_distance()
{
    const std::size_t nb_points = 42;

    sight::data::point_list::sptr pl1;
    sight::data::point_list::sptr pl2;

    sight::data::point::sptr p;

    // Simple test with empty point lists
    {
        pl1 = std::make_shared<sight::data::point_list>();
        pl2 = std::make_shared<sight::data::point_list>();

        // Compare the point lists
        auto output_array = geometry::data::point_list::compute_distance(pl1, pl2);

        CPPUNIT_ASSERT(output_array->num_elements() == 0);
    }

    // Simple test with parallel point lists
    {
        pl1 = std::make_shared<sight::data::point_list>();
        pl2 = std::make_shared<sight::data::point_list>();

        // Build 2 pointlists:
        // The first one with increasing x values
        // And the second one with inscreasing x values but shifted in y
        for(std::size_t i = 0 ; i < nb_points ; i++)
        {
            p = std::make_shared<sight::data::point>(static_cast<float>(i), 0.0F, 0.0F);
            pl1->push_back(p);

            p = std::make_shared<sight::data::point>(static_cast<float>(i), 1.0F, 0.0F);
            pl2->push_back(p);
        }

        // Compare the point lists
        auto output_array = geometry::data::point_list::compute_distance(pl1, pl2);

        const auto dump_lock    = output_array->dump_lock();
        auto distance_array_itr = output_array->begin<double>();

        for(std::size_t i = 0 ; i < nb_points ; i++, ++distance_array_itr)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(*distance_array_itr, 1.0, 1e-8);
        }
    }

    // Simple test with diverging point lists
    {
        pl1 = std::make_shared<sight::data::point_list>();
        pl2 = std::make_shared<sight::data::point_list>();

        // Build 2 point lists:
        // The first one with increasing x values
        // And the second one with increasing x values but shifted in y
        for(std::size_t i = 0 ; i < nb_points ; i++)
        {
            p = std::make_shared<sight::data::point>(static_cast<float>(i), 0.0F, 0.0F);
            pl1->push_back(p);

            p = std::make_shared<sight::data::point>(static_cast<float>(i), static_cast<float>(i), 0.0F);
            pl2->push_back(p);
        }

        // Compare the point lists
        auto output_array    = geometry::data::point_list::compute_distance(pl1, pl2);
        const auto dump_lock = output_array->dump_lock();

        auto distance_array_itr = output_array->begin<double>();

        for(std::size_t i = 0 ; i < nb_points ; i++, ++distance_array_itr)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(*distance_array_itr, static_cast<double>(i), 1e-8);
        }
    }
}

//------------------------------------------------------------------------------

void point_list_test::transform()
{
    // Simple test with identity
    {
        // Test sample
        sight::data::point_list::sptr pl1 = std::make_shared<sight::data::point_list>();
        pl1->push_back(std::make_shared<sight::data::point>(0.0F, 0.0F, 0.0F));
        pl1->push_back(std::make_shared<sight::data::point>(1.0F, 0.0F, 0.0F));
        pl1->push_back(std::make_shared<sight::data::point>(1.0F, 1.0F, 0.0F));
        pl1->push_back(std::make_shared<sight::data::point>(0.0F, 1.0F, 0.0F));

        pl1->push_back(std::make_shared<sight::data::point>(0.0F, 0.0F, 1.0F));
        pl1->push_back(std::make_shared<sight::data::point>(1.0F, 0.0F, 1.0F));
        pl1->push_back(std::make_shared<sight::data::point>(1.0F, 1.0F, 1.0F));
        pl1->push_back(std::make_shared<sight::data::point>(0.0F, 1.0F, 1.0F));

        // Reference list
        const sight::data::point_list::container_t points1 = pl1->get_points();
        const std::size_t size                             = points1.size();

        sight::data::point_list::sptr pl2 = std::make_shared<sight::data::point_list>();
        for(std::size_t i = 0 ; i < size ; i++)
        {
            const auto& tmp = (*points1[i]);
            pl2->push_back(std::make_shared<sight::data::point>(tmp[0], tmp[1], tmp[2]));
        }

        const auto tf1 = std::make_shared<sight::data::matrix4>();
        geometry::data::point_list::transform(pl1, tf1);

        const sight::data::point_list::container_t points2 = pl2->get_points();
        for(std::size_t i = 0 ; i < size ; i++)
        {
            const auto& tmp1 = (*points1[i]);
            const auto& tmp2 = (*points2[i]);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[0], tmp2[0], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[1], tmp2[1], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[2], tmp2[2], 1e-8);
        }
    }

    // Simple test with translation
    {
        std::vector<float> translation(3, 0.0F);
        translation[0] = 8.0;
        translation[1] = 16.0;
        translation[2] = 32.0;

        // Test sample
        sight::data::point_list::sptr pl1 = std::make_shared<sight::data::point_list>();
        pl1->push_back(std::make_shared<sight::data::point>(0.0F, 0.0F, 0.0F));
        pl1->push_back(std::make_shared<sight::data::point>(1.0F, 0.0F, 0.0F));
        pl1->push_back(std::make_shared<sight::data::point>(1.0F, 1.0F, 0.0F));
        pl1->push_back(std::make_shared<sight::data::point>(0.0F, 1.0F, 0.0F));

        pl1->push_back(std::make_shared<sight::data::point>(0.0F, 0.0F, 1.0F));
        pl1->push_back(std::make_shared<sight::data::point>(1.0F, 0.0F, 1.0F));
        pl1->push_back(std::make_shared<sight::data::point>(1.0F, 1.0F, 1.0F));
        pl1->push_back(std::make_shared<sight::data::point>(0.0F, 1.0F, 1.0F));

        // Reference list
        const sight::data::point_list::container_t points1 = pl1->get_points();
        const std::size_t size                             = points1.size();

        sight::data::point_list::sptr pl2 = std::make_shared<sight::data::point_list>();
        for(std::size_t i = 0 ; i < size ; i++)
        {
            const auto& tmp = (*points1[i]);
            pl2->push_back(
                std::make_shared<sight::data::point>(
                    tmp[0] + translation[0],
                    tmp[1] + translation[1],
                    tmp[2] + translation[2]
                )
            );
        }

        const auto tf1 = std::make_shared<sight::data::matrix4>();
        (*tf1)(0, 3) = translation[0];
        (*tf1)(1, 3) = translation[1];
        (*tf1)(2, 3) = translation[2];
        geometry::data::point_list::transform(pl1, tf1);

        const sight::data::point_list::container_t points2 = pl2->get_points();
        for(std::size_t i = 0 ; i < size ; i++)
        {
            const auto& tmp1 = (*points1[i]);
            const auto& tmp2 = (*points2[i]);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[0], tmp2[0], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[1], tmp2[1], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[2], tmp2[2], 1e-8);
        }
    }

    // Simple test with rotation
    {
        sight::data::point_list::sptr pl1 = std::make_shared<sight::data::point_list>();
        pl1->push_back(std::make_shared<sight::data::point>(0.0F, 0.0F, 0.0F));
        pl1->push_back(std::make_shared<sight::data::point>(1.0F, 0.0F, 0.0F));
        pl1->push_back(std::make_shared<sight::data::point>(1.0F, 1.0F, 0.0F));
        pl1->push_back(std::make_shared<sight::data::point>(0.0F, 1.0F, 0.0F));

        pl1->push_back(std::make_shared<sight::data::point>(0.0F, 0.0F, 1.0F));
        pl1->push_back(std::make_shared<sight::data::point>(1.0F, 0.0F, 1.0F));
        pl1->push_back(std::make_shared<sight::data::point>(1.0F, 1.0F, 1.0F));
        pl1->push_back(std::make_shared<sight::data::point>(0.0F, 1.0F, 1.0F));

        sight::data::point_list::sptr pl2 = std::make_shared<sight::data::point_list>();
        pl2->push_back(std::make_shared<sight::data::point>(0.0F, 0.0F, 0.0F));
        pl2->push_back(std::make_shared<sight::data::point>(-1.0F, 0.0F, 0.0F));
        pl2->push_back(std::make_shared<sight::data::point>(-1.0F, -1.0F, 0.0F));
        pl2->push_back(std::make_shared<sight::data::point>(0.0F, -1.0F, 0.0F));

        pl2->push_back(std::make_shared<sight::data::point>(0.0F, 0.0F, 1.0F));
        pl2->push_back(std::make_shared<sight::data::point>(-1.0F, 0.0F, 1.0F));
        pl2->push_back(std::make_shared<sight::data::point>(-1.0F, -1.0F, 1.0F));
        pl2->push_back(std::make_shared<sight::data::point>(0.0F, -1.0F, 1.0F));

        // Perform a 180 degrees rotation around Z
        const auto tf1 = std::make_shared<sight::data::matrix4>();
        (*tf1)(0, 0) = -1.0F;
        (*tf1)(0, 1) = 0.0F;
        (*tf1)(1, 0) = 0.0F;
        (*tf1)(1, 1) = -1.0F;

        geometry::data::point_list::transform(pl1, tf1);

        const sight::data::point_list::container_t points1 = pl1->get_points();
        std::size_t size                                   = points1.size();
        const sight::data::point_list::container_t points2 = pl2->get_points();

        for(std::size_t i = 0 ; i < size ; i++)
        {
            const auto& tmp1 = (*points1[i]);
            const auto& tmp2 = (*points2[i]);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[0], tmp2[0], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[1], tmp2[1], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[2], tmp2[2], 1e-8);
        }
    }
}

//------------------------------------------------------------------------------

void point_list_test::associate()
{
    const std::size_t nb_points = 42;

    sight::data::point_list::sptr pl1;
    sight::data::point_list::sptr pl2;

    sight::data::point::sptr p;

    // Simple test with empty point lists
    {
        pl1 = std::make_shared<sight::data::point_list>();
        pl2 = std::make_shared<sight::data::point_list>();

        // Associate empty point lists
        geometry::data::point_list::associate(pl1, pl2);

        // No results expected
    }

    // Test with simple matrices
    // Create two lists with the same sets of points and shift them with transformation matrices
    // Associating them should make the x components match
    {
        pl1 = std::make_shared<sight::data::point_list>();
        pl2 = std::make_shared<sight::data::point_list>();

        // Build 2 point lists with the same points, the point are in the inverse order in the second list
        for(std::size_t i = 0 ; i <= nb_points ; i++)
        {
            p = std::make_shared<sight::data::point>(static_cast<float>(i), 0.0F, 0.0F);
            pl1->push_back(p);

            p = std::make_shared<sight::data::point>(static_cast<float>(nb_points - i), 0.0F, 0.0F);
            pl2->push_back(p);
        }

        // Transform the point lists, shift the points in y
        auto tf1 = std::make_shared<sight::data::matrix4>();
        (*tf1)(1, 3) = 42.0;
        auto tf2 = std::make_shared<sight::data::matrix4>();
        (*tf2)(1, 3) = -42.0;

        geometry::data::point_list::transform(pl1, tf1);
        geometry::data::point_list::transform(pl2, tf2);

        // Associate the point lists
        geometry::data::point_list::associate(pl1, pl2);

        // Check that the two list are equal (re-ordered)
        // Only the last component should differ, as there is a (0,0,42) translation
        // on the first one and a (0,0,-42) translation on the second one
        const sight::data::point_list::container_t points1 = pl1->get_points();
        const sight::data::point_list::container_t points2 = pl2->get_points();

        const std::size_t size = points1.size();

        for(std::size_t i = 0 ; i < size ; i++)
        {
            const auto& tmp1 = (*points1[i]);
            const auto& tmp2 = (*points2[i]);

            // Check that the last component is equal to i
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[0], tmp2[0], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[0], static_cast<float>(i), 1e-8);

            // Check that the second component is equal to 42
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[1], -tmp2[1], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[1], static_cast<float>(42), 1e-8);

            // Check that the last component is equal to 0
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[2], tmp2[2], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[2], 0.0, 1e-8);
        }
    }

    // Test with random point lists
    // Fill two point lists with random points
    // Check that the matched points are really the closest
    {
        // Create an initial octahedron with points
        pl1 = std::make_shared<sight::data::point_list>();
        pl1->push_back(std::make_shared<sight::data::point>(1.0F, 0.0F, 0.0F));
        pl1->push_back(std::make_shared<sight::data::point>(2.0F, 0.0F, 1.0F));
        pl1->push_back(std::make_shared<sight::data::point>(2.0F, 0.8F, 0.0F));
        pl1->push_back(std::make_shared<sight::data::point>(2.0F, 0.0F, -1.0F));
        pl1->push_back(std::make_shared<sight::data::point>(2.0F, -0.8F, 0.0F));
        pl1->push_back(std::make_shared<sight::data::point>(3.0F, 0.0F, 0.0F));

        // Reference final octahedron
        pl2 = std::make_shared<sight::data::point_list>();
        pl2->push_back(std::make_shared<sight::data::point>(0.0F, 0.0F, 5.2F));
        pl2->push_back(std::make_shared<sight::data::point>(1.0F, 0.0F, 6.2F));
        pl2->push_back(std::make_shared<sight::data::point>(0.0F, 0.8F, 6.2F));
        pl2->push_back(std::make_shared<sight::data::point>(-1.0F, 0.0F, 6.2F));
        pl2->push_back(std::make_shared<sight::data::point>(0.0F, -0.8F, 6.2F));
        pl2->push_back(std::make_shared<sight::data::point>(0.0F, 0.0F, 7.2F));

        // Transform the point list
        auto tf1 = std::make_shared<sight::data::matrix4>();
        // Shift the points in Z
        (*tf1)(2, 3) = 4.2;

        // Add a 90 degrees rotation around Y
        (*tf1)(0, 0) = 0.0F;
        (*tf1)(0, 2) = 1.0F;
        (*tf1)(2, 0) = 1.0F;
        (*tf1)(2, 2) = 0.0F;

        geometry::data::point_list::transform(pl1, tf1);

        // Associate the point lists
        geometry::data::point_list::associate(pl1, pl2);

        // Check that the two lists are equal
        // The second one corresponds to a Z translation + a 90 degrees Y translation of the first one
        const sight::data::point_list::container_t points1 = pl1->get_points();
        const sight::data::point_list::container_t points2 = pl2->get_points();

        const std::size_t size  = points1.size();
        const int nb_components = 3;

        for(std::size_t i = 0 ; i < size ; i++)
        {
            const auto& tmp1 = (*points1[i]);
            const auto& tmp2 = (*points2[i]);

            // Compare the components
            for(std::size_t j = 0 ; j < nb_components ; j++)
            {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[j], tmp2[j], 1e-5);
            }
        }
    }
}

//------------------------------------------------------------------------------

void point_list_test::remove_closest_point_nominal()
{
    const std::size_t nb_points      = 42;
    sight::data::point_list::sptr pl = std::make_shared<sight::data::point_list>();

    // Remove points in an empty list
    for(std::size_t i = 0 ; i < nb_points ; ++i)
    {
        const auto p =
            std::make_shared<sight::data::point>(static_cast<float>(i), static_cast<float>(i), static_cast<float>(i));
        CPPUNIT_ASSERT(
            geometry::data::point_list::remove_closest_point(
                pl,
                p,
                std::numeric_limits<float>::min()
            )
            == nullptr
        );
    }

    // Build a list
    for(std::size_t i = 0 ; i < nb_points ; ++i)
    {
        const auto p =
            std::make_shared<sight::data::point>(static_cast<float>(i), static_cast<float>(i), static_cast<float>(i));
        pl->push_back(p);
    }

    // Remove points with an unmatched delta
    for(std::size_t i = 0 ; i < nb_points ; ++i)
    {
        const auto p = std::make_shared<sight::data::point>(
            static_cast<float>(nb_points + 1),
            static_cast<float>(nb_points + 1),
            static_cast<float>(nb_points + 1)
        );
        CPPUNIT_ASSERT(
            geometry::data::point_list::remove_closest_point(
                pl,
                p,
                std::numeric_limits<float>::min()
            )
            == nullptr
        );
    }

    // Remove points
    for(std::size_t i = 0 ; i < nb_points ; ++i)
    {
        const auto p =
            std::make_shared<sight::data::point>(static_cast<float>(i), static_cast<float>(i), static_cast<float>(i));
        const auto p_res =
            geometry::data::point_list::remove_closest_point(pl, p, std::numeric_limits<float>::max());
        CPPUNIT_ASSERT(p_res != nullptr);
        CPPUNIT_ASSERT_EQUAL((*p_res)[0], (*p)[0]);
        CPPUNIT_ASSERT_EQUAL((*p_res)[1], (*p)[1]);
        CPPUNIT_ASSERT_EQUAL((*p_res)[2], (*p)[2]);
    }
}

//------------------------------------------------------------------------------

void point_list_test::remove_closest_point_extreme()
{
    const std::size_t nb_points      = 42;
    sight::data::point_list::sptr pl = std::make_shared<sight::data::point_list>();

    // Build a list
    for(std::size_t i = 0 ; i < nb_points ; ++i)
    {
        const auto p =
            std::make_shared<sight::data::point>(static_cast<float>(i), static_cast<float>(i), static_cast<float>(i));
        pl->push_back(p);
    }

    // Remove points with negative delta
    for(std::size_t i = 0 ; i < nb_points ; ++i)
    {
        const auto p =
            std::make_shared<sight::data::point>(static_cast<float>(i), static_cast<float>(i), static_cast<float>(i));
        CPPUNIT_ASSERT(
            geometry::data::point_list::remove_closest_point(
                pl,
                p,
                -std::numeric_limits<float>::max()
            )
            == nullptr
        );
    }

    // Remove points with biggest delta
    for(std::size_t i = 0 ; i < nb_points ; ++i)
    {
        const auto p =
            std::make_shared<sight::data::point>(static_cast<float>(i), static_cast<float>(i), static_cast<float>(i));
        const auto p_res =
            geometry::data::point_list::remove_closest_point(pl, p, std::numeric_limits<float>::max());
        CPPUNIT_ASSERT(p_res != nullptr);
        CPPUNIT_ASSERT_EQUAL((*p_res)[0], (*p)[0]);
        CPPUNIT_ASSERT_EQUAL((*p_res)[1], (*p)[1]);
        CPPUNIT_ASSERT_EQUAL((*p_res)[2], (*p)[2]);
    }
}

} // namespace sight::geometry::data::ut
