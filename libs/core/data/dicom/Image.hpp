/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "data/config.hpp"

#include <core/tools/Type.hpp>

#include <stdint.h>

namespace sight::data::dicom
{

/**
 * @brief DICOM Image Helper
 */
class DATA_CLASS_API Image
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
    DATA_API Image(
        unsigned short samplesPerPixel,
        unsigned short bitsAllocated,
        unsigned short bitsStored,
        unsigned short highBit,
        unsigned short pixelRepresentation,
        double rescaleSlope,
        double rescaleIntercept);

    /// Destructor
    DATA_API virtual ~Image();

    /// Find Image Type
    DATA_API core::tools::Type findImageTypeFromMinMaxValues() const;

protected:

    /// Return minimum possible value of the pixel
    DATA_API int64_t getPixelMin() const;

    /// Return maximum possible value of the pixel
    DATA_API int64_t getPixelMax() const;

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
