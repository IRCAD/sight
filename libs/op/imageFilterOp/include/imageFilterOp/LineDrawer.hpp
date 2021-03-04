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

#pragma once

#include "imageFilterOp/BresenhamLine.hpp"
#include "imageFilterOp/config.hpp"

#include <fwData/Image.hpp>

#include <fwDataTools/helper/MedicalImage.hpp>
#include <fwDataTools/ImageDiff.hpp>

#include <array>

namespace imageFilterOp
{

/**
 * @brief Draws lines with a given thickness in a 3D image slice.
 */
class IMAGEFILTEROP_CLASS_API LineDrawer
{
public:

    typedef BresenhamLine::CoordinatesType CoordinatesType;

    typedef ::fwDataTools::helper::MedicalImage::Orientation OrientationType;

    /// Constructor, instantiates a drawer with an image and a region of interest.
    IMAGEFILTEROP_API LineDrawer(const ::fwData::Image::sptr& img, const ::fwData::Image::csptr& roi);

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
    IMAGEFILTEROP_API ::fwDataTools::ImageDiff draw(const OrientationType orientation,
                                                    const CoordinatesType& startCoord,
                                                    const CoordinatesType& endCoord,
                                                    ::fwData::Image::BufferType* value,
                                                    const double thickness,
                                                    const bool overwrite = true);

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
    bool drawEllipse( const LineDrawer::CoordinatesType& c, ::fwData::Image::BufferType* value,
                      const double radius, const size_t firstDim, const size_t secondDim, const bool overwrite,
                      ::fwDataTools::ImageDiff& diff);

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
    bool drawPixel( const ::fwData::Image::IndexType index, ::fwData::Image::BufferType* value, const bool overwrite,
                    ::fwDataTools::ImageDiff& diff);

    /// Takes ROI into account.
    bool m_useROI;

    /// Image pixel size.
    unsigned char m_imageTypeSize;

    /// ROI pixel size.
    unsigned char m_roiTypeSize;

    /// Image length.
    ::fwData::Image::IndexType m_yPitch;

    /// Image depth.
    ::fwData::Image::IndexType m_zPitch;

    /// Image to draw in.
    ::fwData::Image::sptr m_image;

    /// ROI to use.
    ::fwData::Image::csptr m_roiImage;
};

} // namespace imageFilterOp
