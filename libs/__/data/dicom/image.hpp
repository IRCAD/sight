/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <core/type.hpp>

#include <cstdint>

namespace sight::data::dicom
{

/**
 * @brief DICOM image Helper
 */
class DATA_CLASS_API image
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
    DATA_API image(
        std::uint16_t samplesPerPixel,
        std::uint16_t bitsAllocated,
        std::uint16_t bitsStored,
        std::uint16_t highBit,
        std::uint16_t pixelRepresentation,
        double rescaleSlope,
        double rescaleIntercept
    );

    /// Destructor
    DATA_API virtual ~image();

    /// Find image Type
    [[nodiscard]] DATA_API core::type findImageTypeFromMinMaxValues() const;

protected:

    /// Return minimum possible value of the pixel
    [[nodiscard]] DATA_API int64_t getPixelMin() const;

    /// Return maximum possible value of the pixel
    [[nodiscard]] DATA_API int64_t getPixelMax() const;

    /// Samples per pixel
    std::uint16_t m_samplesPerPixel;

    /// Bits allocated
    std::uint16_t m_bitsAllocated;

    /// Bits stored
    std::uint16_t m_bitsStored;

    /// High bit
    std::uint16_t m_highBit;

    /// Pixel representation
    std::uint16_t m_pixelRepresentation;

    /// Rescale slope
    double m_rescaleSlope;

    /// Rescale Intercept
    double m_rescaleIntercept;
};

} // namespace sight::data::dicom
