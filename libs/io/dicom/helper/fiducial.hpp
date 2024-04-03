/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#pragma once

#include <sight/io/dicom/config.hpp>

#include <core/macros.hpp>

#include <gdcmDataSet.h>

namespace sight::data
{

class series_set;

} // namespace sight::data

namespace sight::io::dicom::helper
{

/**
 * @brief Fiducial helper.
 */
class SIGHT_IO_DICOM_CLASS_API fiducial
{
public:

    /**
     * @brief Return true if the series contains at least one image containing landmarks.
     * @param[in] _series_set series_set where to look for landmarks
     */
    SIGHT_IO_DICOM_API static bool contains_landmarks(const SPTR(data::series_set)& _series_set);

    /**
     * @brief Return true if the series contains at least one image containing distances.
     * @param[in] _series_set series_set where to look for distances
     */
    SIGHT_IO_DICOM_API static bool contains_distances(const SPTR(data::series_set)& _series_set);

    /**
     * @brief Return true if the series contains at least one image containing 3D distances.
     * @param[in] _series_set series_set where to look for 3D distances
     */
    SIGHT_IO_DICOM_API static bool contains_3d_distances(const SPTR(data::series_set)& _series_set);
};

} // namespace sight::io::dicom::helper
