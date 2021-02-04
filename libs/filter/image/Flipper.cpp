/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "filter/image/Flipper.hpp"

#include <core/tools/Dispatcher.hpp>
#include <core/tools/TypeKeyTypeMapping.hpp>

#include <data/tools/fieldHelper/MedicalImageHelpers.hpp>

#include <io/itk/itk.hpp>

#include <itkFlipImageFilter.h>

namespace sight::filter::image
{

struct Parameters
{
    data::Image::csptr i_image;
    std::array<bool, 3> i_flipAxes;
    data::Image::sptr o_image;
};

//------------------------------------------------------------------------------

template<typename PixelType, int dimension>
struct Flipping
{
    //------------------------------------------------------------------------------

    void operator()(Parameters& params)
    {
        typedef typename ::itk::Image< PixelType, dimension> ImageType;
        const typename ImageType::Pointer itkImage = io::itk::itkImageFactory< ImageType >(params.i_image);

        typename ::itk::FlipImageFilter<ImageType>::Pointer flipFilter =
            ::itk::FlipImageFilter<ImageType>::New();

        flipFilter->SetInput(itkImage);
        typename ::itk::FlipImageFilter<ImageType>::FlipAxesArrayType axes;
        for(size_t i = 0; i < axes.Size() && i < params.i_flipAxes.size(); i++)
        {
            axes[i] = params.i_flipAxes[i];
        }

        flipFilter->SetFlipAxes(axes);
        flipFilter->Update();

        typename ImageType::Pointer outputImage = flipFilter->GetOutput();
        io::itk::itkImageToFwDataImage(outputImage, params.o_image);
    }
};

struct FlippingDimensionExtractor
{
    //------------------------------------------------------------------------------

    template<class PixelType>
    void operator()(Parameters& params)
    {
        const data::Image::Size size = params.i_image->getSize2();
        switch(size.size())
        {
            case 1:
                Flipping<PixelType, 1> d1;
                d1(params);
                break;
            case 2:
                Flipping<PixelType, 2> d2;
                d2(params);
                break;
            case 3:
                Flipping<PixelType, 3> d3;
                d3(params);
                break;
            default:
                SLM_ERROR("Flipping cannot be performed due to incompatible image size ("
                          + std::to_string(size.size()) + ").");
                // In this case, we just deep copy the input image in the output
                params.o_image->deepCopy(params.i_image);
                break;
        }
    }
};

//-----------------------------------------------------------------------------

void Flipper::flip(const data::Image::csptr& _inImage,
                   const data::Image::sptr& _outImage,
                   const std::array<bool, 3>& _inFlipAxes)
{
    // If the image is valid, process it, otherwise copy it in the output image
    if(data::tools::fieldHelper::MedicalImageHelpers::checkImageValidity(_inImage))
    {
        Parameters params;
        params.i_image    = _inImage;
        params.i_flipAxes = _inFlipAxes;
        params.o_image    = _outImage;

        const core::tools::Type type = _inImage->getType();
        core::tools::Dispatcher< core::tools::SupportedDispatcherTypes, FlippingDimensionExtractor >::invoke(type,
                                                                                                             params);
    }
    else
    {
        _outImage->deepCopy(_inImage);
    }
}

//-----------------------------------------------------------------------------

}// namespace sight::filter::image
