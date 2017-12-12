/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "FastRegistrationTest.hpp"

#include "helper.hpp"

#include <itkRegistrationOp/FastRegistration.hpp>
#include <itkRegistrationOp/Metric.hpp>
#include <itkRegistrationOp/Resampler.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Image.hpp>
#include <fwDataTools/helper/MedicalImageAdaptor.hpp>
#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwItkIO/itk.hpp>

#include <fwTest/generator/Image.hpp>

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>

#include <itkImage.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkResampleImageFilter.h>

CPPUNIT_TEST_SUITE_REGISTRATION(::itkRegistrationOp::ut::FastRegistrationTest);

namespace itkRegistrationOp
{
namespace ut
{

namespace itkReg = ::itkRegistrationOp;

//------------------------------------------------------------------------------

void FastRegistrationTest::setUp()
{
}

//------------------------------------------------------------------------------

void FastRegistrationTest::tearDown()
{
}

//------------------------------------------------------------------------------

void FastRegistrationTest::identityTest()
{
    ::fwData::Image::csptr target              = createSphereImage< ::std::uint16_t, 3>();
    ::fwData::Image::sptr reference            = ::fwData::Object::copy(target);
    ::fwData::TransformationMatrix3D::sptr mat = ::fwData::TransformationMatrix3D::New();
    auto trans = itkReg::FastRegistration<float>::registerImage(target, reference);

    for(size_t i = 0; i != 3; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Translation value is not equal to '0' ", 0., trans[i], 1e-8);
    }
}

//------------------------------------------------------------------------------

void FastRegistrationTest::translateTransformTest()
{
    ::fwData::Image::csptr target   = createSphereImage< ::std::uint16_t, 3>();
    ::fwData::Image::sptr reference = ::fwData::Image::New();

    ::fwData::TransformationMatrix3D::sptr transform = ::fwData::TransformationMatrix3D::New();
    transform->setCoefficient(0, 3, 4.);
    transform->setCoefficient(1, 3, 12.);
    transform->setCoefficient(2, 3, 7.);
    itkReg::Resampler::resample(target, reference, transform);

    std::array<float, 3> expected { -4., -12., -7. };
    auto actual = itkReg::FastRegistration<float>::registerImage(target, reference);
    for(size_t i = 0; i < 3; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Actual transform does not match expected results",
                                             expected[i], actual[i], 1e-2);
    }
}

//------------------------------------------------------------------------------

void FastRegistrationTest::translateTransformWithScalesTest()
{
    using ImageType = ::itk::Image< std::uint16_t, 3>;

    ::fwData::Image::sptr target    = createSphereImage< ::std::uint16_t, 3>();
    ::fwData::Image::sptr reference = ::fwData::Image::New();
    target->setOrigin({ 0., 0., 0. });
    target->setSpacing({ 1., 1., 1. });

    // Translate the image a bit
    ::fwData::TransformationMatrix3D::sptr transform = ::fwData::TransformationMatrix3D::New();
    transform->setCoefficient(0, 3, 4.);
    transform->setCoefficient(1, 3, 12.);
    transform->setCoefficient(2, 3, 7.);
    itkReg::Resampler::resample(target, reference, transform);
    auto itkReference = ::fwItkIO::itkImageFactory<ImageType>(reference, false);
    std::array<float, 3> expected { -4., -12., -7. };

    // Extract a part of the image containing the ellipsoid
    ImageType::RegionType subRegion;
    subRegion.SetIndex(0, 20);
    subRegion.SetIndex(1, 20);
    subRegion.SetIndex(2, 20);
    subRegion.SetSize(0, 60);
    subRegion.SetSize(1, 60);
    subRegion.SetSize(2, 60);
    auto extract = ::itk::RegionOfInterestImageFilter<ImageType, ImageType>::New();
    extract->SetInput(itkReference);
    extract->SetRegionOfInterest(subRegion);
    extract->Update();
    auto extracted       = extract->GetOutput();
    auto extractedOrigin = extracted->GetOrigin();
    expected[0] -= extractedOrigin[0];
    expected[1] -= extractedOrigin[1];
    expected[2] -= extractedOrigin[2];

    // Resample the image to get a different spacing
    ImageType::SizeType newSize;
    ImageType::SpacingType newSpacing(2.);
    newSize.Fill(30);
    auto resample = ::itk::ResampleImageFilter<ImageType, ImageType>::New();
    resample->SetInput(extracted);
    resample->SetSize(newSize);
    resample->SetOutputSpacing(newSpacing);
    resample->SetOutputOrigin(extractedOrigin);
    resample->Update();
    auto resampled             = resample->GetOutput();
    auto resampledF4sReference = ::fwItkIO::dataImageFactory<ImageType>(resampled, true);

    auto actual = itkReg::FastRegistration<float>::registerImage(target, resampledF4sReference);
    for(size_t i = 0; i < 3; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Actual transform does not match expected results",
                                             expected[i], actual[i], 1e-2);
    }
}

} // ut
} // itkRegistrationOp
