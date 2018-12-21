/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "FlipTest.hpp"

#include <imageFilterOp/Flipper.hpp>

#include <fwData/Image.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <fwTest/generator/Image.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(::imageFilterOp::ut::FlipTest);

namespace imageFilterOp
{
namespace ut
{

//------------------------------------------------------------------------------

void FlipTest::flipAlongXAxisTest()
{
    const ::fwData::Image::SizeType SIZE       = {{ 3, 3, 3 }};
    const ::fwData::Image::SpacingType SPACING = {{ 0.5, 0.5, 0.5 }};
    const ::fwData::Image::OriginType ORIGIN   = {{ 0., 0., 0. }};
    const ::fwTools::Type TYPE                 = ::fwTools::Type::s_UINT8;
    std::vector<bool> flipAxes{true, false, false};

    ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
    ::fwData::Image::sptr imageOut = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(imageIn, SIZE, SPACING, ORIGIN, TYPE);
    ::fwTest::generator::Image::randomizeArray(imageIn->getDataArray());

    ::fwDataTools::helper::Image imageInHelper(imageIn);
    ::imageFilterOp::Flipper::flip(imageIn, imageOut, flipAxes);
    ::fwDataTools::helper::Image imageOutHelper(imageOut);

    for(size_t i = 0; i < SIZE[0]; ++i)
    {
        for(size_t j = 0; j < SIZE[1]; ++j )
        {
            for(size_t k = 0; k < SIZE[2]; ++k)
            {
                const uint8_t valueIn  = *(reinterpret_cast<uint8_t*>(imageInHelper.getPixelBuffer(i, j, k)));
                const uint8_t valueOut =
                    *(reinterpret_cast<uint8_t*>(imageOutHelper.getPixelBuffer(SIZE[0]-i-1, j, k)));

                CPPUNIT_ASSERT_EQUAL(valueIn, valueOut);
            }
        }
    }
}

//------------------------------------------------------------------------------

void FlipTest::flipAlongYAxisTest()
{
    const ::fwData::Image::SizeType SIZE       = {{ 3, 3, 3 }};
    const ::fwData::Image::SpacingType SPACING = {{ 0.5, 0.5, 0.5 }};
    const ::fwData::Image::OriginType ORIGIN   = {{ 0., 0., 0. }};
    const ::fwTools::Type TYPE                 = ::fwTools::Type::s_UINT8;
    std::vector<bool> flipAxes{false, true, false};

    ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
    ::fwData::Image::sptr imageOut = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(imageIn, SIZE, SPACING, ORIGIN, TYPE);
    ::fwTest::generator::Image::randomizeArray(imageIn->getDataArray());

    ::fwDataTools::helper::Image imageInHelper(imageIn);
    ::imageFilterOp::Flipper::flip(imageIn, imageOut, flipAxes);
    ::fwDataTools::helper::Image imageOutHelper(imageOut);

    for(size_t i = 0; i < SIZE[0]; ++i)
    {
        for(size_t j = 0; j < SIZE[1]; ++j )
        {
            for(size_t k = 0; k < SIZE[2]; ++k)
            {
                const uint8_t valueIn  = *(reinterpret_cast<uint8_t*>(imageInHelper.getPixelBuffer(i, j, k)));
                const uint8_t valueOut =
                    *(reinterpret_cast<uint8_t*>(imageOutHelper.getPixelBuffer(i, SIZE[1]-j-1, k)));

                CPPUNIT_ASSERT_EQUAL(valueIn, valueOut);
            }
        }
    }
}

//------------------------------------------------------------------------------

void FlipTest::flipAlongZAxisTest()
{
    const ::fwData::Image::SizeType SIZE       = {{ 3, 3, 3 }};
    const ::fwData::Image::SpacingType SPACING = {{ 0.5, 0.5, 0.5 }};
    const ::fwData::Image::OriginType ORIGIN   = {{ 0., 0., 0. }};
    const ::fwTools::Type TYPE                 = ::fwTools::Type::s_UINT8;
    std::vector<bool> flipAxes{false, false, true};

    ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
    ::fwData::Image::sptr imageOut = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(imageIn, SIZE, SPACING, ORIGIN, TYPE);
    ::fwTest::generator::Image::randomizeArray(imageIn->getDataArray());

    ::fwDataTools::helper::Image imageInHelper(imageIn);
    ::imageFilterOp::Flipper::flip(imageIn, imageOut, flipAxes);
    ::fwDataTools::helper::Image imageOutHelper(imageOut);

    for(size_t i = 0; i < SIZE[0]; ++i)
    {
        for(size_t j = 0; j < SIZE[1]; ++j )
        {
            for(size_t k = 0; k < SIZE[2]; ++k)
            {
                const uint8_t valueIn  = *(reinterpret_cast<uint8_t*>(imageInHelper.getPixelBuffer(i, j, k)));
                const uint8_t valueOut =
                    *(reinterpret_cast<uint8_t*>(imageOutHelper.getPixelBuffer(i, j, SIZE[2]-k-1)));

                CPPUNIT_ASSERT_EQUAL(valueIn, valueOut);
            }
        }
    }
}

//------------------------------------------------------------------------------

void FlipTest::flipEmptyImageTest()
{
    const ::fwData::Image::SizeType SIZE       = {{ 0, 0, 0 }};
    const ::fwData::Image::SpacingType SPACING = {{ 0.5, 0.5, 0.5 }};
    const ::fwData::Image::OriginType ORIGIN   = {{ 0., 0., 0. }};
    const ::fwTools::Type TYPE                 = ::fwTools::Type::s_UINT8;
    std::vector<bool> flipAxes{false, true, false};

    ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
    ::fwData::Image::sptr imageOut = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(imageIn, SIZE, SPACING, ORIGIN, TYPE);
    ::fwTest::generator::Image::randomizeArray(imageIn->getDataArray());

    ::fwDataTools::helper::Image imageInHelper(imageIn);
    ::imageFilterOp::Flipper::flip(imageIn, imageOut, flipAxes);
    ::fwDataTools::helper::Image imageOutHelper(imageOut);

    const ::fwData::Image::SizeType imageSize    = imageIn->getSize();
    const ::fwData::Image::SizeType imageOutSize = imageOut->getSize();

    CPPUNIT_ASSERT_EQUAL(imageSize[0], imageOutSize[0]);
    CPPUNIT_ASSERT_EQUAL(imageSize[1], imageOutSize[1]);
    CPPUNIT_ASSERT_EQUAL(imageSize[2], imageOutSize[2]);
}

//------------------------------------------------------------------------------
} // ut
} // imageFilterOp
