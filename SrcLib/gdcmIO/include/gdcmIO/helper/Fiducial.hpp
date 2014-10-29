/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_HELPER_FIDUCIAL_HPP__
#define __GDCMIO_HELPER_FIDUCIAL_HPP__

#include <gdcmDataSet.h>

#include "gdcmIO/config.hpp"

namespace fwMedData
{
    class SeriesDB;
}

namespace gdcmIO
{
namespace helper
{

/**
 * @class Fiducial.
 * @brief Fiducial helper.
 */
class GDCMIO_CLASS_API Fiducial
{

public:

    /**
     * @brief Return true if the series contains at least one image containing landmarks.
     * @param[in] seriesDB SeriesDB where to look for landmarks
     */
    GDCMIO_API static bool containsLandmarks(SPTR(::fwMedData::SeriesDB) seriesDB);

    /**
     * @brief Return true if the series contains at least one image containing distances.
     * @param[in] seriesDB SeriesDB where to look for distances
     */
    GDCMIO_API static bool containsDistances(SPTR(::fwMedData::SeriesDB) seriesDB);

    /**
     * @brief Return true if the series contains at least one image containing 3D distances.
     * @param[in] seriesDB SeriesDB where to look for 3D distances
     */
    GDCMIO_API static bool contains3DDistances(SPTR(::fwMedData::SeriesDB) seriesDB);
};

} // namespace helper
} // namespace gdcmIO

#endif /* __GDCMIO_HELPER_FIDUCIAL_HPP__ */
