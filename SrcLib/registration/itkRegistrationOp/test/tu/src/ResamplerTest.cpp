/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ResamplerTest.hpp"

#include <itkRegistrationOp/Resampler.hpp>

#include <fwData/Image.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Image.hpp>
#include <fwDataTools/helper/MedicalImage.hpp>

#include <fwTest/generator/Image.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(::itkRegistrationOp::ut::ResamplerTest);

namespace itkRegistrationOp
{
namespace ut
{

//------------------------------------------------------------------------------

void ResamplerTest::setUp()
{

}

//------------------------------------------------------------------------------

void ResamplerTest::tearDown()
{

}

//------------------------------------------------------------------------------

void ResamplerTest::identityTest()
{
    const ::fwData::Image::SizeType SIZE = {{ 32, 32, 32 }};

    // TODO: make it work with an anisotropic spacing.
    const ::fwData::Image::SpacingType SPACING = {{ 0.5, 0.5, 0.5 }};
    const ::fwData::Image::OriginType ORIGIN   = {{ 0., 0., 0. }};
    const ::fwTools::Type TYPE                 = ::fwTools::Type::s_INT16;

    ::fwData::Image::sptr imageIn = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(imageIn, SIZE, SPACING, ORIGIN, TYPE);
    ::fwTest::generator::Image::randomizeArray(imageIn->getDataArray());

    ::fwData::Image::sptr imageOut = ::fwData::Image::New();

    // Identity.
    ::fwData::TransformationMatrix3D::sptr idMat = ::fwData::TransformationMatrix3D::New();

    ::itkRegistrationOp::Resampler::resample(
        ::fwData::Image::csptr(imageIn), imageOut, ::fwData::TransformationMatrix3D::csptr(idMat), imageIn);

    CPPUNIT_ASSERT(imageOut->getSize() == SIZE);
    CPPUNIT_ASSERT(imageOut->getSpacing() == SPACING);
    CPPUNIT_ASSERT(imageOut->getType() == TYPE);

    fwDataTools::helper::Image imageInHelper(imageIn);
    fwDataTools::helper::Image imageOutHelper(imageOut);

    for(size_t i = 0; i < SIZE[0]; ++i)
    {
        for(size_t j = 0; j < SIZE[1]; ++j )
        {
            for(size_t k = 0; k < SIZE[2]; ++k)
            {
                const uint8_t valueIn  = *(reinterpret_cast<uint8_t*>(imageInHelper.getPixelBuffer(i, j, k)));
                const uint8_t valueOut = *(reinterpret_cast<uint8_t*>(imageOutHelper.getPixelBuffer(i, j, k)));

                // The image shouldn't change.
                std::string msg = std::to_string(valueIn) + " " + std::to_string(valueOut);
                CPPUNIT_ASSERT_EQUAL_MESSAGE(msg.c_str(), valueIn, valueOut);
            }
        }
    }
}

//------------------------------------------------------------------------------

static size_t computeOffset( const size_t x, const size_t y, const size_t z, ::fwData::Image::sptr image )
{
    const fwData::Image::SizeType size = image->getSize();
    return z * size[0] * size[1] + y * size[0] + x;
}

//------------------------------------------------------------------------------

void ResamplerTest::translateTest()
{
    // Generate a simple image with a white cube at its center.
    const ::fwData::Image::SizeType SIZE       = {{ 16, 16, 16 }};
    const ::fwData::Image::SpacingType SPACING = {{ 1., 1., 1. }};
    const ::fwData::Image::OriginType ORIGIN   = {{ 0., 0., 0. }};
    const ::fwTools::Type TYPE                 = ::fwTools::Type::s_UINT8;

    ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
    ::fwData::Image::sptr imageOut = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(imageIn, SIZE, SPACING, ORIGIN, TYPE);

    std::uint8_t value = 255;

    SPTR(::fwData::Image::BufferType) bufferValue =
        ::fwDataTools::fieldHelper::MedicalImageHelpers::getPixelBufferInImageSpace(imageIn, value);

    fwDataTools::helper::Image imageHelper(imageIn);

    // Draw a tiny 2x2 cube at the center
    imageHelper.setPixelBuffer( computeOffset(7, 7, 7, imageIn), bufferValue.get() );
    imageHelper.setPixelBuffer( computeOffset(7, 7, 8, imageIn), bufferValue.get() );
    imageHelper.setPixelBuffer( computeOffset(7, 8, 7, imageIn), bufferValue.get() );
    imageHelper.setPixelBuffer( computeOffset(7, 8, 8, imageIn), bufferValue.get() );
    imageHelper.setPixelBuffer( computeOffset(8, 7, 7, imageIn), bufferValue.get() );
    imageHelper.setPixelBuffer( computeOffset(8, 7, 8, imageIn), bufferValue.get() );
    imageHelper.setPixelBuffer( computeOffset(8, 8, 7, imageIn), bufferValue.get() );
    imageHelper.setPixelBuffer( computeOffset(8, 8, 8, imageIn), bufferValue.get() );

    // 5 mm translation along the x axis.
    ::fwData::TransformationMatrix3D::sptr transMat = ::fwData::TransformationMatrix3D::New();
    transMat->setCoefficient(0, 3, 5);

    ::itkRegistrationOp::Resampler::resample(
        ::fwData::Image::csptr(imageIn), imageOut, ::fwData::TransformationMatrix3D::csptr(transMat));

    fwDataTools::helper::Image imageOutHelper(imageOut);

    for(size_t i = 0; i < SIZE[0]; ++i)
    {
        for(size_t j = 0; j < SIZE[1]; ++j )
        {
            for(size_t k = 0; k < SIZE[2]; ++k)
            {
                const uint8_t valueOut = *(reinterpret_cast<uint8_t*>(imageOutHelper.getPixelBuffer(i, j, k)));

                if((i >= 2 && i <= 3) && (j >= 7 && j <= 8) && (k >= 7 && k <= 8))
                {
                    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(255), valueOut);
                }
                else
                {
                    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), valueOut);
                }
            }
        }
    }

    // Check if size and spacing are the same as the input.
    CPPUNIT_ASSERT(imageOut->getSize() == SIZE);
    CPPUNIT_ASSERT(imageOut->getSpacing() == SPACING);
}

