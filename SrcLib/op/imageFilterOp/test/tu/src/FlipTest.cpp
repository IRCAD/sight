/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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
    const ::fwData::Image::SizeType size       = {{ 3, 3, 3 }};
    const ::fwData::Image::SpacingType spacing = {{ 0.5, 0.5, 0.5 }};
    const ::fwData::Image::OriginType origin   = {{ 0., 0., 0. }};
    const ::fwTools::Type type                 = ::fwTools::Type::s_UINT8;
    std::array<bool, 3> flipAxes{true, false, false};

    ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
    ::fwData::Image::sptr imageOut = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(imageIn, size, spacing, origin, type);
    ::fwTest::generator::Image::randomizeArray(imageIn->getDataArray());

    ::fwDataTools::helper::Image imageInHelper(imageIn);
    ::imageFilterOp::Flipper::flip(imageIn, imageOut, flipAxes);
    ::fwDataTools::helper::Image imageOutHelper(imageOut);

    for(size_t i = 0; i < size[0]; ++i)
    {
        for(size_t j = 0; j < size[1]; ++j )
        {
            for(size_t k = 0; k < size[2]; ++k)
            {
                const uint8_t valueIn  = *(reinterpret_cast<uint8_t*>(imageInHelper.getPixelBuffer(i, j, k)));
                const uint8_t valueOut =
                    *(reinterpret_cast<uint8_t*>(imageOutHelper.getPixelBuffer(size[0]-i-1, j, k)));

                CPPUNIT_ASSERT_EQUAL(valueIn, valueOut);
            }
        }
    }
}

//------------------------------------------------------------------------------

void FlipTest::flipAlongYAxisTest()
{
    const ::fwData::Image::SizeType size       = {{ 3, 3, 3 }};
    const ::fwData::Image::SpacingType spacing = {{ 0.5, 0.5, 0.5 }};
    const ::fwData::Image::OriginType origin   = {{ 0., 0., 0. }};
    const ::fwTools::Type type                 = ::fwTools::Type::s_UINT8;
    std::array<bool, 3> flipAxes{false, true, false};

    ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
    ::fwData::Image::sptr imageOut = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(imageIn, size, spacing, origin, type);
    ::fwTest::generator::Image::randomizeArray(imageIn->getDataArray());

    ::fwDataTools::helper::Image imageInHelper(imageIn);
    ::imageFilterOp::Flipper::flip(imageIn, imageOut, flipAxes);
    ::fwDataTools::helper::Image imageOutHelper(imageOut);

    for(size_t i = 0; i < size[0]; ++i)
    {
        for(size_t j = 0; j < size[1]; ++j )
        {
            for(size_t k = 0; k < size[2]; ++k)
            {
                const uint8_t valueIn  = *(reinterpret_cast<uint8_t*>(imageInHelper.getPixelBuffer(i, j, k)));
                const uint8_t valueOut =
                    *(reinterpret_cast<uint8_t*>(imageOutHelper.getPixelBuffer(i, size[1]-j-1, k)));

                CPPUNIT_ASSERT_EQUAL(valueIn, valueOut);
            }
        }
    }
}

//------------------------------------------------------------------------------

void FlipTest::flipAlongZAxisTest()
{
    const ::fwData::Image::SizeType size       = {{ 3, 3, 3 }};
    const ::fwData::Image::SpacingType spacing = {{ 0.5, 0.5, 0.5 }};
    const ::fwData::Image::OriginType origin   = {{ 0., 0., 0. }};
    const ::fwTools::Type type                 = ::fwTools::Type::s_UINT8;
    std::array<bool, 3> flipAxes{false, false, true};

    ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
    ::fwData::Image::sptr imageOut = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(imageIn, size, spacing, origin, type);
    ::fwTest::generator::Image::randomizeArray(imageIn->getDataArray());

    ::fwDataTools::helper::Image imageInHelper(imageIn);
    ::imageFilterOp::Flipper::flip(imageIn, imageOut, flipAxes);
    ::fwDataTools::helper::Image imageOutHelper(imageOut);

    for(size_t i = 0; i < size[0]; ++i)
    {
        for(size_t j = 0; j < size[1]; ++j )
        {
            for(size_t k = 0; k < size[2]; ++k)
            {
                const uint8_t valueIn  = *(reinterpret_cast<uint8_t*>(imageInHelper.getPixelBuffer(i, j, k)));
                const uint8_t valueOut =
                    *(reinterpret_cast<uint8_t*>(imageOutHelper.getPixelBuffer(i, j, size[2]-k-1)));

                CPPUNIT_ASSERT_EQUAL(valueIn, valueOut);
            }
        }
    }
}

