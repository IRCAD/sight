/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
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

#include "filter/image/LineDrawer.hpp"

#include <core/spyLog.hpp>

#include <data/helper/MedicalImage.hpp>

#include <utility>

namespace sight::filter::image
{

//-----------------------------------------------------------------------------

LineDrawer::LineDrawer(data::Image::sptr img, data::Image::csptr roi) :
    m_image(std::move(img)),
    m_roiImage(std::move(roi))
{
    m_useROI = data::helper::MedicalImage::checkImageValidity(m_roiImage);

    m_imageTypeSize = std::uint8_t(m_image->getType().size());
    m_roiTypeSize   = m_useROI ? std::uint8_t(m_roiImage->getType().size()) : 0;
    const auto& size = m_image->getSize();
    m_yPitch = size[0];
    m_zPitch = size[1] * m_yPitch;
}

//-----------------------------------------------------------------------------

bool LineDrawer::drawEllipse(
    const LineDrawer::CoordinatesType& c,
    data::Image::BufferType* value,
    const double radius,
    const std::size_t firstDim,
    const std::size_t secondDim,
    const bool overwrite,
    ImageDiff& diff
)
{
    bool modified = false;

    const auto& spacing = m_image->getSpacing();
    const auto& size    = m_image->getSize();

    const double width  = radius / spacing[firstDim];
    const double height = radius / spacing[secondDim];

    int origX = static_cast<int>(c[firstDim]);
    int origY = static_cast<int>(c[secondDim]);

    LineDrawer::CoordinatesType point = c;

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
                point[firstDim]  = static_cast<data::Image::IndexType>(origX) + static_cast<data::Image::IndexType>(x);
                point[secondDim] = static_cast<data::Image::IndexType>(origY) + static_cast<data::Image::IndexType>(y);

                const data::Image::IndexType index = point[0] + point[1] * m_yPitch + point[2] * m_zPitch;

                modified |= this->drawPixel(index, value, overwrite, diff);
            }
        }
    }

    return modified;
}

//-----------------------------------------------------------------------------

bool LineDrawer::drawPixel(
    const data::Image::IndexType index,
    data::Image::BufferType* value,
    const bool overwrite,
    ImageDiff& diff
)
{
    const data::Image::BufferType* pixBuf =
        reinterpret_cast<data::Image::BufferType*>(m_image->getPixel(index));

    if(m_useROI)
    {
        const auto* roiVal =
            reinterpret_cast<const data::Image::BufferType*>(m_roiImage->getPixel(index));
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

ImageDiff LineDrawer::draw(
    const BresenhamLine::Orientation orientation,
    const CoordinatesType& startCoord,
    const CoordinatesType& endCoord,
    data::Image::BufferType* value,
    const double thickness,
    const bool overwrite
)
{
    ImageDiff diff(m_imageTypeSize, 128);

    std::size_t dim0 = 0;
    std::size_t dim1 = 0;

    switch(orientation)
    {
        case BresenhamLine::Orientation::Z_AXIS:
            dim0 = 0;
            dim1 = 1;
            break;

        case BresenhamLine::Orientation::Y_AXIS:
            dim0 = 2;
            dim1 = 0;
            break;

        case BresenhamLine::Orientation::X_AXIS:
            dim0 = 1;
            dim1 = 2;
            break;

        default:
            SIGHT_ASSERT("Unknown axis", false);
            dim0 = 0;
            dim1 = 1;
    }

    BresenhamLine::PathType path = BresenhamLine::draw(orientation, startCoord, endCoord);

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
