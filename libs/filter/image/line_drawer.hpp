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

#pragma once

#include "filter/image/bresenham_line.hpp"
#include "filter/image/config.hpp"
#include "filter/image/image_diff.hpp"

#include <data/image.hpp>

#include <array>

namespace sight::filter::image
{

/**
 * @brief Draws lines with a given thickness in a 3D image slice.
 */
class FILTER_IMAGE_CLASS_API line_drawer
{
public:

    typedef bresenham_line::coordinates_t coordinates_t;

    /// Constructor, instantiates a drawer with an image and a region of interest.
    FILTER_IMAGE_API line_drawer(data::image::sptr _img, data::image::csptr _roi);

    /**
     * @brief Draws a line in an image slice.
     * @param[in] orientation slice orientation.
     * @param[in] startCoord point where the line begins.
     * @param[in] endCoord point where the line ends.
     * @param[in] value value written in the image buffer.
     * @param[in] thickness line thickness in millimeters.
     * @param[in] overwrite overwrite mode, don't overwrite existing pixels if not set.
     * @return The differences in the image before and after drawing.
     */
    FILTER_IMAGE_API image_diff draw(
        bresenham_line::Orientation _orientation,
        const coordinates_t& _start_coord,
        const coordinates_t& _end_coord,
        data::image::buffer_t* _value,
        double _thickness,
        bool _overwrite = true
    );

private:

    /**
     * @brief Draws a filled discrete ellipse in a 3D image slice.
     *
     *  The minor and major axes are computed from the radius and the spacing.
     *
     * @param[in] c center of the ellipse.
     * @param[in] value value written in the image buffer.
     * @param[in] radius the radius of the ellipse in millimeters.
     * @param[in] firstDim the main axis of the plane (slice) in which the ellipse is drawn.
     * @param[in] secondDim the secondary axis of the plane (slice) in which the ellipse is drawn.
     * @param[in] overwrite overwrite mode, don't overwrite existing pixels if not set.
     * @param[in,out] diff vector storing the differences in the image before and after drawing.
     * @return true if at least one pixel has been drawn, false otherwise.
     */
    bool drawEllipse(
        const line_drawer::coordinates_t& _c,
        data::image::buffer_t* _value,
        double _radius,
        std::size_t _first_dim,
        std::size_t _second_dim,
        bool _overwrite,
        image_diff& _diff
    );

    /**
     * @brief Draws a pixel in a 3D image if the right conditions are met.
     *
     * Drawing conditions:
     * - Inside the ROI, if one is used
     * - The pixel has a value different from the one being set
     * - Don't overwrite a non empty pixel if not in overwrite mode
     *
     * @param[in] index index in the image buffer where the pixel will be written.
     * @param[in] value value written in the image buffer.
     * @param[in] overwrite overwrite mode, don't overwrite existing pixels if not set.
     * @param[in,out] diff vector storing the differences in the image before and after drawing.
     * @return true if at least one pixel has been drawn, false otherwise.
     */
    bool drawPixel(
        data::image::index_t _index,
        data::image::buffer_t* _value,
        bool _overwrite,
        image_diff& _diff
    );

    /// Takes ROI into account.
    bool m_useROI;

    /// image pixel size.
    unsigned char m_imageTypeSize;

    /// ROI pixel size.
    unsigned char m_roiTypeSize;

    /// image length.
    data::image::index_t m_yPitch;

    /// image depth.
    data::image::index_t m_zPitch;

    /// image to draw in.
    data::image::sptr m_image;

    /// ROI to use.
    data::image::csptr m_roiImage;
};

} // namespace sight::filter::image
