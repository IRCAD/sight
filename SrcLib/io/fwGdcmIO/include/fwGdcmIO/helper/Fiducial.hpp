/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
