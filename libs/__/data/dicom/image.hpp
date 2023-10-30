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
        std::uint16_t _samples_per_pixel,
        std::uint16_t _bits_allocated,
        std::uint16_t _bits_stored,
        std::uint16_t _high_bit,
        std::uint16_t _pixel_representation,
        double _rescale_slope,
        double _rescale_intercept
    );

    /// Destructor
    DATA_API virtual ~image();

    /// Find image Type
    [[nodiscard]] DATA_API core::type find_image_type_from_min_max_values() const;

protected:

    /// Return minimum possible value of the pixel
    [[nodiscard]] DATA_API int64_t get_pixel_min() const;

    /// Return maximum possible value of the pixel
    [[nodiscard]] DATA_API int64_t get_pixel_max() const;

    /// Samples per pixel
    std::uint16_t m_samples_per_pixel;

    /// Bits allocated
    std::uint16_t m_bits_allocated;

    /// Bits stored
    std::uint16_t m_bits_stored;

    /// High bit
    std::uint16_t m_high_bit;

    /// Pixel representation
    std::uint16_t m_pixel_representation;

    /// Rescale slope
    double m_rescale_slope;

    /// Rescale Intercept
    double m_rescale_intercept;
};

} // namespace sight::data::dicom
