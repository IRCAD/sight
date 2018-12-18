/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
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

#ifndef __FWGDCMIO_HELPER_FIDUCIAL_HPP__
#define __FWGDCMIO_HELPER_FIDUCIAL_HPP__

#include "fwGdcmIO/config.hpp"

#include <fwCore/macros.hpp>
#include <gdcmDataSet.h>

namespace fwMedData
{
class SeriesDB;
}

namespace fwGdcmIO
{
namespace helper
{

/**
 * @brief Fiducial helper.
 */
class FWGDCMIO_CLASS_API Fiducial
{

public:

    /**
     * @brief Return true if the series contains at least one image containing landmarks.
     * @param[in] seriesDB SeriesDB where to look for landmarks
     */
    FWGDCMIO_API static bool containsLandmarks(const SPTR(::fwMedData::SeriesDB)& seriesDB);

    /**
     * @brief Return true if the series contains at least one image containing distances.
     * @param[in] seriesDB SeriesDB where to look for distances
     */
    FWGDCMIO_API static bool containsDistances(const SPTR(::fwMedData::SeriesDB)& seriesDB);

    /**
     * @brief Return true if the series contains at least one image containing 3D distances.
     * @param[in] seriesDB SeriesDB where to look for 3D distances
     */
    FWGDCMIO_API static bool contains3DDistances(const SPTR(::fwMedData::SeriesDB)& seriesDB);
};

} // namespace helper
} // namespace fwGdcmIO

#endif /* __FWGDCMIO_HELPER_FIDUCIAL_HPP__ */
