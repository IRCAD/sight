/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "vtkGdcmIO/config.hpp"

#include <gdcmScanner.h>

#include <filesystem>
#include <vector>

namespace vtkGdcmIO
{
namespace helper
{

/// Contains helpers to search dicom files on filesystem and get values on a gdcm scanner.
class VTKGDCMIO_CLASS_API GdcmHelper
{

public:

    /**
     * @brief Searchs recursively DICOM files on the filesystem.
     * @param[in] _dirPath the path of the DICOM folder.
     * @param[out] _dicomFiles the list of all DICOM files.
     */
    VTKGDCMIO_API static void searchRecursivelyFiles(const std::filesystem::path& _dirPath,
                                                     std::vector<std::string>& _dicomFiles);
    /**
     * @brief Gets a tag value in a DICOM file.
     * @param _scanner the gdcm scanner.
     * @param _dcmFile the main file.
     * @param _tag the tag to get.
     * @return The string representation of the requested tag.
     */
    VTKGDCMIO_CLASS_API static std::string getValue(::gdcm::Scanner& _scanner, const std::string& _dcmFile,
                                                    const ::gdcm::Tag& _tag);

private:

    static bool compare(std::string& _strOrgin, std::vector<std::string>* _vecStr);

};

} // namespace helper
} // namespace vtkGdcmIO
