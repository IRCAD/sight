/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "utils_test.hpp"

#include <data/image.hpp>

#include <utest_data/generator/image.hpp>

#include <viz/scene3d/utils.hpp>

#include <OGRE/OgreColourValue.h>
#include <OGRE/OgrePrerequisites.h>

#include <random>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::scene3d::ut::utils_test);

namespace sight::viz::scene3d::ut
{

//------------------------------------------------------------------------------

void utils_test::setUp()
{
}

//------------------------------------------------------------------------------

void utils_test::tearDown()
{
}

//------------------------------------------------------------------------------

void utils_test::from_ogre_color()
{
    data::color::sptr ref_color = std::make_shared<data::color>();
    ref_color->set_rgba(1.F, 1.F, 1.F, 1.F);

    data::color::sptr result_color = viz::scene3d::utils::from_ogre_color(Ogre::ColourValue());
    CPPUNIT_ASSERT(static_cast<int>(result_color->red()) == static_cast<int>(ref_color->red()));
    CPPUNIT_ASSERT(static_cast<int>(result_color->green()) == static_cast<int>(ref_color->green()));
    CPPUNIT_ASSERT(static_cast<int>(result_color->blue()) == static_cast<int>(ref_color->blue()));
    CPPUNIT_ASSERT(static_cast<int>(result_color->alpha()) == static_cast<int>(ref_color->alpha()));
}

//------------------------------------------------------------------------------

void utils_test::convert_ogre_matrix_to_t_m_3d()
{
    std::mt19937 rng {std::random_device {}()};
    std::uniform_real_distribution<float> dist(-10., 10.);

    // Convert from Sight to ogre and back to Sight.
    {
        data::matrix4::sptr mat0 = std::make_shared<data::matrix4>();

        for(double& coeff : *mat0)
        {
            coeff = dist(rng);
        }

        const Ogre::Matrix4 ogre_mat0 = viz::scene3d::utils::to_ogre_matrix(mat0);

        for(std::uint8_t l = 0 ; l < 4 ; ++l)
        {
            for(std::uint8_t c = 0 ; c < 4 ; ++c)
            {
                CPPUNIT_ASSERT_EQUAL((*mat0)(l, c), double(ogre_mat0[l][c]));
            }
        }

        // Convert back to TM3D.
        data::matrix4::sptr mat0_copy = std::make_shared<data::matrix4>();

        viz::scene3d::utils::from_ogre_matrix(ogre_mat0, mat0_copy);

        for(std::uint8_t l = 0 ; l < 4 ; ++l)
        {
            for(std::uint8_t c = 0 ; c < 4 ; ++c)
            {
                CPPUNIT_ASSERT_EQUAL((*mat0)(l, c), (*mat0_copy)(l, c));
                CPPUNIT_ASSERT_EQUAL(double(ogre_mat0[l][c]), (*mat0_copy)(l, c));
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

        data::matrix4::sptr mat1_copy = std::make_shared<data::matrix4>();
        viz::scene3d::utils::from_ogre_matrix(ogre_mat1, mat1_copy);

        for(std::uint8_t l = 0 ; l < 4 ; ++l)
        {
            for(std::uint8_t c = 0 ; c < 4 ; ++c)
            {
                CPPUNIT_ASSERT_EQUAL(double(ogre_mat1[l][c]), (*mat1_copy)(l, c));
            }
        }

        const Ogre::Matrix4 ogre_mat1_copy = viz::scene3d::utils::to_ogre_matrix(mat1_copy);

        for(std::uint8_t l = 0 ; l < 4 ; ++l)
        {
            for(std::uint8_t c = 0 ; c < 4 ; ++c)
            {
                CPPUNIT_ASSERT_EQUAL(ogre_mat1[l][c], ogre_mat1_copy[l][c]);
                CPPUNIT_ASSERT_EQUAL((*mat1_copy)(l, c), double(ogre_mat1_copy[l][c]));
            }
        }
    }
}

//------------------------------------------------------------------------------

void utils_test::world_to_slice_test()
{
    data::image::sptr image = std::make_shared<data::image>();

    Ogre::Vector3 world_inside_im  = {20., 10., 5.};
    Ogre::Vector3 world_outside_im = {50., -1., 5.};

    // Spacing is 0, throw exception.
    CPPUNIT_ASSERT_THROW(utils::world_to_slices(*image, world_inside_im), core::exception);

    utest_data::generator::image::generate_image(
        image,
        {40, 40, 40},
        {1., 1., 1.},
        {0., 0., 0.},
        core::type::UINT8,
        data::image::pixel_format_t::gray_scale
    );

    Ogre::Vector3i slice_idx;

    CPPUNIT_ASSERT_NO_THROW(slice_idx = utils::world_to_slices(*image, world_inside_im));

    CPPUNIT_ASSERT_EQUAL(20, slice_idx[0]);
    CPPUNIT_ASSERT_EQUAL(10, slice_idx[1]);
    CPPUNIT_ASSERT_EQUAL(5, slice_idx[2]);

    CPPUNIT_ASSERT_THROW(utils::world_to_slices(*image, world_outside_im), core::exception);
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::ut
