/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "MIPMatchingRegistrationTest.hpp"

#include "helper.hpp"

#include <core/tools/Dispatcher.hpp>
#include <core/tools/TypeKeyTypeMapping.hpp>

#include <data/Matrix4.hpp>
#include <data/fieldHelper/MedicalImageHelpers.hpp>

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>

#include <utestData/generator/Image.hpp>

#include <filter/image/Metric.hpp>
#include <filter/image/MIPMatchingRegistration.hpp>
#include <filter/image/Resampler.hpp>

#include <io/itk/itk.hpp>

#include <itkImage.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkResampleImageFilter.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::MIPMatchingRegistrationTest);

namespace sight::filter::image
{
namespace ut
{

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
    data::Image::csptr moving = createSphereImage< ::std::uint16_t, 3>();
    data::Image::csptr fixed  = data::Object::copy(moving);
    data::Matrix4::sptr mat   = data::Matrix4::New();

    filter::image::RegistrationDispatch::Parameters params;
    params.fixed     = fixed;
    params.moving    = moving;
    params.transform = data::Matrix4::New();
    core::tools::Type type = moving->getType();
    core::tools::Dispatcher< core::tools::SupportedDispatcherTypes, RegistrationDispatch >::invoke( type, params );

    for(size_t i = 0; i != 3; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Translation value is not equal to '0' ", 0.,
                                             params.transform->getCoefficient(i, 3), 1e-8);
    }
}

//------------------------------------------------------------------------------

void MIPMatchingRegistrationTest::translateTransformTest()
{
    data::Image::csptr moving = createSphereImage< ::std::uint16_t, 3>();
    data::Image::sptr fixed   = data::Image::New();

    data::Matrix4::sptr transform = data::Matrix4::New();
    transform->setCoefficient(0, 3, 4.);
    transform->setCoefficient(1, 3, 12.);
    transform->setCoefficient(2, 3, 7.);
    sight::filter::image::Resampler::resample(moving, fixed, transform);

    std::array<double, 3> expected {{ 4., 12., 7. }};
    filter::image::RegistrationDispatch::Parameters params;
    params.fixed     = fixed;
    params.moving    = moving;
    params.transform = data::Matrix4::New();
    core::tools::Type type = moving->getType();
    core::tools::Dispatcher< core::tools::SupportedDispatcherTypes, RegistrationDispatch >::invoke( type, params );
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
    movingSpacing[1] = 1.3;
    data::Image::sptr moving = createSphereImage< ::std::uint16_t, 3>(movingSpacing);
    data::Image::sptr fixed  = data::Image::New();
    moving->setOrigin2({ 107., 50., -30. });

    // Translate the image a bit
    std::array<double, 3> vTrans {{ 4., 19., 7. }};
    data::Matrix4::sptr transform = data::Matrix4::New();
    transform->setCoefficient(0, 3, vTrans[0]);
    transform->setCoefficient(1, 3, vTrans[1]);
    transform->setCoefficient(2, 3, vTrans[2]);
    sight::filter::image::Resampler::resample(moving, fixed, transform);
    auto fixedOrigin  = std::array<double, 3> {{ 20., 10., 35. }},
         movingOrigin = moving->getOrigin2();
    fixed->setOrigin2(fixedOrigin);
    std::array<float, 3> expected {
        {
            float(movingOrigin[0] + vTrans[0] - fixedOrigin[0]),
            float(movingOrigin[1] + vTrans[1] - fixedOrigin[1]),
            float(movingOrigin[2] + vTrans[2] - fixedOrigin[2])
        }
    };

    auto itkFixed = io::itk::itkImageFactory<ImageType>(fixed, false);

    // Resample the image to get a different spacing
    ImageType::SizeType newSize;
    ImageType::SpacingType newSpacing(2.);
    for(uint8_t i = 0; i != 3; ++i)
    {
        newSize[i] = static_cast<unsigned int>(movingSpacing[i] / newSpacing[i] * moving->getSize2()[i]);
    }
    auto resample = ::itk::ResampleImageFilter<ImageType, ImageType>::New();
    resample->SetInput(itkFixed);
    resample->SetSize(newSize);
    resample->SetOutputSpacing(newSpacing);
    resample->SetOutputOrigin(itkFixed->GetOrigin());
    resample->Update();
    auto resampled         = resample->GetOutput();
    auto resampledF4sFixed = io::itk::dataImageFactory<ImageType>(resampled, true);

    filter::image::RegistrationDispatch::Parameters params;
    params.fixed     = resampledF4sFixed;
    params.moving    = moving;
    params.transform = data::Matrix4::New();
    core::tools::Type type = moving->getType();
    core::tools::Dispatcher< core::tools::SupportedDispatcherTypes, RegistrationDispatch >::invoke( type, params );
    for(size_t i = 0; i < 3; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Actual transform does not match expected results",
                                             double(expected[i]), params.transform->getCoefficient(i, 3),
                                             double(movingSpacing[i]));
    }
}

} // ut
} // itkRegistrationOp
