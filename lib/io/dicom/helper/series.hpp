/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include <sight/io/dicom/config.hpp>

#include <data/series.hpp>

#include <cstddef>
#include <optional>

namespace sight::io::dicom::helper
{

class SIGHT_IO_DICOM_CLASS_API series
{
public:

    /**
     * @brief Get the path where to store the series or an instance of the series.
     * @param _series The series.
     * @param _instance_number The instance number. If not set, the path of the series is returned.
     * @return The path where to store the series or an instance of the series.
     */
    SIGHT_IO_DICOM_API static std::filesystem::path get_path(
        const sight::data::series& _series,
        std::optional<std::size_t> _instance_number = std::nullopt
    );

    /**
     * @brief Get the path where to store the series.
     * @param _series_uid The series.
     * @parm _instance_uid The instance UID.
     * @return The path where to store the series.
     */
    SIGHT_IO_DICOM_API static std::filesystem::path get_path(
        const std::string& _series_uid,
        const std::string& _instance_uid
    );

    /**
     * @brief Indicates an instance is downloaded in the DICOM container of the series.
     * @param _series The series.
     * @param _instance_number The instance number to add.
     * @param _sop_instance_uid The SOP Instance UID of the instance to add.
     */
    SIGHT_IO_DICOM_API static void add_instance(
        sight::data::series& _series,
        std::size_t _instance_number,
        const std::string& _sop_instance_uid
    );

    /**
     * @brief Check if an instance is available in the DICOM container of the series.
     * @param _series The series.
     * @param _instance_number The instance number to check.
     * @return true if the instance is available, false otherwise.
     */
    SIGHT_IO_DICOM_API static bool is_instance_available(
        const sight::data::series& _series,
        std::size_t _instance_number
    );
};

} // namespace sight::io::dicom::helper
