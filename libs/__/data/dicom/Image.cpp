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

#include "Image.hpp"

#include <core/spy_log.hpp>

namespace sight::data::dicom
{

Image::Image(
    std::uint16_t samplesPerPixel,
    std::uint16_t bitsAllocated,
    std::uint16_t bitsStored,
    std::uint16_t highBit,
    std::uint16_t pixelRepresentation,
    double rescaleSlope,
    double rescaleIntercept
) :
    m_samplesPerPixel(samplesPerPixel),
    m_bitsAllocated(bitsAllocated),
    m_bitsStored(bitsStored),
    m_highBit(highBit),
    m_pixelRepresentation(pixelRepresentation),
    m_rescaleSlope(rescaleSlope),
    m_rescaleIntercept(rescaleIntercept)
{
}

//-----------------------------------------------------------------------------

Image::~Image()
= default;

//-----------------------------------------------------------------------------

core::type Image::findImageTypeFromMinMaxValues() const
{
    core::type result = core::type::NONE;

    // Bool & Monochrome values
    if(m_bitsAllocated == 1 && m_pixelRepresentation == 0)
    {
        result = core::type::INT8;
    }
    else
    {
        // Double
        if(m_rescaleSlope != (int) m_rescaleSlope || m_rescaleIntercept != (int) m_rescaleIntercept)
        {
            result = core::type::DOUBLE;
        }
        else
        {
            const auto min =
                static_cast<int64_t>(m_rescaleSlope * static_cast<double>(this->getPixelMin()) + m_rescaleIntercept);
            const auto max =
                static_cast<int64_t>(m_rescaleSlope * static_cast<double>(this->getPixelMax()) + m_rescaleIntercept);

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

int64_t Image::getPixelMin() const
{
    SIGHT_ASSERT("The number of bits allocated must be known", m_bitsAllocated);
    SIGHT_ASSERT("The number of bits stored must be less than 33.", m_bitsStored <= 32);

    if(m_pixelRepresentation == 1)
    {
        return (int64_t) (~(((1ULL << m_bitsStored) - 1) >> 1));
    }

    if(m_pixelRepresentation == 0)
    {
        return 0;
    }

    SIGHT_ASSERT("Unable to determine minimum value of pixel", 0);
    return 0;
}

//-----------------------------------------------------------------------------

int64_t Image::getPixelMax() const
{
    SIGHT_ASSERT("The number of bits allocated must be known", m_bitsAllocated);
    SIGHT_ASSERT("The number of bits stored must be less than 33.", m_bitsStored <= 32);

    if(m_pixelRepresentation == 1)
    {
        return (int64_t) (((1ULL << m_bitsStored) - 1) >> 1);
    }

    if(m_pixelRepresentation == 0)
    {
        return (int64_t) ((1ULL << m_bitsStored) - 1);
    }

    SIGHT_ASSERT("Unable to determine maximum value of pixel", 0);
    return 0;
}

} // namespace sight::data::dicom
