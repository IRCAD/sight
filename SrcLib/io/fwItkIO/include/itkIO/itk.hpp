/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWITKIO_ITK_HPP_
#define _FWITKIO_ITK_HPP_

#include <boost/shared_ptr.hpp>

#include <fwData/Image.hpp>

#include "itkIO/config.hpp"

namespace itkIO
{

/// allocate a new data Image from an itk one. itkImage release buffer management so ::fwData::Image should manage it
template< class ITKIMAGE>
::fwData::Image::sptr dataImageFactory( typename ITKIMAGE::Pointer itkImage );

/// update a data Image from an itk one. itkImage release buffer management so ::fwData::Image should manage it
template< class ITKIMAGE>
void dataImageFactory( typename ITKIMAGE::Pointer itkImage , ::fwData::Image::sptr _dataImage );

/// create an ITK image from an ::fwData::Image . itk Image does not manage its buffer, an assertion check if
/// ITKIMAGE::PixelType correspond to imageData->getPixelType
template< class ITKIMAGE>
typename ITKIMAGE::Pointer itkImageFactory( ::fwData::Image::sptr imageData);

}

#include "itkIO/itk.hxx"

#endif // _FWITKIO_ITK_HPP_
