/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
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

#include "Resampler.hpp"

#include <core/tools/Dispatcher.hpp>
#include <core/tools/TypeKeyTypeMapping.hpp>

#include <io/itk/helper/Transform.hpp>
#include <io/itk/itk.hpp>

#include <itkAffineTransform.h>
#include <itkBoundingBox.h>
#include <itkBSplineInterpolateImageFunction.h>
#include <itkMatrix.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkResampleImageFilter.h>

namespace sight::filter::image
{

struct Resampling
{
    struct Parameters
    {
        itk::AffineTransform<double, 3>::Pointer i_trf;
        data::Image::csptr i_image;
        data::Image::sptr o_image;
        std::optional<std::tuple<data::Image::Size,
                                 data::Image::Origin,
                                 data::Image::Spacing> > i_parameters;
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(Parameters& params)
    {
        typedef typename itk::Image<PIXELTYPE, 3> ImageType;
        const typename ImageType::Pointer itkImage = io::itk::moveToItk<ImageType>(params.i_image);

        typename itk::ResampleImageFilter<ImageType, ImageType>::Pointer resampler =
            itk::ResampleImageFilter<ImageType, ImageType>::New();

        typename itk::MinimumMaximumImageCalculator<ImageType>::Pointer minCalculator =
            itk::MinimumMaximumImageCalculator<ImageType>::New();

        minCalculator->SetImage(itkImage);
        minCalculator->ComputeMinimum();
        resampler->SetDefaultPixelValue(minCalculator->GetMinimum());

        resampler->SetTransform(params.i_trf.GetPointer());
        resampler->SetInput(itkImage);

        typename ImageType::SizeType size           = itkImage->GetLargestPossibleRegion().GetSize();
        typename ImageType::PointType origin        = itkImage->GetOrigin();
        typename ImageType::SpacingType spacing     = itkImage->GetSpacing();
        typename ImageType::DirectionType direction = itkImage->GetDirection();

        SIGHT_ASSERT("Input spacing can't be null along any axis", spacing[0] > 0 && spacing[1] > 0 && spacing[2] > 0);

        if(params.i_parameters.has_value())
        {
            auto& [outSize, outOrigin, outSpacing] = params.i_parameters.value();
            for(std::uint8_t i = 0 ; i < 3 ; ++i)
            {
                // ITK uses unsigned long to store sizes.
                size[i] = static_cast<typename ImageType::SizeType::SizeValueType>(outSize[i]);

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

        typename ImageType::Pointer outputImage = resampler->GetOutput();

        io::itk::moveFromItk(outputImage, params.o_image);
    }
};

//-----------------------------------------------------------------------------

void Resampler::resample(
    const data::Image::csptr& _inImage,
    const data::Image::sptr& _outImage,
    const data::Matrix4::csptr& _trf,
    std::optional<std::tuple<data::Image::Size,
                             data::Image::Origin,
                             data::Image::Spacing> > parameters
)
{
    const itk::Matrix<double, 4, 4> itkMatrix = io::itk::helper::Transform::convertToITK(_trf);

    // We need to extract a 3x3 matrix and a vector to set the affine transform.
    itk::Matrix<double, 3, 3> transformMat;

    for(std::uint8_t i = 0 ; i < 3 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 3 ; ++j)
        {
            transformMat(i, j) = itkMatrix(i, j);
        }
    }

    itk::Vector<double, 3> translation;
    for(std::uint8_t i = 0 ; i < 3 ; ++i)
    {
        // Extract the last column to get the translation.
        translation.SetElement(i, itkMatrix(i, 3));
    }

    itk::AffineTransform<double, 3>::Pointer transf = itk::AffineTransform<double, 3>::New();
    transf->SetMatrix(transformMat);
    transf->SetTranslation(translation);

    Resampling::Parameters params;
    params.i_image      = _inImage;
    params.o_image      = _outImage;
    params.i_trf        = transf.GetPointer();
    params.i_parameters = parameters;

    const core::tools::Type type = _inImage->getType();
    core::tools::Dispatcher<core::tools::SupportedDispatcherTypes, Resampling>::invoke(type, params);
}

//-----------------------------------------------------------------------------

data::Image::sptr Resampler::resample(
    const data::Image::csptr& _img,
    const data::Matrix4::csptr& _trf,
    const data::Image::Spacing& _outputSpacing
)
{
    using PointType           = itk::Point<double, 3>;
    using VectorContainerType = itk::VectorContainer<int, PointType>;
    using BoundingBoxType     = itk::BoundingBox<int, 3, double, VectorContainerType>;

    const auto& inputSize    = _img->getSize();
    const auto& inputOrigin  = _img->getOrigin();
    const auto& inputSpacing = _img->getSpacing();

    SIGHT_ASSERT(
        "Image dimension must be 3.",
        inputOrigin.size() == 3 && inputSpacing.size() == 3 && inputSize.size() == 3
    );

    typename BoundingBoxType::Pointer inputBB = BoundingBoxType::New();

    const PointType min(inputOrigin.data());
    PointType max;
    for(std::uint8_t i = 0 ; i < 3 ; ++i)
    {
        max[i] = inputOrigin[i] + static_cast<double>(inputSize[i]) * inputSpacing[i];
    }

    inputBB->SetMinimum(min);
    inputBB->SetMaximum(max);

    const auto inputCorners = inputBB->GetCorners();
    const itk::Matrix<double, 4, 4> matrix(io::itk::helper::Transform::convertToITK(_trf).GetInverse());

    // Apply transform matrix to all bounding box corners.
    typename VectorContainerType::Pointer outputCorners = VectorContainerType::New();
    outputCorners->Reserve(inputCorners->Size());
    std::transform(
        inputCorners->begin(),
        inputCorners->end(),
        outputCorners->begin(),
        [&matrix](const PointType& _in)
        {
            // Convert input to homogeneous coordinates.
            const itk::Point<double, 4> input(std::array<double, 4>({{_in[0], _in[1], _in[2], 1.}}).data());
            const auto p = matrix * input;
            return PointType(p.GetDataPointer());
        });

    // Compute the transformed axis aligned bounding box.
    typename BoundingBoxType::Pointer outputBB = BoundingBoxType::New();
    outputBB->SetPoints(outputCorners);
    outputBB->ComputeBoundingBox();

    // Compute output size and origin.
    data::Image::sptr output = data::Image::New();
    data::Image::Origin outputOrigin;
    data::Image::Size outputSize;

    for(std::uint8_t i = 0 ; i < 3 ; ++i)
    {
        outputOrigin[i] = outputBB->GetMinimum()[i];
        outputSize[i]   = size_t((outputBB->GetMaximum()[i] - outputOrigin[i]) / _outputSpacing[i]);
    }

    output->setSpacing(_outputSpacing);
    output->setOrigin(outputOrigin);

    resample(_img, output, _trf, std::make_tuple(outputSize, outputOrigin, _outputSpacing));

    return output;
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::image
