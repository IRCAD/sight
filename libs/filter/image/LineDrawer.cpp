/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include <data/fieldHelper/MedicalImageHelpers.hpp>
#include <data/helper/Image.hpp>
#include <data/helper/ImageGetter.hpp>

namespace sight::filter::image
{

//-----------------------------------------------------------------------------

LineDrawer::LineDrawer(const data::Image::sptr& img, const data::Image::csptr& roi) :
    m_image(img),
    m_roiImage(roi)
{
    m_useROI = data::fieldHelper::MedicalImageHelpers::checkImageValidity(m_roiImage);

    m_imageTypeSize = m_image->getType().sizeOf();
    m_roiTypeSize   = m_useROI ? m_roiImage->getType().sizeOf() : 0;
    const auto& size = m_image->getSize2();
    m_yPitch = size[0];
    m_zPitch = size[1] * m_yPitch;
}

//-----------------------------------------------------------------------------

bool LineDrawer::drawEllipse(
    const LineDrawer::CoordinatesType& c,
    data::Image::BufferType* value,
    const double radius,
    const size_t firstDim,
    const size_t secondDim,
    const bool overwrite,
    ImageDiff& diff
)
{
    bool modified = false;

    const auto& spacing = m_image->getSpacing2();
    const auto& size    = m_image->getSize2();

    const double width  = radius / spacing[firstDim];
    const double height = radius / spacing[secondDim];

    int origX = static_cast<int>(c[firstDim]);
    int origY = static_cast<int>(c[secondDim]);

    LineDrawer::CoordinatesType point = c;

    int wbegin = std::max(static_cast<int>(-width), -origX);
    int hbegin = std::max(static_cast<int>(-height), -origY);

    int wend = std::min(static_cast<int>(width), static_cast<int>(size[firstDim]) - 1 - origX);
    int hend = std::min(static_cast<int>(height), static_cast<int>(size[secondDim]) - 1 - origY);

    for(int y = hbegin ; y <= hend ; y++)
    {
        for(int x = wbegin ; x <= wend ; x++)
        {
            double dx = x / width;
            double dy = y / height;
            if(dx * dx + dy * dy <= 1)
            {
                point[firstDim]  = static_cast<data::Image::IndexType>(origX + x);
                point[secondDim] = static_cast<data::Image::IndexType>(origY + y);

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
        reinterpret_cast<data::Image::BufferType*>(m_image->getPixelBuffer(index));

    if(m_useROI)
    {
        data::Image::BufferType* roiVal =
            reinterpret_cast<data::Image::BufferType*>(m_roiImage->getPixelBuffer(index));
        if(data::fieldHelper::MedicalImageHelpers::isBufNull(roiVal, m_roiTypeSize))
        {
            return false;
        }
    }

    if(std::equal(pixBuf, pixBuf + m_imageTypeSize, value))
    {
        return false;
    }

    if(!overwrite && !data::fieldHelper::MedicalImageHelpers::isBufNull(pixBuf, m_imageTypeSize))
    {
        return false;
    }

    diff.addDiff(index, pixBuf, value);
    m_image->setPixelBuffer(index, value);

    return true;
}

//-----------------------------------------------------------------------------

ImageDiff LineDrawer::draw(
    const OrientationType orientation,
    const CoordinatesType& startCoord,
    const CoordinatesType& endCoord,
    data::Image::BufferType* value,
    const double thickness,
    const bool overwrite
)
{
    ImageDiff diff(m_imageTypeSize, 128);

    size_t dim0, dim1;

    switch(orientation)
    {
        case data::helper::MedicalImage::Z_AXIS:
            dim0 = 0;
            dim1 = 1;
            break;

        case data::helper::MedicalImage::Y_AXIS:
            dim0 = 2;
            dim1 = 0;
            break;

        case data::helper::MedicalImage::X_AXIS:
            dim0 = 1;
            dim1 = 2;
            break;

        default:
            SIGHT_ASSERT("Unknown axis", false);
            dim0 = 0;
            dim1 = 1;
    }

    BresenhamLine::PathType path = BresenhamLine::draw(orientation, startCoord, endCoord);

    BresenhamLine::PathType::iterator pixel    = path.begin();
    BresenhamLine::PathType::iterator endPixel = path.end();

    bool modified = false;

    for( ; pixel != endPixel ; ++pixel)
    {
        modified = this->drawEllipse(*pixel, value, thickness / 2.0, dim0, dim1, overwrite, diff) || modified;
    }

    return diff;
}

} // namespace sight::filter::image
