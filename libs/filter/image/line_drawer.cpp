/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "filter/image/line_drawer.hpp"

#include <core/spy_log.hpp>

#include <data/helper/MedicalImage.hpp>

#include <utility>

namespace sight::filter::image
{

//-----------------------------------------------------------------------------

line_drawer::line_drawer(data::image::sptr img, data::image::csptr roi) :
    m_image(std::move(img)),
    m_roiImage(std::move(roi))
{
    m_useROI = data::helper::MedicalImage::checkImageValidity(m_roiImage);

    m_imageTypeSize = std::uint8_t(m_image->getType().size());
    m_roiTypeSize   = m_useROI ? std::uint8_t(m_roiImage->getType().size()) : 0;
    const auto& size = m_image->size();
    m_yPitch = size[0];
    m_zPitch = size[1] * m_yPitch;
}

//-----------------------------------------------------------------------------

bool line_drawer::drawEllipse(
    const line_drawer::CoordinatesType& c,
    data::image::BufferType* value,
    const double radius,
    const std::size_t firstDim,
    const std::size_t secondDim,
    const bool overwrite,
    image_diff& diff
)
{
    bool modified = false;

    const auto& spacing = m_image->getSpacing();
    const auto& size    = m_image->size();

    const double width  = radius / spacing[firstDim];
    const double height = radius / spacing[secondDim];

    int origX = static_cast<int>(c[firstDim]);
    int origY = static_cast<int>(c[secondDim]);

    line_drawer::CoordinatesType point = c;

    int w_begin = std::max(static_cast<int>(-width), -origX);
    int h_begin = std::max(static_cast<int>(-height), -origY);

    int w_end = std::min(static_cast<int>(width), static_cast<int>(size[firstDim]) - 1 - origX);
    int h_end = std::min(static_cast<int>(height), static_cast<int>(size[secondDim]) - 1 - origY);

    for(int y = h_begin ; y <= h_end ; y++)
    {
        for(int x = w_begin ; x <= w_end ; x++)
        {
            double dx = x / width;
            double dy = y / height;
            if(dx * dx + dy * dy <= 1)
            {
                point[firstDim]  = static_cast<data::image::IndexType>(origX) + static_cast<data::image::IndexType>(x);
                point[secondDim] = static_cast<data::image::IndexType>(origY) + static_cast<data::image::IndexType>(y);

                const data::image::IndexType index = point[0] + point[1] * m_yPitch + point[2] * m_zPitch;

                modified |= this->drawPixel(index, value, overwrite, diff);
            }
        }
    }

    return modified;
}

//-----------------------------------------------------------------------------

bool line_drawer::drawPixel(
    const data::image::IndexType index,
    data::image::BufferType* value,
    const bool overwrite,
    image_diff& diff
)
{
    const data::image::BufferType* pixBuf =
        reinterpret_cast<data::image::BufferType*>(m_image->getPixel(index));

    if(m_useROI)
    {
        const auto* roiVal =
            reinterpret_cast<const data::image::BufferType*>(m_roiImage->getPixel(index));
        if(data::helper::MedicalImage::isBufNull(roiVal, m_roiTypeSize))
        {
            return false;
        }
    }

    if(std::equal(pixBuf, pixBuf + m_imageTypeSize, value))
    {
        return false;
    }

    if(!overwrite && !data::helper::MedicalImage::isBufNull(pixBuf, m_imageTypeSize))
    {
        return false;
    }

    diff.addDiff(index, pixBuf, value);
    m_image->setPixel(index, value);

    return true;
}

//-----------------------------------------------------------------------------

image_diff line_drawer::draw(
    const bresenham_line::Orientation orientation,
    const CoordinatesType& startCoord,
    const CoordinatesType& endCoord,
    data::image::BufferType* value,
    const double thickness,
    const bool overwrite
)
{
    image_diff diff(m_imageTypeSize, 128);

    std::size_t dim0 = 0;
    std::size_t dim1 = 0;

    switch(orientation)
    {
        case bresenham_line::Orientation::Z_AXIS:
            dim0 = 0;
            dim1 = 1;
            break;

        case bresenham_line::Orientation::Y_AXIS:
            dim0 = 2;
            dim1 = 0;
            break;

        case bresenham_line::Orientation::X_AXIS:
            dim0 = 1;
            dim1 = 2;
            break;

        default:
            SIGHT_ASSERT("Unknown axis", false);
            dim0 = 0;
            dim1 = 1;
    }

    bresenham_line::PathType path = bresenham_line::draw(orientation, startCoord, endCoord);

    auto pixel    = path.begin();
    auto endPixel = path.end();

    bool modified = false;

    for( ; pixel != endPixel ; ++pixel)
    {
        modified = this->drawEllipse(*pixel, value, thickness / 2.0, dim0, dim1, overwrite, diff) || modified;
    }

    return diff;
}

} // namespace sight::filter::image
