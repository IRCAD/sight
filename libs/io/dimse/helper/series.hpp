/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include "io/dimse/data/pacs_configuration.hpp"

#include <data/series_set.hpp>
#include <data/vector.hpp>

#include <dcmtk/dcmnet/scu.h>

namespace sight::data
{

class dicom_series;
class image_series;
class model_series;

} // namespace sight::data

namespace sight::io::dimse::helper
{

/// Series Helper.
class IO_DIMSE_CLASS_API series
{
public:

    using DicomSeriesContainer = sight::data::series_set::container_t;
    using InstanceUIDContainer = std::vector<std::string>;
    using instance_count_map   = std::map<std::string, unsigned int>;

    /**
     * @brief Releases the responses.
     * @param _responses the responses to free.
     */
    IO_DIMSE_API static void release_responses(OFList<QRResponse*> _responses);

    /**
     * @brief Converts DCMTK series to data::dicom_series.
     * @param _responses the DCMTK responses from the pacs that must be converted.
     */
    IO_DIMSE_API static DicomSeriesContainer to_fw_med_data(OFList<QRResponse*> _responses);

    /**
     * @brief Converts DCMTK series to instance uid vector.
     * @param _responses the DCMTK responses from the pacs that must be converted.
     */
    IO_DIMSE_API static InstanceUIDContainer to_series_instance_uid_container(OFList<QRResponse*> _responses);

    /**
     * @brief Converts std::vector< data::dicom_series > to series instance uid container.
     * @param _series the series vector used to extract the series instance uids.
     */
    IO_DIMSE_API static InstanceUIDContainer to_series_instance_uid_container(DicomSeriesContainer _series);
};

} // namespace sight::io::dimse::helper
