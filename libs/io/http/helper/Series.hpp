/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/http/config.hpp"

#include <data/SeriesDB.hpp>
#include <data/Vector.hpp>

#include <QJsonObject>

namespace sight::data
{
class DicomSeries;
}

namespace sight::data
{
class ImageSeries;
class ModelSeries;
}

namespace sight::io::http
{
namespace helper
{

/**
 * @brief Series Helper
 */
class IO_HTTP_CLASS_API Series
{
public:

    typedef data::SeriesDB::ContainerType DicomSeriesContainer;
    typedef std::vector< std::string > InstanceUIDContainer;
    typedef std::map < std::string, unsigned int > InstanceCountMap;

    /**
     * @brief Convert HTTP series response to data::DicomSeries
     * @param[in] answer HTTP responses from the PACS that must be converted
     */
    IO_HTTP_API static DicomSeriesContainer toFwMedData(const QJsonObject& answer);

    /**
     * @brief Convert std::vector< data::DicomSeries > to series instance uid container
     * @param[in] series Series vector used to extract the series instance uids
     */
    IO_HTTP_API static InstanceUIDContainer toSeriesInstanceUIDContainer(DicomSeriesContainer series);

};

} //helper
} //fwNetworkIO
