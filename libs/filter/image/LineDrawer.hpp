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

#pragma once

#include "filter/image/BresenhamLine.hpp"
#include "filter/image/config.hpp"
#include "filter/image/ImageDiff.hpp"

#include <data/Image.hpp>

#include <array>

namespace sight::filter::image
{

/**
 * @brief Draws lines with a given thickness in a 3D image slice.
 */
class FILTER_IMAGE_CLASS_API LineDrawer
{
public:

    typedef BresenhamLine::CoordinatesType CoordinatesType;

    /// Constructor, instantiates a drawer with an image and a region of interest.
    FILTER_IMAGE_API LineDrawer(const data::Image::sptr& img, const data::Image::csptr& roi);

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
    FILTER_IMAGE_API ImageDiff draw(
        const BresenhamLine::Orientation orientation,
        const CoordinatesType& startCoord,
        const CoordinatesType& endCoord,
        data::Image::BufferType* value,
        const double thickness,
        const bool overwrite = true
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
        const LineDrawer::CoordinatesType& c,
        data::Image::BufferType* value,
        const double radius,
        const std::size_t firstDim,
        const std::size_t secondDim,
        const bool overwrite,
        ImageDiff& diff
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
        const data::Image::IndexType index,
        data::Image::BufferType* value,
        const bool overwrite,
        ImageDiff& diff
    );

    /// Takes ROI into account.
    bool m_useROI;

    /// Image pixel size.
    unsigned char m_imageTypeSize;

    /// ROI pixel size.
    unsigned char m_roiTypeSize;

    /// Image length.
    data::Image::IndexType m_yPitch;

    /// Image depth.
    data::Image::IndexType m_zPitch;

    /// Image to draw in.
    data::Image::sptr m_image;

    /// ROI to use.
    data::Image::csptr m_roiImage;
};

} // namespace sight::filter::image
