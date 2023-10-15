/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "bitwise_and.hpp"

#include <core/com/signal.hxx>
#include <core/tools/dispatcher.hpp>

#include <io/itk/itk.hpp>

#include <itkAndImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>

namespace sight::module::filter::image
{

struct AndImageFilterParameters
{
    data::image::csptr inputImage;
    data::image::csptr mask;
    data::image::sptr outputImage;
};

//------------------------------------------------------------------------------

template<typename PIXELTYPE>
struct AndImageFilter
{
    //------------------------------------------------------------------------------

    template<class MASK_PIXELTYPE>
    void operator()(AndImageFilterParameters& params)
    {
        data::image::csptr inputImage = params.inputImage;
        data::image::csptr mask       = params.mask;
        data::image::sptr outputImage = params.outputImage;

        const unsigned int dimension = 3;
        SIGHT_ASSERT("Only image dimension 3 managed.", inputImage->numDimensions() == dimension);

        using InputImageType  = typename itk::Image<PIXELTYPE, dimension>;
        using MaskImageType   = typename itk::Image<MASK_PIXELTYPE, dimension>;
        using OutputImageType = typename itk::Image<PIXELTYPE, dimension>;

        typename InputImageType::Pointer itkInputImage = io::itk::moveToItk<InputImageType>(inputImage);
        typename MaskImageType::Pointer itkMaskImage   = io::itk::moveToItk<MaskImageType>(mask);
        typename OutputImageType::Pointer itkOutputImage;

        // We assume that the mask pixel type has a lower size in bits than the image pixel type
        // Cast mask pixel type to the image pixel type
        using FilterType = itk::CastImageFilter<MaskImageType, InputImageType>;
        typename FilterType::Pointer caster = FilterType::New();
        caster->SetInput(itkMaskImage);

        // Rescale the image so that the output range of the casted mask image is in the same range as the input image.
        using RescaleType = itk::RescaleIntensityImageFilter<InputImageType, InputImageType>;
        typename RescaleType::Pointer rescaler = RescaleType::New();
        rescaler->SetInput(caster->GetOutput());
        rescaler->SetOutputMinimum(0);
        rescaler->SetOutputMaximum(std::numeric_limits<PIXELTYPE>::max());
        rescaler->Update();

        typename InputImageType::Pointer itkMaskImageCasted = rescaler->GetOutput();

        using ITKFilterType = typename itk::AndImageFilter<InputImageType, InputImageType, OutputImageType>;
        typename ITKFilterType::Pointer filter = ITKFilterType::New();
        filter->SetInput1(itkInputImage);
        filter->SetInput2(itkMaskImageCasted);
        itkOutputImage = filter->GetOutput();
        filter->Update();

        itkOutputImage->GetSource()->Update();
        io::itk::moveFromItk<OutputImageType>(itkOutputImage, outputImage);
    }
};

//------------------------------------------------------------------------------

struct AndImageFilterCaller
{
    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(AndImageFilterParameters& params)
    {
        const auto maskType = params.mask->getType();
        core::tools::dispatcher<core::tools::integer_types, AndImageFilter<PIXELTYPE> >::invoke(maskType, params);
    }
};

//-----------------------------------------------------------------------------

bitwise_and::bitwise_and()
= default;

//-----------------------------------------------------------------------------

bitwise_and::~bitwise_and()
= default;

//-----------------------------------------------------------------------------

void bitwise_and::configuring()
{
}

//-----------------------------------------------------------------------------

void bitwise_and::starting()
{
}

//-----------------------------------------------------------------------------

void bitwise_and::updating()
{
    const auto image = m_image.lock();
    SIGHT_ASSERT("image does not exist.", image);

    const auto mask = m_mask.lock();
    SIGHT_ASSERT("mask does not exist.", mask);

    data::image::sptr outputImage = std::make_shared<data::image>();

    AndImageFilterParameters params;
    params.inputImage  = image.get_shared();
    params.mask        = mask.get_shared();
    params.outputImage = outputImage;

    const auto type = image->getType();
    core::tools::dispatcher<core::tools::integer_types, AndImageFilterCaller>::invoke(type, params);

    this->set_output(s_OUTPUTIMAGE_OUT, outputImage);

    m_sigComputed->async_emit();
}

//-----------------------------------------------------------------------------

void bitwise_and::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::image.
