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

#include <data/helper/medical_image.hpp>

#include <utility>

namespace sight::filter::image
{

//-----------------------------------------------------------------------------

line_drawer::line_drawer(data::image::sptr _img, data::image::csptr _roi) :
    m_image(std::move(_img)),
    m_roiImage(std::move(_roi))
{
    m_useROI = data::helper::medical_image::check_image_validity(m_roiImage);

    m_imageTypeSize = std::uint8_t(m_image->getType().size());
    m_roiTypeSize   = m_useROI ? std::uint8_t(m_roiImage->getType().size()) : 0;
    const auto& size = m_image->size();
    m_yPitch = size[0];
    m_zPitch = size[1] * m_yPitch;
}

//-----------------------------------------------------------------------------

bool line_drawer::drawEllipse(
    const line_drawer::coordinates_t& _c,
    data::image::buffer_t* _value,
    const double _radius,
    const std::size_t _first_dim,
    const std::size_t _second_dim,
    const bool _overwrite,
    image_diff& _diff
)
{
    bool modified = false;

    const auto& spacing = m_image->getSpacing();
    const auto& size    = m_image->size();

    const double width  = _radius / spacing[_first_dim];
    const double height = _radius / spacing[_second_dim];

    int orig_x = static_cast<int>(_c[_first_dim]);
    int orig_y = static_cast<int>(_c[_second_dim]);

    line_drawer::coordinates_t point = _c;

    int w_begin = std::max(static_cast<int>(-width), -orig_x);
    int h_begin = std::max(static_cast<int>(-height), -orig_y);

    int w_end = std::min(static_cast<int>(width), static_cast<int>(size[_first_dim]) - 1 - orig_x);
    int h_end = std::min(static_cast<int>(height), static_cast<int>(size[_second_dim]) - 1 - orig_y);

    for(int y = h_begin ; y <= h_end ; y++)
    {
        for(int x = w_begin ; x <= w_end ; x++)
        {
            double dx = x / width;
            double dy = y / height;
            if(dx * dx + dy * dy <= 1)
            {
                point[_first_dim]  = static_cast<data::image::index_t>(orig_x) + static_cast<data::image::index_t>(x);
                point[_second_dim] = static_cast<data::image::index_t>(orig_y) + static_cast<data::image::index_t>(y);

                const data::image::index_t index = point[0] + point[1] * m_yPitch + point[2] * m_zPitch;

                modified |= this->drawPixel(index, _value, _overwrite, _diff);
            }
        }
    }

    return modified;
}

//-----------------------------------------------------------------------------

bool line_drawer::drawPixel(
    const data::image::index_t _index,
    data::image::buffer_t* _value,
    const bool _overwrite,
    image_diff& _diff
)
{
    const data::image::buffer_t* pix_buf =
        reinterpret_cast<data::image::buffer_t*>(m_image->getPixel(_index));

    if(m_useROI)
    {
        const auto* roi_val =
            reinterpret_cast<const data::image::buffer_t*>(m_roiImage->getPixel(_index));
        if(data::helper::medical_image::is_buf_null(roi_val, m_roiTypeSize))
        {
            return false;
        }
    }

    if(std::equal(pix_buf, pix_buf + m_imageTypeSize, _value))
    {
        return false;
    }

    if(!_overwrite && !data::helper::medical_image::is_buf_null(pix_buf, m_imageTypeSize))
    {
        return false;
    }

    _diff.addDiff(_index, pix_buf, _value);
    m_image->setPixel(_index, _value);

    return true;
}

//-----------------------------------------------------------------------------

image_diff line_drawer::draw(
    const bresenham_line::Orientation _orientation,
    const coordinates_t& _start_coord,
    const coordinates_t& _end_coord,
    data::image::buffer_t* _value,
    const double _thickness,
    const bool _overwrite
)
{
    image_diff diff(m_imageTypeSize, 128);

    std::size_t dim0 = 0;
    std::size_t dim1 = 0;

    switch(_orientation)
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

    bresenham_line::path_t path = bresenham_line::draw(_orientation, _start_coord, _end_coord);

    auto pixel     = path.begin();
    auto end_pixel = path.end();

    bool modified = false;

    for( ; pixel != end_pixel ; ++pixel)
    {
        modified = this->drawEllipse(*pixel, _value, _thickness / 2.0, dim0, dim1, _overwrite, diff) || modified;
    }

    return diff;
}

} // namespace sight::filter::image
