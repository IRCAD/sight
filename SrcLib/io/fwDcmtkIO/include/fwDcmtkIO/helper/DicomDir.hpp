/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDCMTKIO_HELPER_DICOMDIR_HPP__
#define __FWDCMTKIO_HELPER_DICOMDIR_HPP__

#include "fwDcmtkIO/config.hpp"

#include <boost/filesystem/path.hpp>

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
    FWDCMTKIO_API static bool readDicomDir(const ::boost::filesystem::path& root,
                                           std::vector<std::string>& dicomFiles);

private:
    /// Create a regex used to find the correct case of a filename as Dicom only store uppercase paths.
    FWDCMTKIO_API static std::string createRegex(std::string filename);

    /// Return the filename with the correct case.
    FWDCMTKIO_API static ::boost::filesystem::path getRealFilename(
        const ::boost::filesystem::path& root, const std::string& filename);

};

} //helper
} //fwDcmtkIO


#endif /* __FWDCMTKIO_HELPER_DICOMDIR_HPP__ */
