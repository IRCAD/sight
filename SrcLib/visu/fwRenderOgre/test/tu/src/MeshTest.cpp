/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "MeshTest.hpp"

#include "fwRenderOgre/helper/Mesh.hpp"
#include "fwRenderOgre/Utils.hpp"

#include <cstdint>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwRenderOgre::helper::ut::MeshTest );

namespace fwRenderOgre
{
namespace helper
{
namespace ut
{

//------------------------------------------------------------------------------

MeshTest::MeshTest()
{

}

//------------------------------------------------------------------------------

MeshTest::~MeshTest()
{
}

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
        const size_t numComponents = 3;
        const size_t numPoints     = 1024;
        ::Ogre::RGBA* pDestAlloc = new ::Ogre::RGBA[numPoints];
        ::Ogre::RGBA* pDest      = pDestAlloc;
        std::uint8_t* pSrcAlloc = new std::uint8_t[numPoints * numComponents];
        std::uint8_t* pSrc      = pSrcAlloc;

        for(size_t i = 0; i < numPoints; ++i)
        {
            pSrc[0] = static_cast< ::std::uint8_t>(rand()%255);
            pSrc[1] = static_cast< ::std::uint8_t>(rand()%255);
            pSrc[2] = static_cast< ::std::uint8_t>(rand()%255);
            pSrc   += numComponents;
        }

        ::fwRenderOgre::helper::Mesh::copyColors(pDest, pSrcAlloc, numPoints, numComponents);

        pSrc = pSrcAlloc;
        for(size_t i = 0; i < numPoints; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast< ::Ogre::RGBA>(pSrc[0]), (*pDest & 0x000000FF));
            CPPUNIT_ASSERT_EQUAL(static_cast< ::Ogre::RGBA>(pSrc[1]), (*pDest & 0x0000FF00) >> 8);
            CPPUNIT_ASSERT_EQUAL(static_cast< ::Ogre::RGBA>(pSrc[2]), (*pDest & 0x00FF0000) >> 16);
            CPPUNIT_ASSERT_EQUAL(static_cast< ::Ogre::RGBA>(0xFF), (*pDest & 0xFF000000) >> 24);
            ++pDest;
            pSrc += numComponents;
        }

        delete [] pDestAlloc;
        delete [] pSrcAlloc;
    }

    ////////////////////////////////////////////////////////////////////////////
    /// 4 components
    {
        const size_t numComponents = 4;
        const size_t numPoints     = 2047;
        ::Ogre::RGBA* pDestAlloc = new ::Ogre::RGBA[numPoints];
        ::Ogre::RGBA* pDest      = pDestAlloc;
        std::uint8_t* pSrcAlloc = new std::uint8_t[numPoints * numComponents];
        std::uint8_t* pSrc      = pSrcAlloc;

        for(size_t i = 0; i < numPoints; ++i)
        {
            pSrc[0] = static_cast< ::std::uint8_t>(rand()%255);
            pSrc[1] = static_cast< ::std::uint8_t>(rand()%255);
            pSrc[2] = static_cast< ::std::uint8_t>(rand()%255);
            pSrc[3] = static_cast< ::std::uint8_t>(rand()%255);
            pSrc   += numComponents;
        }

        ::fwRenderOgre::helper::Mesh::copyColors(pDest, pSrcAlloc, numPoints, numComponents);

        pSrc = pSrcAlloc;
        for(size_t i = 0; i < numPoints; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast< ::Ogre::RGBA>(pSrc[0]), (*pDest & 0x000000FF));
            CPPUNIT_ASSERT_EQUAL(static_cast< ::Ogre::RGBA>(pSrc[1]), (*pDest & 0x0000FF00) >> 8);
            CPPUNIT_ASSERT_EQUAL(static_cast< ::Ogre::RGBA>(pSrc[2]), (*pDest & 0x00FF0000) >> 16);
            CPPUNIT_ASSERT_EQUAL(static_cast< ::Ogre::RGBA>(pSrc[3]), (*pDest & 0xFF000000) >> 24);
            ++pDest;
            pSrc += numComponents;
        }

        delete [] pDestAlloc;
        delete [] pSrcAlloc;
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace helper
} //namespace fwRenderOgre
