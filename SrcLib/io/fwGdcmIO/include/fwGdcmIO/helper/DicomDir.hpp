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

#include "fwGdcmIO/config.hpp"

#include <fwCore/macros.hpp>

#include <filesystem>

#include <cstdint>

namespace fwMedData
{
class DicomSeries;
}

namespace fwJobs
{
class Observer;
}

namespace fwLog
{
class Logger;
}

namespace fwGdcmIO
{
namespace helper
{

/**
 * @brief DicomDir Helper. This class is used to extract a list of files from a dicomdir file.
 */
class FWGDCMIO_CLASS_API DicomDir
{
public:
    /**
     * @brief Find the DICOMDIR file in the parent arborescence
     * @return Path to the DICOMDIR or empty path if the DICOMDIR has not been found
     */
    FWGDCMIO_API static std::filesystem::path findDicomDir(const std::filesystem::path& root);

    /**
     * @brief Create DicomSeries from information stored in DICOMDIR.
     * @param dicomdir Path to dicomdir file
     * @param[out] seriesDB Dicom series created
     * @param[in] fileLookupObserver file lookup observer
     */
    FWGDCMIO_API static void retrieveDicomSeries(const std::filesystem::path& dicomdir,
                                                 std::vector< SPTR(::fwMedData::DicomSeries) >& seriesDB,
                                                 const SPTR(::fwLog::Logger)& logger,
                                                 std::function< void(std::uint64_t) > progress = nullptr,
                                                 std::function< bool() > cancel                = nullptr);

};

} //helper
} //fwGdcmIO
