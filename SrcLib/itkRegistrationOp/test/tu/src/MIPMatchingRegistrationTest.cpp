/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "MIPMatchingRegistrationTest.hpp"

#include "helper.hpp"

#include <itkRegistrationOp/Metric.hpp>
#include <itkRegistrationOp/MIPMatchingRegistration.hpp>
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
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#undef GLM_ENABLE_EXPERIMENTAL
#include <glm/mat4x4.hpp>

#include <itkImage.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkResampleImageFilter.h>

CPPUNIT_TEST_SUITE_REGISTRATION(::itkRegistrationOp::ut::MIPMatchingRegistrationTest);

namespace itkRegistrationOp
{
namespace ut
{

namespace itkReg = ::itkRegistrationOp;

//------------------------------------------------------------------------------

void MIPMatchingRegistrationTest::setUp()
{
}

//------------------------------------------------------------------------------

void MIPMatchingRegistrationTest::tearDown()
{
}

//------------------------------------------------------------------------------

void MIPMatchingRegistrationTest::identityTest()
{
    ::fwData::Image::csptr moving              = createSphereImage< ::std::uint16_t, 3>();
    ::fwData::Image::csptr fixed               = ::fwData::Object::copy(moving);
    ::fwData::TransformationMatrix3D::sptr mat = ::fwData::TransformationMatrix3D::New();

    ::itkRegistrationOp::RegistrationDispatch::Parameters params;
    params.fixed                = fixed;
    params.moving               = moving;
    params.transform            = ::fwData::TransformationMatrix3D::New();
    ::fwTools::DynamicType type = moving->getPixelType();
    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, RegistrationDispatch >::invoke( type, params );

    for(size_t i = 0; i != 3; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Translation value is not equal to '0' ", 0.,
                                             params.transform->getCoefficient(i, 3), 1e-8);
    }
}

//------------------------------------------------------------------------------

void MIPMatchingRegistrationTest::translateTransformTest()
{
    ::fwData::Image::csptr moving = createSphereImage< ::std::uint16_t, 3>();
    ::fwData::Image::sptr fixed   = ::fwData::Image::New();

    ::fwData::TransformationMatrix3D::sptr transform = ::fwData::TransformationMatrix3D::New();
    transform->setCoefficient(0, 3, 4.);
    transform->setCoefficient(1, 3, 12.);
    transform->setCoefficient(2, 3, 7.);
    itkReg::Resampler::resample(moving, fixed, transform);

    std::array<double, 3> expected {{ 4., 12., 7. }};
    ::itkRegistrationOp::RegistrationDispatch::Parameters params;
    params.fixed                = fixed;
    params.moving               = moving;
    params.transform            = ::fwData::TransformationMatrix3D::New();
    ::fwTools::DynamicType type = moving->getPixelType();
    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, RegistrationDispatch >::invoke( type, params );
    for(size_t i = 0; i < 3; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Actual transform does not match expected results",
                                             expected[i], params.transform->getCoefficient(i, 3), 1e-2);
    }
}

//------------------------------------------------------------------------------

void MIPMatchingRegistrationTest::translateTransformWithScalesTest()
{
    using ImageType = ::itk::Image< std::uint16_t, 3>;

    // Create the moving image
    auto movingSpacing = ImageType::SpacingType(1.);
    movingSpacing[1]             = 1.3;
    ::fwData::Image::sptr moving = createSphereImage< ::std::uint16_t, 3>(movingSpacing);
    ::fwData::Image::sptr fixed  = ::fwData::Image::New();
    moving->setOrigin({ 107., 50., -30. });

    // Translate the image a bit
    std::array<double, 3> vTrans {{ 4., 19., 7. }};
    ::fwData::TransformationMatrix3D::sptr transform = ::fwData::TransformationMatrix3D::New();
    transform->setCoefficient(0, 3, vTrans[0]);
    transform->setCoefficient(1, 3, vTrans[1]);
    transform->setCoefficient(2, 3, vTrans[2]);
    itkReg::Resampler::resample(moving, fixed, transform);
    auto fixedOrigin  = std::vector<double> {{ 20., 10., 35. }},
         movingOrigin = moving->getOrigin();
    fixed->setOrigin(fixedOrigin);
    std::array<float, 3> expected {
        {
            float(movingOrigin[0] + vTrans[0] - fixedOrigin[0]),
            float(movingOrigin[1] + vTrans[1] - fixedOrigin[1]),
            float(movingOrigin[2] + vTrans[2] - fixedOrigin[2])
        }
    };

    auto itkFixed = ::fwItkIO::itkImageFactory<ImageType>(fixed, false);

    // Resample the image to get a different spacing
    ImageType::SizeType newSize;
    ImageType::SpacingType newSpacing(2.);
    for(uint8_t i = 0; i != 3; ++i)
    {
        newSize[i] = static_cast<unsigned int>(movingSpacing[i] / newSpacing[i] * moving->getSize()[i]);
    }
    auto resample = ::itk::ResampleImageFilter<ImageType, ImageType>::New();
    resample->SetInput(itkFixed);
    resample->SetSize(newSize);
    resample->SetOutputSpacing(newSpacing);
    resample->SetOutputOrigin(itkFixed->GetOrigin());
    resample->Update();
    auto resampled         = resample->GetOutput();
    auto resampledF4sFixed = ::fwItkIO::dataImageFactory<ImageType>(resampled, true);

    ::itkRegistrationOp::RegistrationDispatch::Parameters params;
    params.fixed                = resampledF4sFixed;
    params.moving               = moving;
    params.transform            = ::fwData::TransformationMatrix3D::New();
    ::fwTools::DynamicType type = moving->getPixelType();
    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, RegistrationDispatch >::invoke( type, params );
    for(size_t i = 0; i < 3; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Actual transform does not match expected results",
                                             double(expected[i]), params.transform->getCoefficient(i, 3),
                                             double(movingSpacing[i]));
    }
}

} // ut
} // itkRegistrationOp
