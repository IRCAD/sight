/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "mip_matching_registration_test.hpp"

#include "helper.hpp"

#include <core/tools/dispatcher.hpp>

#include <data/helper/MedicalImage.hpp>
#include <data/matrix4.hpp>

#include <filter/image/metric.hpp>
#include <filter/image/mip_matching_registration.hpp>
#include <filter/image/resampler.hpp>

#include <io/itk/itk.hpp>

#include <utest_data/generator/image.hpp>

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>

#include <itkImage.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkResampleImageFilter.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::mip_matching_registration_test);

namespace sight::filter::image::ut
{

//------------------------------------------------------------------------------

void mip_matching_registration_test::setUp()
{
}

//------------------------------------------------------------------------------

void mip_matching_registration_test::tearDown()
{
}

//------------------------------------------------------------------------------

void mip_matching_registration_test::identityTest()
{
    data::image::csptr moving = createSphereImage<std::uint16_t, 3>();
    data::image::csptr fixed  = data::object::copy(moving);
    data::matrix4::sptr mat   = std::make_shared<data::matrix4>();

    filter::image::RegistrationDispatch::Parameters params;
    params.fixed     = fixed;
    params.moving    = moving;
    params.transform = std::make_shared<data::matrix4>();
    core::type type = moving->getType();
    core::tools::dispatcher<core::tools::supported_dispatcher_types, RegistrationDispatch>::invoke(type, params);

    for(std::size_t i = 0 ; i != 3 ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Translation value is not equal to '0' ",
            0.,
            (*params.transform)(i, 3)
        );
    }
}

//------------------------------------------------------------------------------

void mip_matching_registration_test::translateTransformTest()
{
    data::image::csptr moving = createSphereImage<std::uint16_t, 3>();
    data::image::sptr fixed   = std::make_shared<data::image>();

    data::matrix4::sptr transform = std::make_shared<data::matrix4>();
    (*transform)(0, 3) = 4.;
    (*transform)(1, 3) = 12.;
    (*transform)(2, 3) = 7.;
    sight::filter::image::resampler::resample(moving, fixed, transform);

    std::array<double, 3> expected {{4., 12., 7.}};
    filter::image::RegistrationDispatch::Parameters params;
    params.fixed     = fixed;
    params.moving    = moving;
    params.transform = std::make_shared<data::matrix4>();
    core::type type = moving->getType();
    core::tools::dispatcher<core::tools::supported_dispatcher_types, RegistrationDispatch>::invoke(type, params);
    for(std::size_t i = 0 ; i < 3 ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "Actual transform does not match expected results",
            expected[i],
            (*params.transform)(i, 3),
            1e-2
        );
    }
}

//------------------------------------------------------------------------------

void mip_matching_registration_test::translateTransformWithScalesTest()
{
    using ImageType = itk::Image<std::uint16_t, 3>;

    // Create the moving image
    auto movingSpacing = ImageType::SpacingType(1.);
    movingSpacing[1] = 1.3;
    data::image::sptr moving = createSphereImage<std::uint16_t, 3>(movingSpacing);
    data::image::sptr fixed  = std::make_shared<data::image>();
    moving->setOrigin({107., 50., -30.});

    // Translate the image a bit
    std::array<double, 3> vTrans {{4., 19., 7.}};
    data::matrix4::sptr transform = std::make_shared<data::matrix4>();
    (*transform)(0, 3) = vTrans[0];
    (*transform)(1, 3) = vTrans[1];
    (*transform)(2, 3) = vTrans[2];
    sight::filter::image::resampler::resample(moving, fixed, transform);
    auto fixedOrigin  = std::array<double, 3> {{20., 10., 35.}};
    auto movingOrigin = moving->getOrigin();
    fixed->setOrigin(fixedOrigin);
    std::array<float, 3> expected {
        {
            float(movingOrigin[0] + vTrans[0] - fixedOrigin[0]),
            float(movingOrigin[1] + vTrans[1] - fixedOrigin[1]),
            float(movingOrigin[2] + vTrans[2] - fixedOrigin[2])
        }
    };

    auto itkFixed = io::itk::moveToItk<ImageType>(fixed);

    // Resample the image to get a different spacing
    ImageType::SizeType newSize {};
    ImageType::SpacingType newSpacing(2.);
    for(uint8_t i = 0 ; i != 3 ; ++i)
    {
        newSize[i] = static_cast<unsigned int>(movingSpacing[i] / newSpacing[i] * double(moving->size()[i]));
    }

    auto resample = itk::ResampleImageFilter<ImageType, ImageType>::New();
    resample->SetInput(itkFixed);
    resample->SetSize(newSize);
    resample->SetOutputSpacing(newSpacing);
    resample->SetOutputOrigin(itkFixed->GetOrigin());
    resample->Update();
    auto* resampled        = resample->GetOutput();
    auto resampledF4sFixed = io::itk::moveFromItk<ImageType>(resampled, true);

    filter::image::RegistrationDispatch::Parameters params;
    params.fixed     = resampledF4sFixed;
    params.moving    = moving;
    params.transform = std::make_shared<data::matrix4>();
    core::type type = moving->getType();
    core::tools::dispatcher<core::tools::supported_dispatcher_types, RegistrationDispatch>::invoke(type, params);
    for(std::size_t i = 0 ; i < 3 ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "Actual transform does not match expected results",
            double(expected[i]),
            (*params.transform)(i, 3),
            double(movingSpacing[i])
        );
    }
}

} // namespace sight::filter::image::ut
