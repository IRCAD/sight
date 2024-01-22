/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "io/dicom/config.hpp"

#include <core/macros.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace sight::core::jobs
{

class observer;

} // namespace sight::core::jobs

namespace sight::io::dicom::helper
{

/**
 * @brief This class contains helpers to search dicom files on filesystem.
 */
class IO_DICOM_CLASS_API dicom_search
{
public:

    /**
     * @brief Search Dicom files recursively by excluding files with known extensions
     * @param[in] _dir_path Root directory
     * @param[out] _dicom_files Dicom files
     * @param[in] _check_is_dicom If set to true, each file is read to verify that
     * it is a DICOM file
     * @param[in] _file_lookup_observer lookup observer
     */
    IO_DICOM_API static void search_recursively(
        const std::filesystem::path& _dir_path,
        std::vector<std::filesystem::path>& _dicom_files,
        bool _check_is_dicom,
        const SPTR(core::jobs::observer)& _file_lookup_observer = nullptr
    );

protected:

    /**
     * @brief retrieve files according to extension.
     * @param[in] _dir_path Root directory
     * @param[out] _dicom_files Dicom files
     * @param[in] _file_lookup_observer lookup observer
     */
    static void check_filename_extension(
        const std::filesystem::path& _dir_path,
        std::vector<std::filesystem::path>& _dicom_files,
        const SPTR(core::jobs::observer)& _file_lookup_observer = nullptr
    );
};

} // namespace sight::io::dicom::helper
