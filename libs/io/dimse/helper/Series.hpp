/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "io/dimse/config.hpp"
#include "io/dimse/data/PacsConfiguration.hpp"

#include <data/SeriesDB.hpp>
#include <data/Vector.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/scu.h>

namespace sight::data
{

class DicomSeries;

}

namespace sight::data
{

class ImageSeries;
class ModelSeries;

}

namespace sight::io::dimse
{

namespace helper
{

/// Series Helper.
class IO_DIMSE_CLASS_API Series
{
public:

    typedef sight::data::SeriesDB::ContainerType DicomSeriesContainer;
    typedef std::vector<std::string> InstanceUIDContainer;
    typedef std::map<std::string, unsigned int> InstanceCountMap;

    /**
     * @brief Releases the responses.
     * @param _responses the responses to free.
     */
    IO_DIMSE_API static void releaseResponses(OFList<QRResponse*> _responses);

    /**
     * @brief Converts DCMTK series to data::DicomSeries.
     * @param _responses the DCMTK responses from the pacs that must be converted.
     */
    IO_DIMSE_API static DicomSeriesContainer toFwMedData(OFList<QRResponse*> _responses);

    /**
     * @brief Converts DCMTK series to instance uid vector.
     * @param _responses the DCMTK responses from the pacs that must be converted.
     */
    IO_DIMSE_API static InstanceUIDContainer toSeriesInstanceUIDContainer(OFList<QRResponse*> _responses);

    /**
     * @brief Converts std::vector< data::DicomSeries > to series instance uid container.
     * @param _series the series vector used to extract the series instance uids.
     */
    IO_DIMSE_API static InstanceUIDContainer toSeriesInstanceUIDContainer(DicomSeriesContainer _series);
};

} //helper

} //fwPacsIO
