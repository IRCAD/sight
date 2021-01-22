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

namespace fwItkIO
{

//------------------------------------------------------------------------------

template< class ITKIMAGE>
void dataImageFactory( typename ITKIMAGE::Pointer _itkImage, ::fwData::Image::sptr _dataImage,
                       bool _bufferManagerIsDataImage )
{
    SLM_ASSERT("_dataImage not instanced", _dataImage);

    // Add by arnaud
    std::uint8_t dim = ITKIMAGE::ImageDimension;
    ::fwData::Image::Spacing vSpacing = {0., 0., 0.};
    ::fwData::Image::Origin vOrigin   = {0., 0., 0.};
    ::fwData::Image::Size vSize       = {0, 0, 0};

    for (std::uint8_t d = 0; d < dim; ++d)
    {
        vOrigin[d]  = _itkImage->GetOrigin()[d];
        vSize[d]    = _itkImage->GetBufferedRegion().GetSize()[d];
        vSpacing[d] = _itkImage->GetSpacing()[d];
    }

    _dataImage->setSize2( vSize );
    _dataImage->setOrigin2( vOrigin );
    _dataImage->setSpacing2( vSpacing );

    typedef typename ITKIMAGE::PixelType PixelType;
    _dataImage->setType( core::tools::Type::create<PixelType>() );
    _dataImage->setPixelFormat(::fwData::Image::GRAY_SCALE);

    const auto dumpLock = _dataImage->lock();
    if( _bufferManagerIsDataImage )
    {
        SLM_ASSERT("Sorry, this method requires that itkImage manages its buffer.",
                   _itkImage->GetPixelContainer()->GetContainerManageMemory() );
        _dataImage->setBuffer(
            static_cast<void*>(_itkImage->GetBufferPointer()), true, _dataImage->getType(), vSize,
            core::memory::BufferNewPolicy::New());
        /// itk image release its management buffer. dataImage must now deal memory
        _itkImage->GetPixelContainer()->SetContainerManageMemory( false );
    }
    else
    {
        _dataImage->setBuffer( static_cast<void*>(_itkImage->GetBufferPointer()), false,
                               _dataImage->getType(), vSize);
    }

    // Post Condition correct PixelType
    SLM_ASSERT("Sorry, pixel type is not correct", _dataImage->getType() != core::tools::Type::s_UNSPECIFIED_TYPE );
}

//------------------------------------------------------------------------------

template< class ITKIMAGE>
::fwData::Image::sptr dataImageFactory( typename ITKIMAGE::Pointer itkImage, bool bufferManagerIsDataImage )
{
    ::fwData::Image::sptr data = ::fwData::Image::New();
    ::fwItkIO::dataImageFactory< ITKIMAGE >(itkImage, data, bufferManagerIsDataImage);
    return data;
}

//------------------------------------------------------------------------------

template< class ITKIMAGE_PTR >
void itkImageToFwDataImage( ITKIMAGE_PTR itkImage, ::fwData::Image::sptr _dataImage )
{
    dataImageFactory< typename ITKIMAGE_PTR::ObjectType >(itkImage, _dataImage);
}

//------------------------------------------------------------------------------

template< class ITKIMAGE>
typename ITKIMAGE::Pointer fwDataImageToItkImage( ::fwData::Image::csptr imageData, bool bufferManagerIsDataImage )
{
    // Pre Condition
    SLM_ASSERT("Sorry, itk image dimension not correspond to fwData image",
               imageData->getNumberOfDimensions() == ITKIMAGE::ImageDimension );

    const auto dumpLock = imageData->lock();

    typename ITKIMAGE::Pointer itkImage = ITKIMAGE::New();

    // update spacing information ; workaround due to GetSpacing const
    typename ITKIMAGE::SpacingType spacing = itkImage->GetSpacing();
    for (std::uint8_t d = 0; d < ITKIMAGE::ImageDimension; ++d)
    {
        spacing[d] = imageData->getSpacing2()[d];
    }
    itkImage->SetSpacing(spacing);

    // update origin information ; workaround due to GetOrigin const
    std::copy(   imageData->getOrigin2().begin(),
                 imageData->getOrigin2().end(),
                 const_cast< typename ITKIMAGE::PointType* >( &itkImage->GetOrigin())->Begin()
                 );

    itk::ImageRegion< ITKIMAGE::ImageDimension > itkRegion;

    unsigned long nbpixels = 1;
    for (std::uint8_t d = 0; d < ITKIMAGE::ImageDimension; ++d)
    {
        // itkRegion.SetIndex( d,  static_cast<int>(imageData->getOrigin()[d]) );
        itkRegion.SetSize( d,   static_cast<unsigned long>(imageData->getSize2()[d]) );
        nbpixels *= itkRegion.GetSize()[d];
    }

    itkImage->SetRegions(itkRegion);

    if( bufferManagerIsDataImage )
    {
        itkImage->GetPixelContainer()->SetImportPointer(
            static_cast< typename ITKIMAGE::PixelType*>( imageData->getBuffer() ), nbpixels, false );
    }
    else
    {
        FW_DEPRECATED_MSG("Image should no longer change buffer ownership", "22.0");
        SLM_ASSERT("Sorry, this method requires that imageData manages its buffer.",
                   imageData->getDataArray()->getIsBufferOwner() );
        itkImage->GetPixelContainer()->SetImportPointer(
            static_cast< typename ITKIMAGE::PixelType*>( imageData->getBuffer() ), nbpixels, true );
        imageData->getDataArray()->setIsBufferOwner( false );
    }

    return itkImage;
}

//------------------------------------------------------------------------------

template< class ITKIMAGE>
typename ITKIMAGE::Pointer itkImageFactory( ::fwData::Image::csptr imageData, bool bufferManagerIsDataImage )
{
    return fwDataImageToItkImage<ITKIMAGE>( imageData, bufferManagerIsDataImage );
}

//------------------------------------------------------------------------------

} // end namespace
