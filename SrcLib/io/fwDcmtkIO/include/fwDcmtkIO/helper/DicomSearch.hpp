/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDCMTKIO_HELPER_DICOMSEARCH_HPP__
#define __FWDCMTKIO_HELPER_DICOMSEARCH_HPP__

#include "fwDcmtkIO/config.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

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
    FWDCMTKIO_API static void searchRecursively(const ::boost::filesystem::path& dirPath,
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

#endif /*__FWDCMTKIO_HELPER_DICOMSEARCH_HPP__*/
