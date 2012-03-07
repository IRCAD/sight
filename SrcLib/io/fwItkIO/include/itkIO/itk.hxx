/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <assert.h>

#include <itkImage.h>
#include <itkImageRegion.h>

#include <fwTools/DynamicType.hpp>
#include <fwTools/Factory.hpp>

namespace itkIO
{

template< class ITKIMAGE>
void dataImageFactory( typename ITKIMAGE::Pointer itkImage , ::fwData::Image::sptr _dataImage, bool bufferManagerIsDataImage )
{
    SLM_ASSERT("_dataImage not instanced", _dataImage);

    // Add by arnaud
    ::boost::uint8_t  dim = ITKIMAGE::ImageDimension;
    ::fwData::Image::SpacingType _vSpacing(dim,1);
    ::fwData::Image::OriginType _vOrigin(dim,0);
    ::fwData::Image::SizeType  _vSize(dim,0);

    for (boost::uint8_t  d=0; d<dim; ++d)
    {
        // _vOrigin[d] = itkImage->GetBufferedRegion().GetIndex()[d];
        _vOrigin[d] = itkImage->GetOrigin()[d];
        _vSize[d] = itkImage->GetBufferedRegion().GetSize()[d];
        _vSpacing[d] = itkImage->GetSpacing()[d];
    }

    _dataImage->setSize( _vSize );
    _dataImage->setOrigin( _vOrigin );
    _dataImage->setSpacing( _vSpacing );

    typedef typename ITKIMAGE::PixelType PixelType;
    _dataImage->setType( ::fwTools::Type::create<PixelType>() );
    ::fwData::Array::NewSptr array;
    if( bufferManagerIsDataImage )
    {
        SLM_ASSERT("Sorry, this method requires that itkImage manages its buffer.",  itkImage->GetPixelContainer()->GetContainerManageMemory() );
        array->setBuffer( static_cast<void *>(itkImage->GetBufferPointer()), true, _dataImage->getType(), _vSize, 1 );
        /// itk image release its management buffer. dataImage must now deal memory
        itkImage->GetPixelContainer()->SetContainerManageMemory( false );
    }
    else
    {
        array->setBuffer( static_cast<void *>(itkImage->GetBufferPointer()), false, _dataImage->getType(), _vSize, 1 );
    }
    _dataImage->setDataArray( array );


    // Post Condition correct PixelType
    SLM_ASSERT("Sorry, pixel type is not correct", _dataImage->getType()!= fwTools::Type() );
}

//------------------------------------------------------------------------------

template< class ITKIMAGE>
::fwData::Image::sptr dataImageFactory( typename ITKIMAGE::Pointer itkImage, bool bufferManagerIsDataImage )
{
    ::fwData::Image::NewSptr data;
    ::itkIO::dataImageFactory< ITKIMAGE >(itkImage,data, bufferManagerIsDataImage) ;
    return data;
}

//------------------------------------------------------------------------------

template< class ITKIMAGE_PTR >
void itkImageToFwDataImage( ITKIMAGE_PTR itkImage , ::fwData::Image::sptr _dataImage )
{
    dataImageFactory< typename ITKIMAGE_PTR::ObjectType >(itkImage, _dataImage);
}

//------------------------------------------------------------------------------

template< class ITKIMAGE>
typename ITKIMAGE::Pointer fwDataImageToItkImage( ::fwData::Image::sptr imageData, bool bufferManagerIsDataImage )
{
    // Pre Condition
    SLM_ASSERT("Sorry, itk image dimension not correspond to fwData image", imageData->getNumberOfDimensions() == ITKIMAGE::ImageDimension );

    typename ITKIMAGE::Pointer itkImage=ITKIMAGE::New();

    // update spacing information ; workaround due to GetSpacing const
    typename ITKIMAGE::SpacingType spacing = itkImage->GetSpacing();
    for (boost::uint8_t  d=0; d<ITKIMAGE::ImageDimension; ++d)
    {
        spacing[d] = imageData->getSpacing()[d];
    }
    itkImage->SetSpacing(spacing);

    // update origin information ; workaround due to GetOrigin const
    std::copy(   imageData->getOrigin().begin(),
                 imageData->getOrigin().end(),
                 const_cast< typename ITKIMAGE::PointType * >( &itkImage->GetOrigin())->Begin()
             );

    itk::ImageRegion< ITKIMAGE::ImageDimension > itkRegion;

    unsigned long nbpixels=1;
    for (::boost::uint8_t  d=0; d<ITKIMAGE::ImageDimension; ++d)
    {
        // itkRegion.SetIndex( d,  static_cast<int>(imageData->getCRefOrigin()[d]) );
        itkRegion.SetSize( d,   static_cast<unsigned long>(imageData->getSize()[d]) );
        nbpixels *= itkRegion.GetSize()[d];;
    }

    itkImage->SetRegions(itkRegion);

    if( bufferManagerIsDataImage )
    {
        itkImage->GetPixelContainer()->SetImportPointer(static_cast< typename ITKIMAGE::PixelType *>( imageData->getBuffer() ) , nbpixels , false );
    }
    else
    {
        SLM_ASSERT("Sorry, this method requires that imageData manages its buffer.",   imageData->getDataArray()->getIsBufferOwner() );
        itkImage->GetPixelContainer()->SetImportPointer(static_cast< typename ITKIMAGE::PixelType *>( imageData->getBuffer() ) , nbpixels , true );
        imageData->getDataArray()->setIsBufferOwner( false );
    }

    return itkImage;
}

//------------------------------------------------------------------------------

template< class ITKIMAGE>
typename ITKIMAGE::Pointer itkImageFactory( ::fwData::Image::sptr imageData, bool bufferManagerIsDataImage )
{
    return fwDataImageToItkImage<ITKIMAGE>( imageData, bufferManagerIsDataImage );
}

//------------------------------------------------------------------------------

} // end namespace


