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

    FWDATATOOLS_API ~ImageDiff();

    /// Move constructor (swap buffers)
    FWDATATOOLS_API ImageDiff(ImageDiff&& voxDiff);

    /// Copy constructor forbidden
    ImageDiff(const ImageDiff& voxDiff) = delete;

    ::fwData::Image::IndexType m_index;
    ::fwData::Image::BufferType* m_oldValue;
    ::fwData::Image::BufferType* m_newValue;
};

typedef std::vector<ImageDiff> ImageDiffsType;

} // namespace fwDataTools

#endif // __FWDATATOOLS_IMAGEDIFF_HPP__
