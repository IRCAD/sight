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

#include "resampler.hpp"

#include <core/tools/dispatcher.hpp>

#include <geometry/data/image.hpp>
#include <geometry/data/matrix4.hpp>

#include <io/itk/helper/transform.hpp>
#include <io/itk/itk.hpp>

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <itkAffineTransform.h>
#include <itkBoundingBox.h>
#include <itkBSplineInterpolateImageFunction.h>
#include <itkMatrix.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkResampleImageFilter.h>

namespace sight::filter::image
{

struct resampling
{
    struct parameters
    {
        itk::AffineTransform<double, 3>::Pointer i_trf;
        data::image::csptr i_image;
        data::image::sptr o_image;
        std::optional<std::tuple<data::image::size_t,
                                 data::image::origin_t,
                                 data::image::orientation_t,
                                 data::image::spacing_t> > i_parameters;
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(parameters& _params)
    {
        using image_t = typename itk::Image<PIXELTYPE, 3>;
        const typename image_t::Pointer itk_image = io::itk::move_to_itk<image_t>(_params.i_image);

        typename itk::ResampleImageFilter<image_t, image_t>::Pointer resampler =
            itk::ResampleImageFilter<image_t, image_t>::New();

        typename itk::MinimumMaximumImageCalculator<image_t>::Pointer min_calculator =
            itk::MinimumMaximumImageCalculator<image_t>::New();

        min_calculator->SetImage(itk_image);
        min_calculator->ComputeMinimum();
        resampler->SetDefaultPixelValue(min_calculator->GetMinimum());

        resampler->SetTransform(_params.i_trf.GetPointer());
        resampler->SetInput(itk_image);

        auto size      = itk_image->GetLargestPossibleRegion().GetSize();
        auto origin    = itk_image->GetOrigin();
        auto spacing   = itk_image->GetSpacing();
        auto direction = itk_image->GetDirection();

        SIGHT_ASSERT("Input spacing can't be null along any axis", spacing[0] > 0 && spacing[1] > 0 && spacing[2] > 0);

        if(_params.i_parameters.has_value())
        {
            const auto& [out_size, out_origin, out_orientation, out_spacing] = _params.i_parameters.value();

            for(std::uint8_t i = 0 ; i < 3 ; ++i)
            {
                // ITK uses unsigned long to store sizes.
                size[i] = static_cast<typename image_t::SizeType::SizeValueType>(out_size[i]);

                origin[i]  = out_origin[i];
                spacing[i] = out_spacing[i];

                SIGHT_ASSERT("Output spacing can't be null along any axis.", spacing[i] > 0);
            }

            direction(0, 0) = out_orientation[0];
            direction(0, 1) = out_orientation[1];
            direction(0, 2) = out_orientation[2];
            direction(1, 0) = out_orientation[3];
            direction(1, 1) = out_orientation[4];
            direction(1, 2) = out_orientation[5];
            direction(2, 0) = out_orientation[6];
            direction(2, 1) = out_orientation[7];
            direction(2, 2) = out_orientation[8];
        }

        resampler->SetSize(size);
        resampler->SetOutputOrigin(origin);
        resampler->SetOutputDirection(direction);
        resampler->SetOutputSpacing(spacing);

        resampler->Update();

        typename image_t::Pointer output_image = resampler->GetOutput();

        io::itk::move_from_itk(output_image, _params.o_image);
    }
};

//-----------------------------------------------------------------------------

void resampler::resample(
    const data::image::csptr& _in_image,
    const data::image::sptr& _out_image,
    const data::matrix4::csptr& _trf,
    std::optional<std::tuple<data::image::size_t,
                             data::image::origin_t,
                             data::image::orientation_t,
                             data::image::spacing_t> > _parameters
)
{
    const itk::Matrix<double, 4, 4> itk_matrix = io::itk::helper::transform::convert_to_itk(_trf);

    // We need to extract a 3x3 matrix and a vector to set the affine transform.
    itk::Matrix<double, 3, 3> transform_mat;

    for(std::uint8_t i = 0 ; i < 3 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 3 ; ++j)
        {
            transform_mat(i, j) = itk_matrix(i, j);
        }
    }

    itk::Vector<double, 3> translation;
    for(std::uint8_t i = 0 ; i < 3 ; ++i)
    {
        // Extract the last column to get the translation.
        translation.SetElement(i, itk_matrix(i, 3));
    }

    itk::AffineTransform<double, 3>::Pointer transf = itk::AffineTransform<double, 3>::New();
    transf->SetMatrix(transform_mat);
    transf->SetTranslation(translation);

    resampling::parameters params;
    params.i_image      = _in_image;
    params.o_image      = _out_image;
    params.i_trf        = transf.GetPointer();
    params.i_parameters = _parameters;

    const core::type type = _in_image->type();
    core::tools::dispatcher<core::tools::supported_dispatcher_types, resampling>::invoke(type, params);
}

//-----------------------------------------------------------------------------

data::image::sptr resampler::resample(
    const data::image::csptr& _img,
    const data::matrix4::csptr& _trf,
    const data::image::spacing_t& _output_spacing
)
{
    const auto& input_origin      = _img->origin();
    const auto& input_orientation = _img->orientation();

    const glm::dmat4 image_to_world_transform {
        input_orientation[0], input_orientation[3], input_orientation[6], 0,
        input_orientation[1], input_orientation[4], input_orientation[7], 0,
        input_orientation[2], input_orientation[5], input_orientation[8], 0,
        input_origin[0], input_origin[1], input_origin[2], 1
    };

    // Get the inverse transform
    const auto glm_inverse = glm::inverse(geometry::data::to_glm_mat(*_trf));

    // Compute the new origin and orientation.
    const auto glm_origin_world = image_to_world_transform * glm_inverse;
    const data::image::origin_t output_origin {
        glm_origin_world[3][0],
        glm_origin_world[3][1],
        glm_origin_world[3][2]
    };

    const data::image::orientation_t output_orientation {
        glm_origin_world[0][0], glm_origin_world[1][0], glm_origin_world[2][0],
        glm_origin_world[0][1], glm_origin_world[1][1], glm_origin_world[2][1],
        glm_origin_world[0][2], glm_origin_world[1][2], glm_origin_world[2][2]
    };

    // Compute the new size
    // Get the max point in current world.
    const auto max = sight::geometry::data::image_to_world(*_img, _img->size());
    glm::dvec4 glm_max {max[0], max[1], max[2], 1.};

    // Apply original and new transforms
    glm_max = glm_max * glm_origin_world;

    // Use the new given spacing to compute the new size.
    const data::image::size_t output_size {
        data::image::size_t::value_type(std::round(glm_max[0] / _output_spacing[0])),
        data::image::size_t::value_type(std::round(glm_max[1] / _output_spacing[1])),
        data::image::size_t::value_type(std::round(glm_max[2] / _output_spacing[2])),
    };

    data::image::sptr output = std::make_shared<data::image>();
    output->set_origin(output_origin);
    output->set_orientation(output_orientation);
    output->set_spacing(_output_spacing);

    resample(
        _img,
        output,
        _trf,
        std::make_tuple(
            output_size,
            output->origin(),
            output->orientation(),
            _output_spacing
        )
    );

    return output;
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::image
