/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwNetworkIO/config.hpp"

#include <fwData/Vector.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <QJsonObject>

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
    FWNETWORKIO_API static DicomSeriesContainer toFwMedData(const QJsonObject& answer);

    /**
     * @brief Convert std::vector< ::fwMedData::DicomSeries > to series instance uid container
     * @param[in] series Series vector used to extract the series instance uids
     */
    FWNETWORKIO_API static InstanceUIDContainer toSeriesInstanceUIDContainer(DicomSeriesContainer series);

};

} //helper
} //fwNetworkIO
