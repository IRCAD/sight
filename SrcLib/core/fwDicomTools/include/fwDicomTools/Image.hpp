/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMTOOLS_IMAGE_HPP__
#define __FWDICOMTOOLS_IMAGE_HPP__

#include "fwDicomTools/config.hpp"

#include <fwTools/Type.hpp>

#include <stdint.h>

namespace fwDicomTools
{

/**
 * @brief DICOM Image Helper
 */
class FWDICOMTOOLS_CLASS_API Image
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
    FWDICOMTOOLS_API Image(
        unsigned short samplesPerPixel,
        unsigned short bitsAllocated,
        unsigned short bitsStored,
        unsigned short highBit,
        unsigned short pixelRepresentation,
        double rescaleSlope,
        double rescaleIntercept);

    /// Destructor
    FWDICOMTOOLS_API virtual ~Image();

    /// Find Image Type
    FWDICOMTOOLS_API ::fwTools::Type findImageTypeFromMinMaxValues() const;

protected:

    /// Return minimum possible value of the pixel
    FWDICOMTOOLS_API int64_t getPixelMin() const;

    /// Return maximum possible value of the pixel
    FWDICOMTOOLS_API int64_t getPixelMax() const;

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

} //fwDicomTools


#endif /* __FWDICOMTOOLS_IMAGE_HPP__ */
