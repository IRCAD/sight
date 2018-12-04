/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWITKIO_ITKPP_HXX__
#define __FWITKIO_ITKPP_HXX__

#include "fwItkIO/itk.hpp"

#include <fwTools/DynamicType.hpp>

#include <itkCastImageFilter.h>

namespace fwItkIO
{

//------------------------------------------------------------------------------

template< class PIXELTYPE, int VDimension, class ScannerType >
typename ScannerType::Pointer  itkppScannerFactory( ::fwData::Image::sptr imageData)
{
    typedef itk::Image< PIXELTYPE, VDimension >    ImageType;

    typename ImageType::Pointer itkRoi = ::fwItkIO::itkImageFactory< ImageType >( imageData );
    typedef itk::Image< unsigned char, VDimension  >   ROIType;
    typedef itk::CastImageFilter< ImageType, ROIType  >        CasterType;
    typename CasterType::Pointer caster = CasterType::New();
    caster->SetInput( itkRoi );
    caster->Update();

    typename ScannerType::Pointer scanner = ScannerType::New();
    typename ROIType::Pointer outImg      = caster->GetOutput();
    scanner->SetMaskImage( outImg );
    outImg->DisconnectPipeline();

    return scanner;
}

} // end namespace

#endif // __FWITKIO_ITKPP_HXX__
