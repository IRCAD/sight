/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_IMAGE_HPP_
#define _FWCOMED_IMAGE_HPP_

#include <fwData/Image.hpp>

#include <fwServices/IService.hpp>

#include "fwComEd/export.hpp"
#include "fwComEd/ImageMsg.hpp"

namespace fwComEd
{
namespace helper
{

/**
 * @brief   Defines an helper to modify an ::fwData::Image by adding few medical fields
 * and create in parallel the message to announce these modifications.
 * @struct  Image
 * @author  IRCAD (Research and Development Team).
 * @date    2007-20012.
 */
class FWCOMED_CLASS_API Image
{

public :

    typedef ::fwData::Image::SizeType SizeType;
    typedef ::fwData::Image::IndexType IndexType;
    typedef ::fwData::Image::BufferType BufferType;
    typedef ::fwData::Image::BufferIndexType BufferIndexType;

    /// Constructor. Initialize parameters.
    FWCOMED_API Image( ::fwData::Image::sptr image );

    /// Destrucotr. Do nothing.
    FWCOMED_API ~Image();

    /**
     * @brief Create landmark fields if not exist, NOT GENERATE MESSAGE.
     *
     * If the image hasn't landmark field, it is added and the method return true.
     *
     * @return true if the field is added.
     */
    FWCOMED_API bool createLandmarks();

    /**
     * @brief Create transfer function pool if the pool is not present, NOT GENERATE MESSAGE.
     *
     * If the image has not transfer functions, the pool is
     * created and a grey level transfer function is added.
     */
    FWCOMED_API bool createTransferFunctionPool(::fwServices::IService::sptr serviceSource = ::fwServices::IService::sptr() );

    /**
     * @brief Create slice index if indexes are not present, NOT GENERATE MESSAGE.
     *
     * If the image hasn't frontal, axial and sagittal slice index fields, they are added.
     *
     * @return true if the fields are added.
     */
    FWCOMED_API bool createImageSliceIndex();

    /// Send the message of modification
    FWCOMED_API void notify( ::fwServices::IService::sptr _serviceSource );

    /// Returns image buffer
    FWCOMED_API void * getBuffer();

    /// Helpers for 3D images
    FWCOMED_API void* getPixelBuffer( SizeType::value_type x, SizeType::value_type y, SizeType::value_type z );

    FWCOMED_API void* getPixelBuffer( IndexType index );

    /// Returns a copy of current lock on image
    FWCOMED_API ::fwTools::BufferObject::Lock getLock() const;

private :

    ::fwComEd::ImageMsg::sptr m_imageMsg;

    ::fwData::Image::sptr m_image;

    ::fwTools::BufferObject::Lock m_lock;
};

} // helper
} // fwComEd


#endif // _FWCOMED_IMAGE_HPP_

