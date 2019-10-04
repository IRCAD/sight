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
 * @brief DicomDir Helper. This class is used to extract a list of files from a dicomdir file.
 */
class FWDCMTKIO_CLASS_API DicomDir
{
public:
    /// Find Dicom instances in a DicomDir file.
    FWDCMTKIO_API static bool readDicomDir(const std::filesystem::path& root,
                                           std::vector<std::string>& dicomFiles);

private:
    /// Create a regex used to find the correct case of a filename as Dicom only store uppercase paths.
    FWDCMTKIO_API static std::string createRegex(std::string filename);

    /// Return the filename with the correct case.
    FWDCMTKIO_API static std::filesystem::path getRealFilename(
        const std::filesystem::path& root, const std::string& filename);

};

} //helper
} //fwDcmtkIO
