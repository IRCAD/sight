/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#include "mesh_test.hpp"

#include <core/tools/random/generator.hpp>

#include <viz/scene3d/helper/mesh.hpp>

#include <cstdint>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::scene3d::helper::ut::mesh_test);

namespace sight::viz::scene3d::helper::ut
{

using core::tools::random::safe_rand;

//------------------------------------------------------------------------------

void mesh_test::setUp()
{
}

//------------------------------------------------------------------------------

void mesh_test::tearDown()
{
}

//------------------------------------------------------------------------------

void mesh_test::copyColors()
{
    ////////////////////////////////////////////////////////////////////////////
    /// 3 components
    {
        const std::size_t num_components = 3;
        const std::size_t num_points     = 1024;
        auto* p_dest_alloc               = new Ogre::RGBA[num_points];
        Ogre::RGBA* p_dest               = p_dest_alloc;
        auto* p_src_alloc                = new std::uint8_t[num_points * num_components];
        std::uint8_t* p_src              = p_src_alloc;

        for(std::size_t i = 0 ; i < num_points ; ++i)
        {
            p_src[0] = static_cast<std::uint8_t>(safe_rand() % 255);
            p_src[1] = static_cast<std::uint8_t>(safe_rand() % 255);
            p_src[2] = static_cast<std::uint8_t>(safe_rand() % 255);
            p_src   += num_components;
        }

        viz::scene3d::helper::mesh::copyColors(p_dest, p_src_alloc, num_points, num_components);

        p_src = p_src_alloc;
        for(std::size_t i = 0 ; i < num_points ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<Ogre::RGBA>(p_src[0]), (*p_dest & 0x000000FF));
            CPPUNIT_ASSERT_EQUAL(static_cast<Ogre::RGBA>(p_src[1]), (*p_dest & 0x0000FF00) >> 8);
            CPPUNIT_ASSERT_EQUAL(static_cast<Ogre::RGBA>(p_src[2]), (*p_dest & 0x00FF0000) >> 16);
            CPPUNIT_ASSERT_EQUAL(static_cast<Ogre::RGBA>(0xFF), (*p_dest & 0xFF000000) >> 24);
            ++p_dest;
            p_src += num_components;
        }

        delete[] p_dest_alloc;
        delete[] p_src_alloc;
    }

    ////////////////////////////////////////////////////////////////////////////
    /// 4 components
    {
        const std::size_t num_components = 4;
        const std::size_t num_points     = 2047;
        auto* p_dest_alloc               = new Ogre::RGBA[num_points];
        Ogre::RGBA* p_dest               = p_dest_alloc;
        auto* p_src_alloc                = new std::uint8_t[num_points * num_components];
        std::uint8_t* p_src              = p_src_alloc;

        for(std::size_t i = 0 ; i < num_points ; ++i)
        {
            p_src[0] = static_cast<std::uint8_t>(safe_rand() % 255);
            p_src[1] = static_cast<std::uint8_t>(safe_rand() % 255);
            p_src[2] = static_cast<std::uint8_t>(safe_rand() % 255);
            p_src[3] = static_cast<std::uint8_t>(safe_rand() % 255);
            p_src   += num_components;
        }

        viz::scene3d::helper::mesh::copyColors(p_dest, p_src_alloc, num_points, num_components);

        p_src = p_src_alloc;
        for(std::size_t i = 0 ; i < num_points ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<Ogre::RGBA>(p_src[0]), (*p_dest & 0x000000FF));
            CPPUNIT_ASSERT_EQUAL(static_cast<Ogre::RGBA>(p_src[1]), (*p_dest & 0x0000FF00) >> 8);
            CPPUNIT_ASSERT_EQUAL(static_cast<Ogre::RGBA>(p_src[2]), (*p_dest & 0x00FF0000) >> 16);
            CPPUNIT_ASSERT_EQUAL(static_cast<Ogre::RGBA>(p_src[3]), (*p_dest & 0xFF000000) >> 24);
            ++p_dest;
            p_src += num_components;
        }

        delete[] p_dest_alloc;
        delete[] p_src_alloc;
    }
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::helper::ut
