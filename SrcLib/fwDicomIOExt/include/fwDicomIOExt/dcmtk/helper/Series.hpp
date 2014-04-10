/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_DCMTK_HELPER_SERIES_HPP__
#define __FWDICOMIOEXT_DCMTK_HELPER_SERIES_HPP__

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/scu.h>

#include <fwData/Vector.hpp>
#include <fwMedData/SeriesDB.hpp>

#include "fwDicomIOExt/data/PacsConfiguration.hpp"
#include "fwDicomIOExt/config.hpp"

namespace fwDicomData
{
    class DicomSeries;
}

namespace fwMedData
{
    class ImageSeries;
    class ModelSeries;
}

namespace fwDicomIOExt
{
namespace dcmtk
{
namespace helper
{

/**
 * @class Series
 * @brief Series Helper
 * @date  2013.
 */
class FWDICOMIOEXT_CLASS_API Series
{
public:

    typedef ::fwMedData::SeriesDB::ContainerType DicomSeriesContainer;
    typedef std::vector< std::string > InstanceUIDContainer;
    typedef std::map < std::string, unsigned int > InstanceCountMap;

    /**
     * @brief Release the responses
     * @param[in] responses Responses to free
     */
    FWDICOMIOEXT_API static void releaseResponses(OFList< QRResponse* > responses);

    /**
     * @brief Convert DCMTK series to ::fwDicomData::DicomSeries
     * @param[in] responses DCMTK responses from the pacs that must be converted
     */
    FWDICOMIOEXT_API static DicomSeriesContainer toFwMedData(OFList< QRResponse* > responses);

    /**
     * @brief Convert DCMTK series to instance uid vector
     * @param[in] responses DCMTK responses from the pacs that must be converted
     */
    FWDICOMIOEXT_API static InstanceUIDContainer toSeriesInstanceUIDContainer(OFList< QRResponse* > responses);

    /**
     * @brief Convert std::vector< ::fwDicomData::DicomSeries > to series instance uid container
     * @param[in] series Series vector used to extract the series instance uids
     */
    FWDICOMIOEXT_API static InstanceUIDContainer toSeriesInstanceUIDContainer(DicomSeriesContainer series);

    /**
     * @brief Convert a DicomSeries to an ImageSeries
     * @param[in] series DicomSeries that must be converted
     */
    FWDICOMIOEXT_API static SPTR(::fwMedData::ImageSeries) convertToImageSeries(
            const SPTR(::fwDicomData::DicomSeries)& series);

    /**
     * @brief Convert a DicomSeries to a ModelSeries
     * @param[in] series DicomSeries that must be converted
     */
    FWDICOMIOEXT_API static SPTR(::fwMedData::ModelSeries) convertToModelSeries(
            const SPTR(::fwDicomData::DicomSeries)& series);

};

} //dcmtk
} //helper
} //fwDicomIOExt


#endif /* __FWDICOMIOEXT_DCMTK_HELPER_SERIES_HPP__ */
