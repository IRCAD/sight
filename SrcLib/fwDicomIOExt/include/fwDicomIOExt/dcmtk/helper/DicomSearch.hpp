/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_DCMTK_HELPER_DICOMSEARCH_HPP__
#define __FWDICOMIOEXT_DCMTK_HELPER_DICOMSEARCH_HPP__

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include "fwDicomIOExt/config.hpp"

namespace fwDicomIOExt
{
namespace dcmtk
{
namespace helper
{

/**
 * @brief   This class contains helpers to search dicom files on filesystem...
 * @class   DicomSearch
 * @date    2013.
 */
class FWDICOMIOEXT_CLASS_API DicomSearch
{
public:
    /**
     * @brief Search Dicom files recursively
     * @param[in] dirPath Root directory
     * @param[out] dicomFiles Dicom files
     */
    FWDICOMIOEXT_API static void searchRecursively(const ::boost::filesystem::path &dirPath,
            std::vector<std::string>& dicomFiles);

    /**
     * @brief Check if the file extension matches one of the forbidden extension.
     * @param[in] filename Filename that must be checked.
     * @param[in] extensions Extensions to check
     */
    FWDICOMIOEXT_API static bool checkFilenameExtension(const std::string &filename, std::vector<std::string> *extensions);
};

} //dcmtk
} // namespace helper
} // namespace fwDicomIOExt

#endif /*__FWDICOMIOEXT_DCMTK_HELPER_DICOMSEARCH_HPP__*/
