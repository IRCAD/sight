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

#include "MeshTest.hpp"

#include <core/tools/random/generator.hpp>

#include <viz/scene3d/helper/Mesh.hpp>

#include <cstdint>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::scene3d::helper::ut::MeshTest);

namespace sight::viz::scene3d::helper::ut
{

using core::tools::random::safe_rand;

//------------------------------------------------------------------------------

void MeshTest::setUp()
{
}

//------------------------------------------------------------------------------

void MeshTest::tearDown()
{
}

//------------------------------------------------------------------------------

void MeshTest::copyColors()
{
    ////////////////////////////////////////////////////////////////////////////
    /// 3 components
    {
        const std::size_t numComponents = 3;
        const std::size_t numPoints     = 1024;
        auto* pDestAlloc                = new Ogre::RGBA[numPoints];
        Ogre::RGBA* pDest               = pDestAlloc;
        auto* pSrcAlloc                 = new std::uint8_t[numPoints * numComponents];
        std::uint8_t* pSrc              = pSrcAlloc;

        for(std::size_t i = 0 ; i < numPoints ; ++i)
        {
            pSrc[0] = static_cast<std::uint8_t>(safe_rand() % 255);
            pSrc[1] = static_cast<std::uint8_t>(safe_rand() % 255);
            pSrc[2] = static_cast<std::uint8_t>(safe_rand() % 255);
            pSrc   += numComponents;
        }

        viz::scene3d::helper::Mesh::copyColors(pDest, pSrcAlloc, numPoints, numComponents);

        pSrc = pSrcAlloc;
        for(std::size_t i = 0 ; i < numPoints ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<Ogre::RGBA>(pSrc[0]), (*pDest & 0x000000FF));
            CPPUNIT_ASSERT_EQUAL(static_cast<Ogre::RGBA>(pSrc[1]), (*pDest & 0x0000FF00) >> 8);
            CPPUNIT_ASSERT_EQUAL(static_cast<Ogre::RGBA>(pSrc[2]), (*pDest & 0x00FF0000) >> 16);
            CPPUNIT_ASSERT_EQUAL(static_cast<Ogre::RGBA>(0xFF), (*pDest & 0xFF000000) >> 24);
            ++pDest;
            pSrc += numComponents;
        }

        delete[] pDestAlloc;
        delete[] pSrcAlloc;
    }

    ////////////////////////////////////////////////////////////////////////////
    /// 4 components
    {
        const std::size_t numComponents = 4;
        const std::size_t numPoints     = 2047;
        auto* pDestAlloc                = new Ogre::RGBA[numPoints];
        Ogre::RGBA* pDest               = pDestAlloc;
        auto* pSrcAlloc                 = new std::uint8_t[numPoints * numComponents];
        std::uint8_t* pSrc              = pSrcAlloc;

        for(std::size_t i = 0 ; i < numPoints ; ++i)
        {
            pSrc[0] = static_cast<std::uint8_t>(safe_rand() % 255);
            pSrc[1] = static_cast<std::uint8_t>(safe_rand() % 255);
            pSrc[2] = static_cast<std::uint8_t>(safe_rand() % 255);
            pSrc[3] = static_cast<std::uint8_t>(safe_rand() % 255);
            pSrc   += numComponents;
        }

        viz::scene3d::helper::Mesh::copyColors(pDest, pSrcAlloc, numPoints, numComponents);

        pSrc = pSrcAlloc;
        for(std::size_t i = 0 ; i < numPoints ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<Ogre::RGBA>(pSrc[0]), (*pDest & 0x000000FF));
            CPPUNIT_ASSERT_EQUAL(static_cast<Ogre::RGBA>(pSrc[1]), (*pDest & 0x0000FF00) >> 8);
            CPPUNIT_ASSERT_EQUAL(static_cast<Ogre::RGBA>(pSrc[2]), (*pDest & 0x00FF0000) >> 16);
            CPPUNIT_ASSERT_EQUAL(static_cast<Ogre::RGBA>(pSrc[3]), (*pDest & 0xFF000000) >> 24);
            ++pDest;
            pSrc += numComponents;
        }

        delete[] pDestAlloc;
        delete[] pSrcAlloc;
    }
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::helper::ut
