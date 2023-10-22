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

#pragma once

#include "io/dicom/config.hpp"

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
class IO_DICOM_CLASS_API Fiducial
{
public:

    /**
     * @brief Return true if the series contains at least one image containing landmarks.
     * @param[in] seriesSet series_set where to look for landmarks
     */
    IO_DICOM_API static bool containsLandmarks(const SPTR(data::series_set)& _series_set);

    /**
     * @brief Return true if the series contains at least one image containing distances.
     * @param[in] seriesSet series_set where to look for distances
     */
    IO_DICOM_API static bool containsDistances(const SPTR(data::series_set)& _series_set);

    /**
     * @brief Return true if the series contains at least one image containing 3D distances.
     * @param[in] seriesSet series_set where to look for 3D distances
     */
    IO_DICOM_API static bool contains3DDistances(const SPTR(data::series_set)& _series_set);
};

} // namespace sight::io::dicom::helper
