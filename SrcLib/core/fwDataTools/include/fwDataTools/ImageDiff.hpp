/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATATOOLS_IMAGEDIFF_HPP__
#define __FWDATATOOLS_IMAGEDIFF_HPP__

#include "fwDataTools/config.hpp"

#include <fwData/Image.hpp>

#include <vector>

namespace fwDataTools
{

/**
 * @brief Structure used to memorize a pixel change in an image.
 */
struct FWDATATOOLS_CLASS_API ImageDiff
{

    /// Constructor (copy buffers)
    FWDATATOOLS_API ImageDiff(const ::fwData::Image::IndexType index, const ::fwData::Image::BufferType* oldValue,
                              const ::fwData::Image::BufferType* newValue, const unsigned char imageTypeSize);

    /// Destructor
    FWDATATOOLS_API ~ImageDiff();

    /// Copy constructor
    FWDATATOOLS_API ImageDiff(const ImageDiff& other);

    /// Move constructor
    FWDATATOOLS_API ImageDiff(ImageDiff&& other);

    /// Copy assignement.
    FWDATATOOLS_API ImageDiff& operator= (const ImageDiff& other);

    /// Move assignement.
    FWDATATOOLS_API ImageDiff& operator= (ImageDiff&& other);

    ::fwData::Image::IndexType m_index;
    ::fwData::Image::BufferType* m_oldValue;
    ::fwData::Image::BufferType* m_newValue;

    unsigned char m_typeSize;
};

typedef std::vector<ImageDiff> ImageDiffsType;

} // namespace fwDataTools

#endif // __FWDATATOOLS_IMAGEDIFF_HPP__
