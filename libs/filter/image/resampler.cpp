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

#include "resampler.hpp"

#include <core/tools/dispatcher.hpp>

#include <io/itk/helper/transform.hpp>
#include <io/itk/itk.hpp>

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
            auto& [outSize, outOrigin, outSpacing] = _params.i_parameters.value();
            for(std::uint8_t i = 0 ; i < 3 ; ++i)
            {
                // ITK uses unsigned long to store sizes.
                size[i] = static_cast<typename image_t::SizeType::SizeValueType>(outSize[i]);

                origin[i]  = outOrigin[i];
                spacing[i] = outSpacing[i];

                SIGHT_ASSERT("Output spacing can't be null along any axis.", spacing[i] > 0);
            }
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
    using point_t            = itk::Point<double, 3>;
    using vector_container_t = itk::VectorContainer<int, point_t>;
    using bounding_box_t     = itk::BoundingBox<int, 3, double, vector_container_t>;

    const auto& input_size    = _img->size();
    const auto& input_origin  = _img->origin();
    const auto& input_spacing = _img->spacing();

    SIGHT_ASSERT(
        "image dimension must be 3.",
        input_origin.size() == 3 && input_spacing.size() == 3 && input_size.size() == 3
    );

    typename bounding_box_t::Pointer input_bb = bounding_box_t::New();

    const point_t min(input_origin.data());
    point_t max;
    for(std::uint8_t i = 0 ; i < 3 ; ++i)
    {
        max[i] = input_origin[i] + static_cast<double>(input_size[i]) * input_spacing[i];
    }

    input_bb->SetMinimum(min);
    input_bb->SetMaximum(max);

    const auto input_corners = input_bb->ComputeCorners();
    const itk::Matrix<double, 4, 4> matrix(io::itk::helper::transform::convert_to_itk(_trf).GetInverse());

    // Apply transform matrix to all bounding box corners.
    typename vector_container_t::Pointer output_corners = vector_container_t::New();
    output_corners->Reserve(input_corners.size());
    std::transform(
        input_corners.begin(),
        input_corners.end(),
        output_corners->begin(),
        [&matrix](const point_t& _in)
        {
            // Convert input to homogeneous coordinates.
            const itk::Point<double, 4> input(std::array<double, 4>({{_in[0], _in[1], _in[2], 1.}}).data());
            const auto p = matrix * input;
            return point_t(p.GetDataPointer());
        });

    // Compute the transformed axis aligned bounding box.
    typename bounding_box_t::Pointer output_bb = bounding_box_t::New();
    output_bb->SetPoints(output_corners);
    output_bb->ComputeBoundingBox();

    // Compute output size and origin.
    data::image::sptr output = std::make_shared<data::image>();
    data::image::origin_t output_origin;
    data::image::size_t output_size;

    for(std::uint8_t i = 0 ; i < 3 ; ++i)
    {
        output_origin[i] = output_bb->GetMinimum()[i];
        output_size[i]   = std::size_t((output_bb->GetMaximum()[i] - output_origin[i]) / _output_spacing[i]);
    }

    output->set_spacing(_output_spacing);
    output->set_origin(output_origin);

    resample(_img, output, _trf, std::make_tuple(output_size, output_origin, _output_spacing));

    return output;
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::image
