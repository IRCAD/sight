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
void dataImageFactory( typename ITKIMAGE::Pointer itkImage , ::fwData::Image::sptr _dataImage )
{
    SLM_ASSERT("_dataImage not instanced", _dataImage);
    assert( _dataImage->getDimension() == ITKIMAGE::ImageDimension ); // ::fwData::Image Dimension hard coded to 3 !!!!!!

    // Add by arnaud
    ::boost::uint8_t  dim = ITKIMAGE::ImageDimension;
    std::vector<double> _vSpacing(dim,1);
    std::vector<double> _vOrigin(dim,0);
    std::vector< ::boost::int32_t >  _vSize(dim,0);

    for (boost::uint8_t  d=0; d<dim; ++d)
    {
        // _vOrigin[d] = itkImage->GetBufferedRegion().GetIndex()[d];
        _vOrigin[d] = itkImage->GetOrigin()[d];
        _vSize[d] = itkImage->GetBufferedRegion().GetSize()[d];
        _vSpacing[d] = itkImage->GetSpacing()[d];
    }

    _dataImage->setCRefSize( _vSize );
    _dataImage->setCRefOrigin( _vOrigin );
    _dataImage->setCRefSpacing( _vSpacing );

    // Remove by Arnaud
    //for (boost::uint8_t  d=0; d<ITKIMAGE::ImageDimension ; ++d)
    //{
    //  data->getRegion().getOrigin()[d]    = itkImage->GetOrigin()[d];// float
    //  data->getSpacing()[d]               = itkImage->GetSpacing()[d]; // float
    //  data->getRegion().getOrigin()[d]    = itkImage->GetBufferedRegion().GetIndex()[d];
    //  data->getRegion().getSize()[d]      = itkImage->GetBufferedRegion().GetSize()[d];
    //}

    typedef typename ITKIMAGE::PixelType PixelType;
    _dataImage->setPixelType( fwTools::makeDynamicType<PixelType>() );
    _dataImage->setBuffer( static_cast<void *>(itkImage->GetBufferPointer()) );

    /// itk image release its management buffer. dataImage must now deal memory
    itkImage->GetPixelContainer()->SetContainerManageMemory( false );

    // Post Condition correct PixelType
    assert( _dataImage->getPixelType()!= fwTools::DynamicType() );
}

//------------------------------------------------------------------------------

template< class ITKIMAGE>
::fwData::Image::sptr dataImageFactory( typename ITKIMAGE::Pointer itkImage )
{
    ::fwData::Image::NewSptr data;
    ::itkIO::dataImageFactory< ITKIMAGE >(itkImage,data) ;
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
typename ITKIMAGE::Pointer fwDataImageToItkImage( ::fwData::Image::sptr imageData)
{
    // Pre Condition
    assert( imageData->getDimension() == ITKIMAGE::ImageDimension );

    typename ITKIMAGE::Pointer itkImage=ITKIMAGE::New();

    // update spacing information ; workaround due to GetSpacing const
    typename ITKIMAGE::SpacingType spacing = itkImage->GetSpacing();
    for (boost::uint8_t  d=0; d<ITKIMAGE::ImageDimension; ++d)
    {
        spacing[d] = imageData->getRefSpacing()[d];
    }
    itkImage->SetSpacing(spacing);

    // update origin information ; workaround due to GetOrigin const
    std::copy(   imageData->getRefOrigin().begin(),
                 imageData->getRefOrigin().end(),
                 const_cast< typename ITKIMAGE::PointType * >( &itkImage->GetOrigin())->Begin()
             );

    itk::ImageRegion< ITKIMAGE::ImageDimension > itkRegion;

    unsigned long nbpixels=1;
    for (::boost::uint8_t  d=0; d<ITKIMAGE::ImageDimension; ++d)
    {
        // itkRegion.SetIndex( d,  static_cast<int>(imageData->getCRefOrigin()[d]) );
        itkRegion.SetSize( d,   imageData->getCRefSize()[d] );
        nbpixels *= itkRegion.GetSize()[d];;
    }

    itkImage->SetRegions(itkRegion);

    itkImage->GetPixelContainer()->SetImportPointer(static_cast< typename ITKIMAGE::PixelType *>( imageData->getBuffer() ) , nbpixels , false );

    return itkImage;
}

//------------------------------------------------------------------------------

template< class ITKIMAGE>
typename ITKIMAGE::Pointer itkImageFactory( ::fwData::Image::sptr imageData)
{
    return fwDataImageToItkImage<ITKIMAGE>( imageData );
}

//------------------------------------------------------------------------------

} // end namespace


