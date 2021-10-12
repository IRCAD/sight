/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#pragma once

#include <itkImage.h>
#include <itkImageRegion.h>

namespace sight::io::itk
{

//------------------------------------------------------------------------------

template<class ITKIMAGE>
void moveFromItk(
    typename ITKIMAGE::Pointer _itkImage,
    data::Image::sptr _dataImage,
    bool _bufferManagerIsDataImage
)
{
    SIGHT_ASSERT("_dataImage not instanced", _dataImage);

    // Add by arnaud
    std::uint8_t dim              = ITKIMAGE::ImageDimension;
    data::Image::Spacing vSpacing = {0., 0., 0.};
    data::Image::Origin vOrigin   = {0., 0., 0.};
    data::Image::Size vSize       = {0, 0, 0};

    for(std::uint8_t d = 0 ; d < dim ; ++d)
    {
        vOrigin[d]  = _itkImage->GetOrigin()[d];
        vSize[d]    = _itkImage->GetBufferedRegion().GetSize()[d];
        vSpacing[d] = _itkImage->GetSpacing()[d];
    }

    _dataImage->setOrigin(vOrigin);
    _dataImage->setSpacing(vSpacing);

    const auto pixelType = core::tools::Type::create<typename ITKIMAGE::PixelType>();
    const auto dumpLock  = _dataImage->lock();
    if(_bufferManagerIsDataImage)
    {
        SIGHT_ASSERT(
            "Sorry, this method requires that itkImage manages its buffer.",
            _itkImage->GetPixelContainer()->GetContainerManageMemory()
        );
        _dataImage->setBuffer(
            static_cast<void*>(_itkImage->GetBufferPointer()),
            true,
            pixelType,
            vSize,
            data::Image::GRAY_SCALE,
            core::memory::BufferNewPolicy::New()
        );
        /// itk image release its management buffer. dataImage must now deal memory
        _itkImage->GetPixelContainer()->SetContainerManageMemory(false);
    }
    else
    {
        _dataImage->setBuffer(
            static_cast<void*>(_itkImage->GetBufferPointer()),
            false,
            pixelType,
            vSize,
            data::Image::GRAY_SCALE
        );
    }

    // Post Condition correct PixelType
    SIGHT_ASSERT("Sorry, pixel type is not correct", _dataImage->getType() != core::tools::Type::s_UNSPECIFIED_TYPE);
}

//------------------------------------------------------------------------------

template<class ITKIMAGE>
data::Image::sptr moveFromItk(typename ITKIMAGE::Pointer itkImage, bool bufferManagerIsDataImage)
{
    data::Image::sptr data = data::Image::New();
    io::itk::moveFromItk<ITKIMAGE>(itkImage, data, bufferManagerIsDataImage);
    return data;
}

//------------------------------------------------------------------------------

template<class ITKIMAGE_PTR>
void moveFromItk(ITKIMAGE_PTR itkImage, data::Image::sptr _dataImage)
{
    moveFromItk<typename ITKIMAGE_PTR::ObjectType>(itkImage, _dataImage);
}

//------------------------------------------------------------------------------

template<class ITKIMAGE>
typename ITKIMAGE::Pointer moveToItk(data::Image::csptr imageData)
{
    // Pre Condition
    SIGHT_ASSERT(
        "Sorry, itk image dimension not correspond to fwData image",
        imageData->numDimensions() == ITKIMAGE::ImageDimension
    );

    const auto dumpLock = imageData->lock();

    typename ITKIMAGE::Pointer itkImage = ITKIMAGE::New();

    // update spacing information ; workaround due to GetSpacing const
    typename ITKIMAGE::SpacingType spacing = itkImage->GetSpacing();
    for(std::uint8_t d = 0 ; d < ITKIMAGE::ImageDimension ; ++d)
    {
        spacing[d] = imageData->getSpacing()[d];
    }

    itkImage->SetSpacing(spacing);

    // update origin information ; workaround due to GetOrigin const
    std::copy(
        imageData->getOrigin().begin(),
        imageData->getOrigin().end(),
        const_cast<typename ITKIMAGE::PointType*>(&itkImage->GetOrigin())->Begin()
    );

    ::itk::ImageRegion<ITKIMAGE::ImageDimension> itkRegion;

    unsigned long nbpixels = 1;
    for(std::uint8_t d = 0 ; d < ITKIMAGE::ImageDimension ; ++d)
    {
        // itkRegion.SetIndex( d,  static_cast<int>(imageData->getOrigin()[d]) );
        itkRegion.SetSize(d, static_cast<unsigned long>(imageData->getSize()[d]));
        nbpixels *= static_cast<unsigned long>(itkRegion.GetSize()[d]);
    }

    itkImage->SetRegions(itkRegion);

    itkImage->GetPixelContainer()->SetImportPointer(
        static_cast<typename ITKIMAGE::PixelType*>(imageData->getBuffer()),
        nbpixels,
        false
    );

    return itkImage;
}

//------------------------------------------------------------------------------

} // end namespace
