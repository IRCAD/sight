/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
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

#include <fwTools/Dispatcher.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>
#include <fwTools/IntrinsicTypes.hpp>

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

    ::itkRegistrationOp::RegistrationDispatch::Parameters params;
    params.source               = reference;
    params.target               = target;
    params.transform            = ::fwData::TransformationMatrix3D::New();
    ::fwTools::DynamicType type = target->getPixelType();
    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, RegistrationDispatch >::invoke( type, params );

    for(size_t i = 0; i != 3; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Translation value is not equal to '0' ", 0.,
                                             params.transform->getCoefficient(i, 3), 1e-8);
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

    std::array<double, 3> expected {{ 4., 12., 7. }};
    ::itkRegistrationOp::RegistrationDispatch::Parameters params;
    params.source               = reference;
    params.target               = target;
    params.transform            = ::fwData::TransformationMatrix3D::New();
    ::fwTools::DynamicType type = target->getPixelType();
    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, RegistrationDispatch >::invoke( type, params );
    for(size_t i = 0; i < 3; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Actual transform does not match expected results",
                                             expected[i], params.transform->getCoefficient(i, 3), 1e-2);
    }
}

//------------------------------------------------------------------------------

void FastRegistrationTest::translateTransformWithScalesTest()
{
    using ImageType = ::itk::Image< std::uint16_t, 3>;

    // Create the target image
    auto targetSpacing = ImageType::SpacingType(1.);
    targetSpacing[1]                = 1.3;
    ::fwData::Image::sptr target    = createSphereImage< ::std::uint16_t, 3>(targetSpacing);
    ::fwData::Image::sptr reference = ::fwData::Image::New();
    target->setOrigin({ 107., 50., -30. });

    // Translate the image a bit
    std::array<double, 3> vTrans {{ 4., 19., 7. }};
    ::fwData::TransformationMatrix3D::sptr transform = ::fwData::TransformationMatrix3D::New();
    transform->setCoefficient(0, 3, vTrans[0]);
    transform->setCoefficient(1, 3, vTrans[1]);
    transform->setCoefficient(2, 3, vTrans[2]);
    itkReg::Resampler::resample(target, reference, transform);
    auto referenceOrigin = std::vector<double> {{ 20., 10., 35. }},
         targetOrigin    = target->getOrigin();
    reference->setOrigin(referenceOrigin);
    std::array<float, 3> expected {
        {
            float(targetOrigin[0] + vTrans[0] - referenceOrigin[0]),
            float(targetOrigin[1] + vTrans[1] - referenceOrigin[1]),
            float(targetOrigin[2] + vTrans[2] - referenceOrigin[2])
        }
    };

    auto itkReference = ::fwItkIO::itkImageFactory<ImageType>(reference, false);

    // Resample the image to get a different spacing
    ImageType::SizeType newSize;
    ImageType::SpacingType newSpacing(2.);
    for(uint8_t i = 0; i != 3; ++i)
    {
        newSize[i] = static_cast<unsigned int>(targetSpacing[i] / newSpacing[i] * target->getSize()[i]);
    }
    auto resample = ::itk::ResampleImageFilter<ImageType, ImageType>::New();
    resample->SetInput(itkReference);
    resample->SetSize(newSize);
    resample->SetOutputSpacing(newSpacing);
    resample->SetOutputOrigin(itkReference->GetOrigin());
    resample->Update();
    auto resampled             = resample->GetOutput();
    auto resampledF4sReference = ::fwItkIO::dataImageFactory<ImageType>(resampled, true);

    ::itkRegistrationOp::RegistrationDispatch::Parameters params;
    params.source               = resampledF4sReference;
    params.target               = target;
    params.transform            = ::fwData::TransformationMatrix3D::New();
    ::fwTools::DynamicType type = target->getPixelType();
    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, RegistrationDispatch >::invoke( type, params );
    for(size_t i = 0; i < 3; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Actual transform does not match expected results",
                                             double(expected[i]), params.transform->getCoefficient(i, 3),
                                             double(targetSpacing[i]));
    }
}

} // ut
} // itkRegistrationOp
