/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
