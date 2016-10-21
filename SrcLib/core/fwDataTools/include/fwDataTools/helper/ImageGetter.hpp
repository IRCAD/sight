/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATATOOLS_HELPER_IMAGEGETTER_HPP__
#define __FWDATATOOLS_HELPER_IMAGEGETTER_HPP__

#include "fwDataTools/config.hpp"

#include <fwData/Image.hpp>

namespace fwDataTools
{
namespace helper
{

/**
 * @brief   Defines an helper to modify an ::fwData::Image by adding few medical fields
 * and create in parallel the message to announce these modifications.
 */
class FWDATATOOLS_CLASS_API ImageGetter
{

public:
    typedef ::fwData::Image::SizeType SizeType;
    typedef ::fwData::Image::IndexType IndexType;
    typedef ::fwData::Image::BufferType BufferType;
    typedef ::fwData::Image::BufferIndexType BufferIndexType;

    /// Constructor. Initialize parameters.
    FWDATATOOLS_API ImageGetter( ::fwData::Image::csptr image );

    /// Destrucotr. Do nothing.
    FWDATATOOLS_API virtual ~ImageGetter();

    /// Returns image buffer
    FWDATATOOLS_API void* getBuffer() const;

    /** @{
     * @brief Helpers for 3D images
     */
    FWDATATOOLS_API void* getPixelBuffer( SizeType::value_type x, SizeType::value_type y,
                                          SizeType::value_type z ) const;

    FWDATATOOLS_API void* getPixelBuffer( IndexType index ) const;

    FWDATATOOLS_API const std::string getPixelAsString(SizeType::value_type x,
                                                       SizeType::value_type y,
                                                       SizeType::value_type z ) const;
    // @}

private:

    ::fwData::Image::csptr m_image;

    ::fwMemory::BufferObject::Lock m_lock;
};

} // helper
} // fwDataTools

#endif // __FWDATATOOLS_HELPER_IMAGEGETTER_HPP__

