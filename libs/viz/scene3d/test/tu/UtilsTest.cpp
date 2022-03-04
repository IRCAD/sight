/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/data/Image.hpp>

#include <utestData/generator/Image.hpp>

#include <viz/scene3d/Utils.hpp>

#include <OGRE/OgreColourValue.h>
#include <OGRE/OgrePrerequisites.h>

#include <random>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::scene3d::ut::UtilsTest);

namespace sight::viz::scene3d
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
    data::Color::sptr refColor = data::Color::New();
    refColor->setRGBA(1.f, 1.f, 1.f, 1.f);

    data::Color::sptr resultColor = viz::scene3d::Utils::convertOgreColorToFwColor(Ogre::ColourValue());
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

    // Convert from Sight to ogre and back to Sight.
    {
        data::Matrix4::sptr mat0 = data::Matrix4::New();

        for(double& coeff : mat0->getCoefficients())
        {
            coeff = dist(rng);
        }

        const Ogre::Matrix4 ogreMat0 = viz::scene3d::Utils::convertTM3DToOgreMx(mat0);

        for(std::uint8_t l = 0 ; l < 4 ; ++l)
        {
            for(std::uint8_t c = 0 ; c < 4 ; ++c)
            {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(mat0->getCoefficient(l, c), ogreMat0[l][c], epsilon);
            }
        }

        // Convert back to TM3D.
        data::Matrix4::sptr mat0Copy = data::Matrix4::New();

        viz::scene3d::Utils::copyOgreMxToTM3D(ogreMat0, mat0Copy);

        for(std::uint8_t l = 0 ; l < 4 ; ++l)
        {
            for(std::uint8_t c = 0 ; c < 4 ; ++c)
            {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(mat0->getCoefficient(l, c), mat0Copy->getCoefficient(l, c), epsilon);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(ogreMat0[l][c], mat0Copy->getCoefficient(l, c), epsilon);
            }
        }
    }

    // Convert from ogre to Sight and back to ogre.
    {
        Ogre::Matrix4 ogreMat1;
        for(std::uint8_t l = 0 ; l < 4 ; ++l)
        {
            for(std::uint8_t c = 0 ; c < 4 ; ++c)
            {
                ogreMat1[l][c] = dist(rng);
            }
        }

        data::Matrix4::sptr mat1Copy = data::Matrix4::New();
        viz::scene3d::Utils::copyOgreMxToTM3D(ogreMat1, mat1Copy);

        for(std::uint8_t l = 0 ; l < 4 ; ++l)
        {
            for(std::uint8_t c = 0 ; c < 4 ; ++c)
            {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(ogreMat1[l][c], mat1Copy->getCoefficient(l, c), epsilon);
            }
        }

        const Ogre::Matrix4 ogreMat1Copy = viz::scene3d::Utils::convertTM3DToOgreMx(mat1Copy);

        for(std::uint8_t l = 0 ; l < 4 ; ++l)
        {
            for(std::uint8_t c = 0 ; c < 4 ; ++c)
            {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(ogreMat1[l][c], ogreMat1Copy[l][c], epsilon);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(mat1Copy->getCoefficient(l, c), ogreMat1Copy[l][c], epsilon);
            }
        }
    }
}

//------------------------------------------------------------------------------

void UtilsTest::worldToSliceTest()
{
    data::Image::sptr image = data::Image::New();

    Ogre::Vector3 world_inside_im  = {20., 10., 5.};
    Ogre::Vector3 world_outside_im = {50., -1., 5.};

    // Spacing is 0, throw exception.
    CPPUNIT_ASSERT_THROW(Utils::worldToSlices(*image, world_inside_im), core::Exception);

    utestData::generator::Image::generateImage(
        image,
        {40, 40, 40},
        {1., 1., 1.},
        {0., 0., 0.},
        core::tools::Type::s_UINT8,
        data::Image::PixelFormat::GRAY_SCALE
    );

    Ogre::Vector3i slice_idx;

    CPPUNIT_ASSERT_NO_THROW(slice_idx = Utils::worldToSlices(*image, world_inside_im));

    CPPUNIT_ASSERT_EQUAL(20, slice_idx[0]);
    CPPUNIT_ASSERT_EQUAL(10, slice_idx[1]);
    CPPUNIT_ASSERT_EQUAL(5, slice_idx[2]);

    CPPUNIT_ASSERT_THROW(Utils::worldToSlices(*image, world_outside_im), core::Exception);
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::viz::scene3d
