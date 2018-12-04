/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
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

#include "UtilsTest.hpp"

#include <fwRenderOgre/Utils.hpp>

#include <OGRE/OgreColourValue.h>

#include <random>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwRenderOgre::ut::UtilsTest );

namespace fwRenderOgre
{

namespace ut
{

//------------------------------------------------------------------------------

void UtilsTest::setUp()
{
}

//------------------------------------------------------------------------------

void UtilsTest::tearDown()
{
}

//------------------------------------------------------------------------------

void UtilsTest::convertOgreColorToFwColor()
{
    ::fwData::Color::sptr refColor = ::fwData::Color::New();
    refColor->setRGBA(1.f, 1.f, 1.f, 1.f);

    ::fwData::Color::sptr resultColor = ::fwRenderOgre::Utils::convertOgreColorToFwColor(::Ogre::ColourValue());
    CPPUNIT_ASSERT(static_cast<int>(resultColor->red()) == static_cast<int>(refColor->red()));
    CPPUNIT_ASSERT(static_cast<int>(resultColor->green()) == static_cast<int>(refColor->green()));
    CPPUNIT_ASSERT(static_cast<int>(resultColor->blue()) == static_cast<int>(refColor->blue()));
    CPPUNIT_ASSERT(static_cast<int>(resultColor->alpha()) == static_cast<int>(refColor->alpha()));
}

//------------------------------------------------------------------------------

void UtilsTest::convertOgreMatrixToTM3D()
{
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_real_distribution<float> dist(-10., 10.);

    const double epsilon = 1e-7; // matrices are cast from double to float meaning that some precision could be lost.

    // Convert from f4s to ogre and back to f4s.
    {
        ::fwData::TransformationMatrix3D::sptr mat0 = ::fwData::TransformationMatrix3D::New();

        for(double& coeff : mat0->getCoefficients())
        {
            coeff = dist(rng);
        }

        const ::Ogre::Matrix4 ogreMat0 = ::fwRenderOgre::Utils::convertTM3DToOgreMx(mat0);

        for(std::uint8_t l = 0; l < 4; ++l)
        {
            for(std::uint8_t c = 0; c < 4; ++c)
            {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(mat0->getCoefficient(l, c), ogreMat0[l][c], epsilon);
            }
        }

        // Convert back to TM3D.
        ::fwData::TransformationMatrix3D::sptr mat0Copy = ::fwData::TransformationMatrix3D::New();

        ::fwRenderOgre::Utils::copyOgreMxToTM3D(ogreMat0, mat0Copy);

        for(std::uint8_t l = 0; l < 4; ++l)
        {
            for(std::uint8_t c = 0; c < 4; ++c)
            {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(mat0->getCoefficient(l, c), mat0Copy->getCoefficient(l, c), epsilon);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(ogreMat0[l][c], mat0Copy->getCoefficient(l, c), epsilon);
            }
        }
    }

    // Convert from ogre to f4s and back to ogre.
    {
        ::Ogre::Matrix4 ogreMat1;
        for(std::uint8_t l = 0; l < 4; ++l)
        {
            for(std::uint8_t c = 0; c < 4; ++c)
            {
                ogreMat1[l][c] = dist(rng);
            }
        }

        ::fwData::TransformationMatrix3D::sptr mat1Copy = ::fwData::TransformationMatrix3D::New();
        ::fwRenderOgre::Utils::copyOgreMxToTM3D(ogreMat1, mat1Copy);

        for(std::uint8_t l = 0; l < 4; ++l)
        {
            for(std::uint8_t c = 0; c < 4; ++c)
            {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(ogreMat1[l][c], mat1Copy->getCoefficient(l, c), epsilon);
            }
        }

        const ::Ogre::Matrix4 ogreMat1Copy = ::fwRenderOgre::Utils::convertTM3DToOgreMx(mat1Copy);

        for(std::uint8_t l = 0; l < 4; ++l)
        {
            for(std::uint8_t c = 0; c < 4; ++c)
            {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(ogreMat1[l][c], ogreMat1Copy[l][c], epsilon);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(mat1Copy->getCoefficient(l, c), ogreMat1Copy[l][c], epsilon);
            }
        }
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwRenderOgre
