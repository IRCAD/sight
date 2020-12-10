/************************************************************************
 * Copyright (C) 2018-2020 IRCAD France - All rights reserved.
 * Copyright (C) 2018-2020 IHU Strasbourg - All rights reserved.
 *
 * This file is part of Sight-nonfree.
 *
 * Sight-nonfree can not be copied, modified and/or distributed without
 * the express permission of IRCAD France and IHU Strasbourg.
 *
 ***********************************************************************/

#include "MinMaxPropagationTest.hpp"

#include <imageFilterOp/MinMaxPropagation.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <fwTest/generator/Image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::imageFilterOp::ut::MinMaxPropagationTest );

namespace imageFilterOp
{
namespace ut
{

//------------------------------------------------------------------------------

static size_t computeOffset( const size_t x, const size_t y, const size_t z, ::fwData::Image::sptr image )
{
    const fwData::Image::Size size = image->getSize2();
    return z * size[0] * size[1] + y * size[0] + x;
}

//------------------------------------------------------------------------------

static void drawCube( ::fwData::Image::sptr image, const std::uint8_t value)
{
    const auto dumpLock = image->lock();

    SPTR(::fwData::Image::BufferType) bufferValue =
        ::fwDataTools::fieldHelper::MedicalImageHelpers::getPixelBufferInImageSpace(image, value );

    for( size_t x = 10; x < 20; ++x  )
    {
        for( size_t y = 10; y < 20; ++y  )
        {
            for( size_t z = 10; z < 20; ++z  )
            {
                image->setPixelBuffer( computeOffset(x, y, z, image), bufferValue.get() );
            }
        }
    }
}

//------------------------------------------------------------------------------

void MinMaxPropagationTest::setUp()
{
}

//------------------------------------------------------------------------------

void MinMaxPropagationTest::tearDown()
{
}

//------------------------------------------------------------------------------

void MinMaxPropagationTest::minPropagTest()
{
    // Create two 32*32*32*8 images
    const ::fwData::Image::Size SIZE       = {{ 32, 32, 32 }};
    const ::fwData::Image::Spacing SPACING = {{ 1., 1., 1. }};
    const ::fwData::Image::Origin ORIGIN   = {{ 0., 0., 0. }};
    const ::fwTools::Type TYPE             = ::fwTools::Type::s_UINT8;

    ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
    ::fwData::Image::sptr imageOut = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(imageIn, SIZE, SPACING, ORIGIN, TYPE, ::fwData::Image::GRAY_SCALE);
    ::fwTest::generator::Image::generateImage(imageOut, SIZE, SPACING, ORIGIN, TYPE, ::fwData::Image::GRAY_SCALE);

    // Draw a cube at 10,10,10 with a 255 value
    drawCube( imageIn, 255 );

    MinMaxPropagation propagator(imageIn, imageOut, nullptr);

    MinMaxPropagation::SeedsType seed = {{ {15, 15, 15} }};

    // Propagate at 15,15,15 with a 255 value (same as cube)
    std::uint8_t value = 255;

    SPTR(::fwData::Image::BufferType) bufferValue =
        ::fwDataTools::fieldHelper::MedicalImageHelpers::getPixelBufferInImageSpace(imageIn, value );

    propagator.propagate( seed, bufferValue.get(), 500, true, MinMaxPropagation::MIN );

    const auto dumpLockIn  = imageIn->lock();
    const auto dumpLockOut = imageOut->lock();

    // Check that the image is not changed because the propagated value is the same
    for( size_t index = 0; index < imageIn->getSizeInBytes(); ++index )
    {
        const std::uint8_t valueIn  = imageIn->at<std::uint8_t>(index);
        const std::uint8_t valueOut = imageOut->at<std::uint8_t>(index);

        CPPUNIT_ASSERT_EQUAL(valueIn, valueOut);
    }

    // Check that the cube is changed with a "3" propagated value
    drawCube( imageIn, 1 );

    value = 3;

    bufferValue =
        ::fwDataTools::fieldHelper::MedicalImageHelpers::getPixelBufferInImageSpace(imageIn, value );

    propagator.propagate( seed, bufferValue.get(), 500, true, MinMaxPropagation::MIN );

    for( size_t index = 0; index < imageIn->getSizeInBytes(); ++index )
    {
        const std::uint8_t valueIn  = imageIn->at<std::uint8_t>(index);
        const std::uint8_t valueOut = imageOut->at<std::uint8_t>(index);

        if( valueIn == 1 )
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(3), valueOut);
        }
        else
        {
            CPPUNIT_ASSERT_EQUAL(valueIn, valueOut);
        }
    }

    // Check that the entire image is completely filled with propagated value
    seed = {{ {0, 0, 0} }};

    value = 4;

    bufferValue =
        ::fwDataTools::fieldHelper::MedicalImageHelpers::getPixelBufferInImageSpace(imageIn, value );

    propagator.propagate( seed, bufferValue.get(), 500, true, MinMaxPropagation::MIN );

