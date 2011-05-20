/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/DynamicType.hpp>
#include <fwTools/Factory.hpp>

#include <assert.h>

#include <itkCastImageFilter.h>

#include "itkIO/itk.hpp"

namespace itkIO
{

template< class PIXELTYPE , int VDimension , class ScannerType >
typename ScannerType::Pointer  itkppScannerFactory( ::fwData::Image::sptr imageData)
{
    typedef itk::Image< PIXELTYPE , VDimension >    ImageType;

    typename ImageType::Pointer itkRoi = ::itkIO::itkImageFactory< ImageType >( imageData ) ;
    typedef itk::Image< unsigned char , VDimension  >   ROIType;
    typedef itk::CastImageFilter< ImageType , ROIType  >        CasterType;
    typename CasterType::Pointer caster = CasterType::New() ;
    caster->SetInput( itkRoi ) ;
    caster->Update() ;

    typename ScannerType::Pointer scanner = ScannerType::New();
    typename ROIType::Pointer outImg = caster->GetOutput();
    scanner->SetMaskImage( outImg );
    outImg->DisconnectPipeline();

    return scanner ;
}


} // end namespace


