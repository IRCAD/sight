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

#include "image.hpp"

#include <core/spy_log.hpp>

namespace sight::data::dicom
{

image::image(
    std::uint16_t _samples_per_pixel,
    std::uint16_t _bits_allocated,
    std::uint16_t _bits_stored,
    std::uint16_t _high_bit,
    std::uint16_t _pixel_representation,
    double _rescale_slope,
    double _rescale_intercept
) :
    m_samples_per_pixel(_samples_per_pixel),
    m_bits_allocated(_bits_allocated),
    m_bits_stored(_bits_stored),
    m_high_bit(_high_bit),
    m_pixel_representation(_pixel_representation),
    m_rescale_slope(_rescale_slope),
    m_rescale_intercept(_rescale_intercept)
{
}

//-----------------------------------------------------------------------------

image::~image()
= default;

//-----------------------------------------------------------------------------

core::type image::find_image_type_from_min_max_values() const
{
    core::type result = core::type::NONE;

    // Bool & Monochrome values
    if(m_bits_allocated == 1 && m_pixel_representation == 0)
    {
        result = core::type::INT8;
    }
    else
    {
        // Double
        if(m_rescale_slope != (int) m_rescale_slope || m_rescale_intercept != (int) m_rescale_intercept)
        {
            result = core::type::DOUBLE;
        }
        else
        {
            const auto min =
                static_cast<int64_t>(m_rescale_slope * static_cast<double>(this->get_pixel_min())
                                     + m_rescale_intercept);
            const auto max =
                static_cast<int64_t>(m_rescale_slope * static_cast<double>(this->get_pixel_max())
                                     + m_rescale_intercept);

            SIGHT_ASSERT("Min must be lower than max.", min <= max);

            // Unsigned values
            if(min >= 0)
            {
                if(max <= std::numeric_limits<uint8_t>::max())
                {
                    result = core::type::UINT8;
                }
                else if(max <= std::numeric_limits<uint16_t>::max())
                {
                    result = core::type::UINT16;
                }
                else if(max <= std::numeric_limits<uint32_t>::max())
                {
                    result = core::type::UINT32;
                }
                else
                {
                    SIGHT_ASSERT("Unable to determine the pixel format.", 0);
                }
            }
            // Signed values
            else
            {
                if(max <= std::numeric_limits<int8_t>::max() && min >= std::numeric_limits<int8_t>::min())
                {
                    result = core::type::INT8;
                }
                else if(max <= std::numeric_limits<int16_t>::max() && min >= std::numeric_limits<int16_t>::min())
                {
                    result = core::type::INT16;
                }
                else if(max <= std::numeric_limits<int32_t>::max() && min >= std::numeric_limits<int32_t>::min())
                {
                    result = core::type::INT32;
                }
                else
                {
                    SIGHT_ASSERT("Unable to determine the pixel format.", 0);
                }
            }
        }
    }

    return result;
}

//-----------------------------------------------------------------------------

int64_t image::get_pixel_min() const
{
    SIGHT_ASSERT("The number of bits allocated must be known", m_bits_allocated);
    SIGHT_ASSERT("The number of bits stored must be less than 33.", m_bits_stored <= 32);

    if(m_pixel_representation == 1)
    {
        return (int64_t) (~(((1ULL << m_bits_stored) - 1) >> 1));
    }

    if(m_pixel_representation == 0)
    {
        return 0;
    }

    SIGHT_ASSERT("Unable to determine minimum value of pixel", 0);
    return 0;
}

//-----------------------------------------------------------------------------

int64_t image::get_pixel_max() const
{
    SIGHT_ASSERT("The number of bits allocated must be known", m_bits_allocated);
    SIGHT_ASSERT("The number of bits stored must be less than 33.", m_bits_stored <= 32);

    if(m_pixel_representation == 1)
    {
        return (int64_t) (((1ULL << m_bits_stored) - 1) >> 1);
    }

    if(m_pixel_representation == 0)
    {
        return (int64_t) ((1ULL << m_bits_stored) - 1);
    }

    SIGHT_ASSERT("Unable to determine maximum value of pixel", 0);
    return 0;
}

} // namespace sight::data::dicom