//------------------------------------------------------------------------------

void FlipTest::flipAlongMultipleAxesTest()
{
    {
        const ::fwData::Image::SizeType size       = {{ 3, 3, 3 }};
        const ::fwData::Image::SpacingType spacing = {{ 0.5, 0.5, 0.5 }};
        const ::fwData::Image::OriginType origin   = {{ 0., 0., 0. }};
        const ::fwTools::Type type                 = ::fwTools::Type::s_UINT8;
        std::array<bool, 3> flipAxes{true, true, false };

        ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
        ::fwData::Image::sptr imageOut = ::fwData::Image::New();

        ::fwTest::generator::Image::generateImage(imageIn, size, spacing, origin, type);
        ::fwTest::generator::Image::randomizeArray(imageIn->getDataArray());

        ::fwDataTools::helper::Image imageInHelper(imageIn);
        ::imageFilterOp::Flipper::flip(imageIn, imageOut, flipAxes);
        ::fwDataTools::helper::Image imageOutHelper(imageOut);

        for(size_t i = 0; i < size[0]; ++i)
        {
            for(size_t j = 0; j < size[1]; ++j )
            {
                for(size_t k = 0; k < size[2]; ++k)
                {
                    const uint8_t valueIn  = *(reinterpret_cast<uint8_t*>(imageInHelper.getPixelBuffer(i, j, k)));
                    const uint8_t valueOut =
                        *(reinterpret_cast<uint8_t*>(imageOutHelper.getPixelBuffer(size[0]-i-1, size[1]-j-1, k)));

                    // Static cast to get proper printing of the value on stdout
                    CPPUNIT_ASSERT_EQUAL(static_cast<int>(valueIn), static_cast<int>(valueOut));
                }
            }
        }
    }

    {
        const ::fwData::Image::SizeType size       = {{ 3, 3, 3 }};
        const ::fwData::Image::SpacingType spacing = {{ 0.5, 0.5, 0.5 }};
        const ::fwData::Image::OriginType origin   = {{ 0., 0., 0. }};
        const ::fwTools::Type type                 = ::fwTools::Type::s_UINT8;
        std::array<bool, 3> flipAxes{true, true, true};

        ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
        ::fwData::Image::sptr imageOut = ::fwData::Image::New();

        ::fwTest::generator::Image::generateImage(imageIn, size, spacing, origin, type);
        ::fwTest::generator::Image::randomizeArray(imageIn->getDataArray());

        ::fwDataTools::helper::Image imageInHelper(imageIn);
        ::imageFilterOp::Flipper::flip(imageIn, imageOut, flipAxes);
        ::fwDataTools::helper::Image imageOutHelper(imageOut);

        for(size_t i = 0; i < size[0]; ++i)
        {
            for(size_t j = 0; j < size[1]; ++j )
            {
                for(size_t k = 0; k < size[2]; ++k)
                {
                    const uint8_t valueIn  = *(reinterpret_cast<uint8_t*>(imageInHelper.getPixelBuffer(i, j, k)));
                    const uint8_t valueOut =
                        *(reinterpret_cast<uint8_t*>(imageOutHelper.getPixelBuffer(size[0]-i-1,
                                                                                   size[1]-j-1,
                                                                                   size[2]-k-1)));

                    // Static cast to get proper printing of the value on stdout
                    CPPUNIT_ASSERT_EQUAL(static_cast<int>(valueIn), static_cast<int>(valueOut));
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void FlipTest::flipEmptyImageTest()
{
    const ::fwData::Image::SizeType size       = {{ 0, 0, 0 }};
    const ::fwData::Image::SpacingType spacing = {{ 0.5, 0.5, 0.5 }};
    const ::fwData::Image::OriginType origin   = {{ 0., 0., 0. }};
    const ::fwTools::Type type                 = ::fwTools::Type::s_UINT8;
    std::array<bool, 3> flipAxes{false, true, false};

    ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
    ::fwData::Image::sptr imageOut = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(imageIn, size, spacing, origin, type);
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
