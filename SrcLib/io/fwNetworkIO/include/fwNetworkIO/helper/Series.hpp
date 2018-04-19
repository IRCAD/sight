/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwNetworkIO/config.hpp"

#include <fwData/Vector.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/scu.h>

#include <QByteArray>

namespace fwMedData
{
class DicomSeries;
}

namespace fwMedData
{
class ImageSeries;
class ModelSeries;
}

namespace fwNetworkIO
{
namespace helper
{

/**
 * @brief Series Helper
 */
class FWNETWORKIO_CLASS_API Series
{
public:

    typedef ::fwMedData::SeriesDB::ContainerType DicomSeriesContainer;
    typedef std::vector< std::string > InstanceUIDContainer;
    typedef std::map < std::string, unsigned int > InstanceCountMap;

    /**
     * @brief Convert HTTP series response to ::fwMedData::DicomSeries
     * @param[in] answer HTTP responses from the PACS that must be converted
     */
    FWNETWORKIO_API static DicomSeriesContainer toFwMedData(const QByteArray& answer);

    /**
     * @brief Convert std::vector< ::fwMedData::DicomSeries > to series instance uid container
     * @param[in] series Series vector used to extract the series instance uids
     */
    FWNETWORKIO_API static InstanceUIDContainer toSeriesInstanceUIDContainer(DicomSeriesContainer series);

};

} //helper
} //fwNetworkIO
