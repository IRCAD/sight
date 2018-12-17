/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWGDCMIO_HELPER_DICOMSEARCH_HPP__
#define __FWGDCMIO_HELPER_DICOMSEARCH_HPP__

#include "fwGdcmIO/config.hpp"

#include <fwCore/macros.hpp>

#include <boost/filesystem.hpp>

#include <string>
#include <vector>

namespace fwJobs
{
class Observer;
}

namespace fwGdcmIO
{
namespace helper
{

/**
 * @brief This class contains helpers to search dicom files on filesystem.
 */
class FWGDCMIO_CLASS_API DicomSearch
{
public:
    /**
     * @brief Search Dicom files recursively by excluding files with known extensions
     * @param[in] dirPath Root directory
     * @param[out] dicomFiles Dicom files
     * @param[in] checkIsDicom If set to true, each file is read to verify that
     * it is a DICOM file
     * @param[in] fileLookupObserver lookup observer
     */
    FWGDCMIO_API static void searchRecursively(const ::boost::filesystem::path& dirPath,
                                               std::vector< ::boost::filesystem::path >& dicomFiles,
                                               bool checkIsDicom,
                                               const SPTR(::fwJobs::Observer)& fileLookupObserver = nullptr);

protected:
    /**
     * @brief retrieve files according to extension.
     * @param[in] dirPath Root directory
     * @param[out] dicomFiles Dicom files
     * @param[in] fileLookupObserver lookup observer
     */
    static void checkFilenameExtension(const ::boost::filesystem::path& dirPath,
                                       std::vector< ::boost::filesystem::path >& dicomFiles,
                                       const SPTR(::fwJobs::Observer)& fileLookupObserver = nullptr);
};

} // namespace helper
} // namespace fwGdcmIO

#endif /*__FWGDCMIO_HELPER_DICOMSEARCH_HPP__*/