//------------------------------------------------------------------------------

void ResamplerTest::rotateTest()
{
    const ::fwData::Image::SizeType SIZE       = {{ 64, 64, 64 }};
    const ::fwData::Image::SpacingType SPACING = {{ 1., 1., 1. }};
    const ::fwData::Image::OriginType ORIGIN   = {{ 0., 0., 0. }};
    const ::fwTools::Type TYPE                 = ::fwTools::Type::s_FLOAT;

    ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
    ::fwData::Image::sptr imageOut = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(imageIn, SIZE, SPACING, ORIGIN, TYPE);

    const float value = 1.f;

    SPTR(::fwData::Image::BufferType) bufferValue =
        ::fwDataTools::fieldHelper::MedicalImageHelpers::getPixelBufferInImageSpace(imageIn, value);

    fwDataTools::helper::Image imageHelper(imageIn);

    // draw the back Z face.
    for(size_t i = 0; i < 64; ++i)
    {
        for(size_t j = 0; j < 64; ++j)
        {
            imageHelper.setPixelBuffer( computeOffset(i, j, 0, imageIn), bufferValue.get() );
        }
    }

    // FIXME: compute to appropriate matrix to rotate a face from negative Z to negative X.

    ::fwData::TransformationMatrix3D::sptr rotMat = ::fwData::TransformationMatrix3D::New();
    // 90° rotation along the Y axis.
    rotMat->setCoefficient(0, 0, 0);
    rotMat->setCoefficient(0, 2, 1);
    rotMat->setCoefficient(2, 0, -1);
    rotMat->setCoefficient(2, 2, 0);

    // 32 mm translation along the X axis.
    rotMat->setCoefficient(0, 3, SIZE[0] / 2.);

    ::itkRegistrationOp::Resampler::resample(
        ::fwData::Image::csptr(imageIn), imageOut, ::fwData::TransformationMatrix3D::csptr(rotMat));

    fwDataTools::helper::Image imageOutHelper(imageOut);

    for(size_t i = 0; i < SIZE[0]; ++i)
    {
        for(size_t j = 0; j < SIZE[1]; ++j )
        {
            for(size_t k = 0; k < SIZE[2]; ++k)
            {
                const float valueOut = *(reinterpret_cast<uint8_t*>(imageOutHelper.getPixelBuffer(i, j, k)));

                std::string msg = std::to_string(i) + " " + std::to_string(j) + " " + std::to_string(k);

                if(i == 0)
                {
                    // The negative Z face must be 'white'.
                    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg.c_str(), 1.f, valueOut);
                }
                else
                {
                    // Everything else should be 'black'.
                    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg.c_str(), 0.f, valueOut);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} // ut
} // itkRegistrationOp
