/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_DCMTK_HELPER_IMAGE_HPP__
#define __FWDICOMIOEXT_DCMTK_HELPER_IMAGE_HPP__

#include <stdint.h>
#include <fwTools/Type.hpp>

#include "fwDicomIOExt/config.hpp"

namespace fwDicomIOExt
{
namespace dcmtk
{
namespace helper
{

/**
 * @class Image
 * @brief Image Helper
 * @date  2013.
 */
class FWDICOMIOEXT_CLASS_API Image
{
public:

    /**
     * @brief Constructor
     * @param[in] samplesPerPixel Samples per pixel
     * @param[in] bitsAllocated Bits allocated
     * @param[in] bitsStored Bits stored
     * @param[in] highBit High bit
     * @param[in] pixelRepresentation Pixel representation
     * @param[in] rescaleSlope Rescale slope
     * @param[in] rescaleIntercept Rescale intercept
     */
    FWDICOMIOEXT_API Image(
        unsigned short samplesPerPixel,
        unsigned short bitsAllocated,
        unsigned short bitsStored,
        unsigned short highBit,
        unsigned short pixelRepresentation,
        double rescaleSlope,
        double rescaleIntercept);

    /// Destructor
    FWDICOMIOEXT_API virtual ~Image();

    /// Find Image Type
    FWDICOMIOEXT_API ::fwTools::Type findImageTypeFromMinMaxValues() const;

protected:

    /// Return minimum possible value of the pixel
    FWDICOMIOEXT_API int64_t getPixelMin() const;

    /// Return maximum possible value of the pixel
    FWDICOMIOEXT_API int64_t getPixelMax() const;

    /// Samples per pixel
    unsigned short m_samplesPerPixel;

    /// Bits allocated
    unsigned short m_bitsAllocated;

    /// Bits stored
    unsigned short m_bitsStored;

    /// High bit
    unsigned short m_highBit;

    /// Pixel representation
    unsigned short m_pixelRepresentation;

    /// Rescale slope
    double m_rescaleSlope;

    /// Rescale Intercept
    double m_rescaleIntercept;

};

} //dcmtk
} //helper
} //fwDicomIOExt


#endif /* __FWDICOMIOEXT_DCMTK_HELPER_IMAGE_HPP__ */
