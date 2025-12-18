/************************************************************************
 *
 * Copyright (C) 2017-2025 IRCAD France
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

// cspell:ignore FFTWFFT

#include "helper.hpp"

#include <core/tools/dispatcher.hpp>

#include <data/helper/medical_image.hpp>
#include <data/matrix4.hpp>

#include <filter/image/metric.hpp>
#include <filter/image/mip_matching_registration.hpp>
#include "filter/image/detail/mip_matching_registration.hxx"
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

#if ITK_VERSION_MAJOR >= 5 && ITK_VERSION_MINOR >= 3 && defined(__unix__)
#include <itkFFTWFFTImageFilterInitFactory.h>
#endif

#include <doctest/doctest.h>

TEST_SUITE("sight::filter::image::mip_matching_registration")
{
//------------------------------------------------------------------------------

    TEST_CASE("identity")
    {
#if ITK_VERSION_MAJOR >= 5 && ITK_VERSION_MINOR >= 3 && defined(__unix__)
        auto factory = ::itk::FFTWFFTImageFilterInitFactory::New();
        itk::FFTWFFTImageFilterInitFactory::RegisterFactories();
#endif

        sight::data::image::csptr moving = create_sphere_image<std::uint16_t, 3>();
        sight::data::image::csptr fixed  = sight::data::object::copy(moving);

        sight::data::matrix4::sptr mat = std::make_shared<sight::data::matrix4>();

        sight::filter::image::registration_dispatch::parameters params;
        params.fixed     = fixed;
        params.moving    = moving;
        params.transform = std::make_shared<sight::data::matrix4>();

        sight::core::type type = moving->type();
        sight::core::tools::dispatcher<sight::core::tools::supported_dispatcher_types,
                                       sight::filter::image::registration_dispatch>::invoke(type, params);

        for(std::size_t i = 0 ; i != 3 ; ++i)
        {
            CHECK_MESSAGE(
                0. == (*params.transform)(i, 3),
                "Translation value is not equal to '0' "
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("translate_transform")
    {
#if ITK_VERSION_MAJOR >= 5 && ITK_VERSION_MINOR >= 3 && defined(__unix__)
        auto factory = ::itk::FFTWFFTImageFilterInitFactory::New();
        itk::FFTWFFTImageFilterInitFactory::RegisterFactories();
#endif

        sight::data::image::csptr moving = create_sphere_image<std::uint16_t, 3>();
        sight::data::image::sptr fixed   = std::make_shared<sight::data::image>();

        sight::data::matrix4::sptr transform = std::make_shared<sight::data::matrix4>();
        (*transform)(0, 3) = 4.;
        (*transform)(1, 3) = 12.;
        (*transform)(2, 3) = 7.;

        sight::filter::image::resampler::resample(moving, fixed, transform);

        std::array<double, 3> expected {{4., 12., 7.}};

        sight::filter::image::registration_dispatch::parameters params;
        params.fixed     = fixed;
        params.moving    = moving;
        params.transform = std::make_shared<sight::data::matrix4>();

        sight::core::type type = moving->type();
        sight::core::tools::dispatcher<sight::core::tools::supported_dispatcher_types,
                                       sight::filter::image::registration_dispatch>::invoke(type, params);

        for(std::size_t i = 0 ; i < 3 ; ++i)
        {
            CHECK_MESSAGE(
                doctest::Approx(expected[i]).epsilon(1e-2) == (*params.transform)(i, 3),
                "Actual transform does not match expected results"
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("translate_transform_with_scales")
    {
#if ITK_VERSION_MAJOR >= 5 && ITK_VERSION_MINOR >= 3 && defined(__unix__)
        auto factory = ::itk::FFTWFFTImageFilterInitFactory::New();
        itk::FFTWFFTImageFilterInitFactory::RegisterFactories();
#endif

        using image_t = itk::Image<std::uint16_t, 3>;

        // Create the moving image
        auto moving_spacing = image_t::SpacingType(1.);
        moving_spacing[1] = 1.3;

        sight::data::image::sptr moving = create_sphere_image<std::uint16_t, 3>(moving_spacing);
        sight::data::image::sptr fixed  = std::make_shared<sight::data::image>();

        moving->set_origin({107., 50., -30.});

        // Translate the image a bit
        std::array<double, 3> v_trans {{4., 19., 7.}};

        sight::data::matrix4::sptr transform = std::make_shared<sight::data::matrix4>();
        (*transform)(0, 3) = v_trans[0];
        (*transform)(1, 3) = v_trans[1];
        (*transform)(2, 3) = v_trans[2];

        sight::filter::image::resampler::resample(moving, fixed, transform);

        auto fixed_origin  = std::array<double, 3> {{20., 10., 35.}};
        auto moving_origin = moving->origin();

        fixed->set_origin(fixed_origin);

        std::array<float, 3> expected
        {
            {
                float(moving_origin[0] + v_trans[0] - fixed_origin[0]),
                float(moving_origin[1] + v_trans[1] - fixed_origin[1]),
                float(moving_origin[2] + v_trans[2] - fixed_origin[2])
            }
        };

        auto itk_fixed = sight::io::itk::move_to_itk<image_t>(fixed);

        // Resample the image to get a different spacing
        image_t::SizeType new_size {};
        image_t::SpacingType new_spacing(2.);

        for(uint8_t i = 0 ; i != 3 ; ++i)
        {
            new_size[i] = static_cast<unsigned int>(moving_spacing[i] / new_spacing[i] * double(moving->size()[i]));
        }

        auto resample = itk::ResampleImageFilter<image_t, image_t>::New();
        resample->SetInput(itk_fixed);
        resample->SetSize(new_size);
        resample->SetOutputSpacing(new_spacing);
        resample->SetOutputOrigin(itk_fixed->GetOrigin());
        resample->Update();

        auto* resampled          = resample->GetOutput();
        auto resampled_f4s_fixed = sight::io::itk::move_from_itk<image_t>(resampled, true);

        sight::filter::image::registration_dispatch::parameters params;
        params.fixed     = resampled_f4s_fixed;
        params.moving    = moving;
        params.transform = std::make_shared<sight::data::matrix4>();

        sight::core::type type = moving->type();
        sight::core::tools::dispatcher<sight::core::tools::supported_dispatcher_types,
                                       sight::filter::image::registration_dispatch>::invoke(type, params);

        for(std::size_t i = 0 ; i < 3 ; ++i)
        {
            CHECK_MESSAGE(
                doctest::Approx(double(expected[i])).epsilon(double(moving_spacing[std::uint32_t(i)]))
                == (*params.transform)(i, 3),
                "Actual transform does not match expected results"
            );
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::filter::image::mip_matching_registration")
