/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwDcmtkIO/config.hpp"

#include <filesystem>

namespace fwDcmtkIO
{
namespace helper
{

/**
 * @brief   This class contains helpers to search dicom files on filesystem...
 */
class FWDCMTKIO_CLASS_API DicomSearch
{
public:
    /**
     * @brief Search Dicom files recursively
     * @param[in] dirPath Root directory
     * @param[out] dicomFiles Dicom files
     */
    FWDCMTKIO_API static void searchRecursively(const std::filesystem::path& dirPath,
                                                std::vector<std::string>& dicomFiles);

    /**
     * @brief Check if the file extension matches one of the forbidden extension.
     * @param[in] filename Filename that must be checked.
     * @param[in] extensions Extensions to check
     */
    FWDCMTKIO_API static bool checkFilenameExtension(const std::string& filename,
                                                     std::vector<std::string>* extensions);
};

} // namespace helper
} // namespace fwDcmtkIO
