/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "imageFilterOp/Flipper.hpp"

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwItkIO/itk.hpp>

#include <fwTools/Dispatcher.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>
#include <fwTools/IntrinsicTypes.hpp>

#include <itkFlipImageFilter.h>

namespace imageFilterOp
{

struct Flipping
{
    struct Parameters
    {
        ::fwData::Image::csptr i_image;
        std::array<bool, 3> i_flipAxes;
        ::fwData::Image::sptr o_image;
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(Parameters& params)
    {
        typedef typename ::itk::Image< PIXELTYPE, 3 > ImageType;
        const typename ImageType::Pointer itkImage = ::fwItkIO::itkImageFactory< ImageType >(params.i_image);

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
        ::fwItkIO::itkImageToFwDataImage(outputImage, params.o_image);
    }
};

//-----------------------------------------------------------------------------

void Flipper::flip(const ::fwData::Image::csptr& _inImage,
                   const ::fwData::Image::sptr& _outImage,
                   const std::array<bool, 3>& _inFlipAxes)
{
    const ::fwData::Image::SizeType SIZE = {{ 0, 0, 0 }};
    if(_inImage->getSize() != SIZE)
    {
        Flipping::Parameters params;
        params.i_image    = _inImage;
        params.i_flipAxes = _inFlipAxes;
        params.o_image    = _outImage;

        const ::fwTools::DynamicType type = _inImage->getPixelType();
        ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, Flipping >::invoke(type, params);
    }

    else
    {
        ::fwData::mt::ObjectReadLock readBlock(_inImage);
        ::fwData::mt::ObjectWriteLock writeBlock(_outImage);
        _outImage->deepCopy(_inImage);
    }
}

//-----------------------------------------------------------------------------

}// namespace imageFilterOp
