/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#ifndef __FWITKIO_ITK_HPP__
#define __FWITKIO_ITK_HPP__


#include <fwData/Image.hpp>

#include "fwItkIO/config.hpp"

namespace fwItkIO
{

/**
 * @brief Allocate a new data Image from an itk one.
 *
 * If bufferManagerIsDataImage = true, itkImage releases buffer management
 * and so ::fwData::Image manages it (in this case, itkImage must be the
 * owner of this buffer). if bufferManagerIsDataImage = false, the created
 * ::fwData::Image does not manage the image buffer.
 */
template< class ITKIMAGE>
::fwData::Image::sptr dataImageFactory( typename ITKIMAGE::Pointer itkImage, bool bufferManagerIsDataImage = true );

/**
 * @brief Update a data Image from an itk one.
 *
 * If bufferManagerIsDataImage = true, itkImage releases buffer management
 * and so ::fwData::Image manages it (in this case, itkImage must be the
 * owner of this buffer). if bufferManagerIsDataImage = false, the created
 * ::fwData::Image does not manage the image buffer.
 */
template< class ITKIMAGE>
void dataImageFactory( typename ITKIMAGE::Pointer itkImage, ::fwData::Image::sptr _dataImage,
                       bool bufferManagerIsDataImage = true );

/**
 * @brief Create an ITK image from an ::fwData::Image.
 *
 * If bufferManagerIsDataImage = true, imageData conserve his buffer management
 * and so the created itkImage not manages it. If bufferManagerIsDataImage = false,
 * the created itkImage manage his image buffer and thus imageData releases his
 * buffer (in this case, imageData must be the owner of this buffer).
 *
 * @pre an assertion check if ITKIMAGE::PixelType correspond to imageData->getPixelType
 * @pre an assertion check if ITKIMAGE dimension correspond to imageData dimension
 */
template< class ITKIMAGE>
typename ITKIMAGE::Pointer itkImageFactory( ::fwData::Image::csptr imageData, bool bufferManagerIsDataImage = true );

}

#include "fwItkIO/itk.hxx"

#endif // __FWITKIO_ITK_HPP__
