/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_HELPER_DICOMSEARCH_HPP__
#define __FWGDCMIO_HELPER_DICOMSEARCH_HPP__

#include "fwGdcmIO/config.hpp"

#include <fwCore/macros.hpp>

#include <boost/filesystem/path.hpp>

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
