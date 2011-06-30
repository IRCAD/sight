/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWITKIO_ITKPP_HPP_
#define _FWITKIO_ITKPP_HPP_

#include <boost/shared_ptr.hpp>

#include <itkpp/MaskScanner.h>
#include <itkpp/ImageWithScanner.h>
#include <itkpp/ImageToImageFilterForImageWithScanner.h>
#include <itkpp/ImageWithScannerIteratorCollection.h>

#include <fwData/Image.hpp>

#include "itkIO/config.hpp"

namespace itkIO
{


/*
 * Create an image with scanner
 * ITKIMAGE template parameter must be instantiated with itk::Image< PIXELTYPE , VDIM > and not with itk::ImageWithScanner< PIXELTYPE , VDIM >,
 * otherwise the cast filter will wait for
*/
template< class PIXELTYPE , int VDimension , class ScannerType >
typename ScannerType::Pointer  itkppScannerFactory( ::fwData::Image::sptr imageData);

}

#include "itkIO/itkpp.hxx"

#endif // _FWITKIO_ITKPP_HPP_
