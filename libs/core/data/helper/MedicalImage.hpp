/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/config.hpp"
#include <data/Composite.hpp>
#include <data/Image.hpp>
#include <data/Integer.hpp>

namespace sight::data
{

namespace helper
{

/**
 * @brief   Helpers for medical image.
 */
class DATA_CLASS_API MedicalImage
{
public:

    SIGHT_DECLARE_CLASS(MedicalImage);

    /// Image orientation
    typedef enum
    {
        X_AXIS = 0,
        Y_AXIS,
        Z_AXIS
    } Orientation;

    /// Destructor. Do nothing.
    DATA_API virtual ~MedicalImage();

    /// Constructor. Do nothing.
    DATA_API MedicalImage();

    /// Return the image orientation.
    Orientation getOrientation() const
    {
        return m_orientation;
    }

    /**
     * @brief Get the image spacing.
     * @param[out] spacing : the image spacing
     */
    DATA_API void getImageSpacing(double spacing[3]) const;

    /**
     * @brief Get the image origin.
     * @param[out] origin : the image origin
     */
    DATA_API void getImageOrigin(double origin[3]) const;

    /**
     * @brief Get the image data size (number of slices).
     * @param[out] size : the image size
     */
    DATA_API void getImageDataSize(size_t size[3]) const;

    /**
     * @brief Get the image size ( = dataSize * spacing ).
     * @param[out] size : the image size
     */
    DATA_API void getImageSize(double size[3]) const;

    /**
     * @brief Get the slice center
     * @param[out] center : the slice center
     */
    DATA_API void getCurrentSliceCenter(double center[3]);

    /// Return the image
    DATA_API data::Image::sptr getImage() const;

    /**
     * @brief Return the 4 points of the image plane
     *
     * - points are ordered in the following way : image origin is the first point
     * - points are inserted using the preference follow X axis if exists, if not exists follow Y axis
     * if Y axis is not present follow Z axis
     */
    DATA_API void getPlane(double points[4][3], int sliceNumber);

    /// Get the slice index
    DATA_API void getSliceIndex(data::Integer::sptr index[3]);

    /// Set the slice index
    DATA_API bool setSliceIndex(const int index[3]);

    /**
     * @brief Set the orientation of the image.
     * @param[in] orientation must be 0 (X_AXIS), 1 (Y_AXIS) or 2 (Z_AXIS).
     */
    DATA_API void setOrientation(int orientation);

    /// Set the image orientation.
    DATA_API virtual void setOrientation(Orientation orientation);

    /// Update the image information (slice index, min/max,...)
    DATA_API void updateImageInfos(data::Image::sptr image);

    /**
     * @brief Convert world coordinates to slice index coordinates
     * @param[in] world : coordinate in the world
     * @param[out] index : coordinate in the slice index
     */
    DATA_API void worldToSliceIndex(const double world[3], int index[3]);

    /**
     * @brief Convert coordinates in the world to coordinates in the image
     * @param[in] world : coordinate in the world
     * @param[out] index : coordinate in the image
     */
    DATA_API void worldToImageSliceIndex(const double world[3], int index[3]);

    /**
     * @brief Convert from world coordinates system to image coordinates system
     * @param[in] index : coordinate in the image
     * @param[out] world : coordinate in the world
     */
    DATA_API void sliceIndexToWorld(const int index[3], double world[3]);

protected:

    /// Image orientation
    Orientation m_orientation;

    /// Axial slice index
    data::Integer::sptr m_axialIndex;

    /// Frontal slice index
    data::Integer::sptr m_frontalIndex;

    /// Sagittal slice index
    data::Integer::sptr m_sagittalIndex;

    /// Current image
    data::Image::wptr m_weakImage;
};

} //namespace helper

} //namespace sight::data
