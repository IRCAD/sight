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

#ifndef __FWPACSIO_HELPER_SERIES_HPP__
#define __FWPACSIO_HELPER_SERIES_HPP__

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/scu.h>

#include "fwPacsIO/config.hpp"
#include "fwPacsIO/data/PacsConfiguration.hpp"

#include <fwData/Vector.hpp>
#include <fwMedData/SeriesDB.hpp>

namespace fwMedData
{
class DicomSeries;
}

namespace fwMedData
{
class ImageSeries;
class ModelSeries;
}

namespace fwPacsIO
{
namespace helper
{

/**
 * @brief Series Helper
 */
class FWPACSIO_CLASS_API Series
{
public:

    typedef ::fwMedData::SeriesDB::ContainerType DicomSeriesContainer;
    typedef std::vector< std::string > InstanceUIDContainer;
    typedef std::map < std::string, unsigned int > InstanceCountMap;

    /**
     * @brief Release the responses
     * @param[in] responses Responses to free
     */
    FWPACSIO_API static void releaseResponses(OFList< QRResponse* > responses);

    /**
     * @brief Convert DCMTK series to ::fwMedData::DicomSeries
     * @param[in] responses DCMTK responses from the pacs that must be converted
     */
    FWPACSIO_API static DicomSeriesContainer toFwMedData(OFList< QRResponse* > responses);

    /**
     * @brief Convert DCMTK series to instance uid vector
     * @param[in] responses DCMTK responses from the pacs that must be converted
     */
    FWPACSIO_API static InstanceUIDContainer toSeriesInstanceUIDContainer(OFList< QRResponse* > responses);

    /**
     * @brief Convert std::vector< ::fwMedData::DicomSeries > to series instance uid container
     * @param[in] series Series vector used to extract the series instance uids
     */
    FWPACSIO_API static InstanceUIDContainer toSeriesInstanceUIDContainer(DicomSeriesContainer series);

};

} //helper
} //fwPacsIO


#endif /* __FWPACSIO_HELPER_SERIES_HPP__ */
