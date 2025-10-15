/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include <data/image.hpp>

#include <utest_data/generator/image.hpp>

#include <viz/scene3d/utils.hpp>

#include <doctest/doctest.h>

#include <OGRE/OgreColourValue.h>
#include <OGRE/OgrePrerequisites.h>

#include <random>

TEST_SUITE("sight::viz::scene3d::utils")
{
    TEST_CASE("from_ogre_color")
    {
        sight::data::color::sptr ref_color = std::make_shared<sight::data::color>();
        ref_color->set_rgba(1.F, 1.F, 1.F, 1.F);

        sight::data::color::sptr result_color = sight::viz::scene3d::utils::from_ogre_color(Ogre::ColourValue());

        CHECK(static_cast<int>(result_color->red()) == static_cast<int>(ref_color->red()));
        CHECK(static_cast<int>(result_color->green()) == static_cast<int>(ref_color->green()));
        CHECK(static_cast<int>(result_color->blue()) == static_cast<int>(ref_color->blue()));
        CHECK(static_cast<int>(result_color->alpha()) == static_cast<int>(ref_color->alpha()));
    }

    TEST_CASE("convert_ogre_matrix")
    {
        std::mt19937 rng {std::random_device {}()};
        std::uniform_real_distribution<float> dist(-10., 10.);

        // Convert from Sight to ogre and back to Sight.
        {
            sight::data::matrix4::sptr mat0 = std::make_shared<sight::data::matrix4>();
            for(double& coeff : *mat0)
            {
                coeff = dist(rng);
            }

            const Ogre::Matrix4 ogre_mat0 = sight::viz::scene3d::utils::to_ogre_matrix(mat0);
            for(std::uint8_t l = 0 ; l < 4 ; ++l)
            {
                for(std::uint8_t c = 0 ; c < 4 ; ++c)
                {
                    CHECK_EQ((*mat0)(l, c), double(ogre_mat0[l][c]));
                }
            }

            // Convert back to TM3D.
            sight::data::matrix4::sptr mat0_copy = std::make_shared<sight::data::matrix4>();
            sight::viz::scene3d::utils::from_ogre_matrix(ogre_mat0, mat0_copy);
            for(std::uint8_t l = 0 ; l < 4 ; ++l)
            {
                for(std::uint8_t c = 0 ; c < 4 ; ++c)
                {
                    CHECK_EQ((*mat0)(l, c), (*mat0_copy)(l, c));
                    CHECK_EQ(double(ogre_mat0[l][c]), (*mat0_copy)(l, c));
                }
            }
        }

        // Convert from ogre to Sight and back to ogre.
        {
            Ogre::Matrix4 ogre_mat1;
            for(std::uint8_t l = 0 ; l < 4 ; ++l)
            {
                for(std::uint8_t c = 0 ; c < 4 ; ++c)
                {
                    ogre_mat1[l][c] = dist(rng);
                }
            }

            sight::data::matrix4::sptr mat1_copy = std::make_shared<sight::data::matrix4>();
            sight::viz::scene3d::utils::from_ogre_matrix(ogre_mat1, mat1_copy);
            for(std::uint8_t l = 0 ; l < 4 ; ++l)
            {
                for(std::uint8_t c = 0 ; c < 4 ; ++c)
                {
                    CHECK_EQ(double(ogre_mat1[l][c]), (*mat1_copy)(l, c));
                }
            }

            const Ogre::Matrix4 ogre_mat1_copy = sight::viz::scene3d::utils::to_ogre_matrix(mat1_copy);
            for(std::uint8_t l = 0 ; l < 4 ; ++l)
            {
                for(std::uint8_t c = 0 ; c < 4 ; ++c)
                {
                    CHECK_EQ(ogre_mat1[l][c], ogre_mat1_copy[l][c]);
                    CHECK_EQ((*mat1_copy)(l, c), double(ogre_mat1_copy[l][c]));
                }
            }
        }
    }

    TEST_CASE("world_to_slice")
    {
        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        Ogre::Vector3 world_inside_im  = {20., 10., 5.};
        Ogre::Vector3 world_outside_im = {50., -1., 5.};

        // Spacing is 0, throw exception.
        CHECK_THROWS_AS(sight::viz::scene3d::utils::world_to_slices(*image, world_inside_im), sight::core::exception);

        sight::utest_data::generator::image::generate_image(
            image,
            {40, 40, 40},
            {1., 1., 1.},
            {0., 0., 0.},
            {1, 0, 0, 0, 1, 0, 0, 0, 1},
            sight::core::type::UINT8,
            sight::data::image::pixel_format_t::gray_scale
        );

        Ogre::Vector3i slice_idx;
        CHECK_NOTHROW(slice_idx = sight::viz::scene3d::utils::world_to_slices(*image, world_inside_im));
        CHECK_EQ(20, slice_idx[0]);
        CHECK_EQ(10, slice_idx[1]);
        CHECK_EQ(5, slice_idx[2]);

        CHECK_THROWS_AS(sight::viz::scene3d::utils::world_to_slices(*image, world_outside_im), sight::core::exception);
    }
}
