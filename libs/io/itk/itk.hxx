/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

// cspell:ignore NOLINTNEXTLINE

#pragma once

#include <data/helper/MedicalImage.hpp>

#include <itkImage.h>
#include <itkImageRegion.h>

namespace sight::io::itk
{

//------------------------------------------------------------------------------

template<class ITKIMAGE>
void moveFromItk(
    typename ITKIMAGE::Pointer _itkImage,
    data::image::sptr _dataImage,
    bool _bufferManagerIsDataImage
)
{
    SIGHT_ASSERT("_dataImage not instanced", _dataImage);

    // Add by arnaud
    std::uint8_t dim              = ITKIMAGE::ImageDimension;
    data::image::Spacing vSpacing = {0., 0., 0.};
    data::image::Origin vOrigin   = {0., 0., 0.};
    data::image::Size vSize       = {0, 0, 0};

    for(std::uint8_t d = 0 ; d < dim ; ++d)
    {
        vOrigin[d]  = _itkImage->GetOrigin()[d];
        vSize[d]    = _itkImage->GetBufferedRegion().GetSize()[d];
        vSpacing[d] = _itkImage->GetSpacing()[d];
    }

    _dataImage->setOrigin(vOrigin);
    _dataImage->setSpacing(vSpacing);

    const auto pixelType = core::type::get<typename ITKIMAGE::PixelType>();
    const auto dumpLock  = _dataImage->dump_lock();
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
            data::image::GRAY_SCALE,
            std::make_shared<core::memory::buffer_new_policy>()
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
            data::image::GRAY_SCALE
        );
    }

    if(sight::data::helper::MedicalImage::checkImageValidity(_dataImage))
    {
        sight::data::helper::MedicalImage::checkImageSliceIndex(_dataImage);
    }

    // Post Condition correct PixelType
    SIGHT_ASSERT("Sorry, pixel type is not correct", _dataImage->getType() != core::type::NONE);
}

//------------------------------------------------------------------------------

template<class ITKIMAGE>
data::image::sptr moveFromItk(typename ITKIMAGE::Pointer itkImage, bool bufferManagerIsDataImage)
{
    data::image::sptr data = std::make_shared<data::image>();
    io::itk::moveFromItk<ITKIMAGE>(itkImage, data, bufferManagerIsDataImage);
    return data;
}

//------------------------------------------------------------------------------

template<class ITKIMAGE_PTR>
void moveFromItk(ITKIMAGE_PTR itkImage, data::image::sptr _dataImage)
{
    moveFromItk<typename ITKIMAGE_PTR::ObjectType>(itkImage, _dataImage);
}

//------------------------------------------------------------------------------

template<class ITKIMAGE>
typename ITKIMAGE::Pointer moveToItk(data::image::csptr imageData)
{
    // Pre Condition
    SIGHT_ASSERT(
        "Sorry, itk image dimension not correspond to fwData image",
        imageData->numDimensions() == ITKIMAGE::ImageDimension
    );

    const auto dumpLock = imageData->dump_lock();

    typename ITKIMAGE::Pointer itkImage = ITKIMAGE::New();

    // update spacing information ; workaround due to GetSpacing const
    typename ITKIMAGE::SpacingType spacing = itkImage->GetSpacing();
    for(std::uint8_t d = 0 ; d < ITKIMAGE::ImageDimension ; ++d)
    {
        spacing[d] = imageData->getSpacing()[d];
    }

    itkImage->SetSpacing(spacing);

    // update origin information
    itkImage->SetOrigin(imageData->getOrigin().data());

    ::itk::ImageRegion<ITKIMAGE::ImageDimension> itkRegion;

    std::uint64_t nb_pixels = 1;
    for(std::uint8_t d = 0 ; d < ITKIMAGE::ImageDimension ; ++d)
    {
        // itkRegion.SetIndex( d,  static_cast<int>(imageData->getOrigin()[d]) );
        itkRegion.SetSize(d, static_cast<std::uint64_t>(imageData->size()[d]));
        nb_pixels *= static_cast<std::uint64_t>(itkRegion.GetSize()[d]);
    }

    itkImage->SetRegions(itkRegion);

    itkImage->GetPixelContainer()->SetImportPointer(
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        static_cast<typename ITKIMAGE::PixelType*>(const_cast<void*>(imageData->buffer())),
        nb_pixels,
        false
    );

    return itkImage;
}

//------------------------------------------------------------------------------

} // namespace sight::io::itk