    for( size_t index = 0; index < imageIn->getSizeInBytes(); ++index )
    {
        const std::uint8_t valueOut = imageOut->at<std::uint8_t>(index);

        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4), valueOut);
    }
}

//------------------------------------------------------------------------------

void MinMaxPropagationTest::maxPropagTest()
{
    // Create two 32*32*32*8 images
    const ::fwData::Image::Size SIZE       = {{ 32, 32, 32 }};
    const ::fwData::Image::Spacing SPACING = {{ 1., 1., 1. }};
    const ::fwData::Image::Origin ORIGIN   = {{ 0., 0., 0. }};
    const ::fwTools::Type TYPE             = ::fwTools::Type::s_UINT8;

    ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
    ::fwData::Image::sptr imageOut = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(imageIn, SIZE, SPACING, ORIGIN, TYPE, ::fwData::Image::GRAY_SCALE);
    ::fwTest::generator::Image::generateImage(imageOut, SIZE, SPACING, ORIGIN, TYPE, ::fwData::Image::GRAY_SCALE);

    // Draw a cube at 10,10,10 with a 2 value
    drawCube( imageIn, 2 );

    MinMaxPropagation propagator(imageIn, imageOut, nullptr);

    MinMaxPropagation::SeedsType seed = {{ {15, 15, 15} }};

    // Propagate at 15,15,15 with a 3 value
    std::uint8_t value = 3;

    SPTR(::fwData::Image::BufferType) bufferValue =
        ::fwDataTools::fieldHelper::MedicalImageHelpers::getPixelBufferInImageSpace(imageIn, value );

    propagator.propagate( seed, bufferValue.get(), 500, true, MinMaxPropagation::MAX );

    const auto dumpLockIn  = imageIn->lock();
    const auto dumpLockOut = imageOut->lock();

    // Check that the entire image is completely filled with propagated value
    for( size_t index = 0; index < imageIn->getSizeInBytes(); ++index )
    {
        const std::uint8_t valueOut = imageOut->at<std::uint8_t>(index);
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(3), valueOut);
    }

    // Propagate at 0,0,0 with a 2 value (outside cube)
    seed  = {{ {0, 0, 0} }};
    value = 2;

    bufferValue = ::fwDataTools::fieldHelper::MedicalImageHelpers::getPixelBufferInImageSpace(imageIn, value );

    propagator.propagate( seed, bufferValue.get(), 500, true, MinMaxPropagation::MAX );

    // Check that the entire image is completely filled with propagated value
    for( size_t index = 0; index < imageIn->getSizeInBytes(); ++index )
    {
        const std::uint8_t valueIn  = imageIn->at<std::uint8_t>(index);
        const std::uint8_t valueOut = imageOut->at<std::uint8_t>(index);

        if( valueIn == 0 )
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(2), valueOut);
        }
        else
        {
            // Old value previously propagated
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(3), valueOut);
        }
    }
}

//------------------------------------------------------------------------------

void MinMaxPropagationTest::radiusTest()
{
    // Create two 32*32*32*8 images
    const ::fwData::Image::Size SIZE       = {{ 33, 33, 33 }};
    const ::fwData::Image::Spacing SPACING = {{ 1., 1., 1. }};
    const ::fwData::Image::Origin ORIGIN   = {{ 0., 0., 0. }};
    const ::fwTools::Type TYPE             = ::fwTools::Type::s_UINT8;

    ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
    ::fwData::Image::sptr imageOut = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(imageIn, SIZE, SPACING, ORIGIN, TYPE, ::fwData::Image::GRAY_SCALE);
    ::fwTest::generator::Image::generateImage(imageOut, SIZE, SPACING, ORIGIN, TYPE, ::fwData::Image::GRAY_SCALE);

    MinMaxPropagation propagator(imageIn, imageOut, nullptr);

    MinMaxPropagation::SeedsType seed = {{ {16, 16, 16} }};

    // Propagate at 16,16,16 with a 3 value
    std::uint8_t value = 3;

    SPTR(::fwData::Image::BufferType) bufferValue =
        ::fwDataTools::fieldHelper::MedicalImageHelpers::getPixelBufferInImageSpace(imageIn, value );

    propagator.propagate( seed, bufferValue.get(), 3.5, true, MinMaxPropagation::MIN );

    const auto dumpLockOut = imageOut->lock();

    // Check the voxel at 16,16,12
    std::uint8_t valueOut = imageOut->at<std::uint8_t>(16, 16, 12);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), valueOut);

    // Check the voxel at 16,16,13
    valueOut = imageOut->at<std::uint8_t>(16, 16, 13);

    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(3), valueOut);

    // Check the voxel at 16,16,16
    valueOut = imageOut->at<std::uint8_t>(16, 16, 16);

    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(3), valueOut);

    // Check the voxel at 16,16,20
    valueOut = imageOut->at<std::uint8_t>(16, 16, 20);

    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), valueOut);
}

//------------------------------------------------------------------------------

} //namespace ut.
} //namespace imageFilterOp.
