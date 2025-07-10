/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/data/config.hpp>

#include <core/type.hpp>

#include <cstdint>

namespace sight::data::dicom
{

/**
 * @brief DICOM image Helper
 */
class SIGHT_DATA_CLASS_API image
{
public:

    /**
     * @brief Constructor
     * @param[in] _samples_per_pixel Samples per pixel
     * @param[in] _bits_allocated Bits allocated
     * @param[in] _bits_stored Bits stored
     * @param[in] _high_bit High bit
     * @param[in] _pixel_representation Pixel representation
     * @param[in] _rescale_slope Rescale slope
     * @param[in] _rescale_intercept Rescale intercept
     */
    SIGHT_DATA_API image(
        std::uint16_t _samples_per_pixel,
        std::uint16_t _bits_allocated,
        std::uint16_t _bits_stored,
        std::uint16_t _high_bit,
        std::uint16_t _pixel_representation,
        double _rescale_slope,
        double _rescale_intercept
    );

    /// Destructor
    SIGHT_DATA_API virtual ~image();

    /// Find image Type
    [[nodiscard]] SIGHT_DATA_API core::type find_image_type_from_min_max_values() const;

protected:

    /// Return minimum possible value of the pixel
    [[nodiscard]] SIGHT_DATA_API int64_t get_pixel_min() const;

    /// Return maximum possible value of the pixel
    [[nodiscard]] SIGHT_DATA_API int64_t get_pixel_max() const;

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
