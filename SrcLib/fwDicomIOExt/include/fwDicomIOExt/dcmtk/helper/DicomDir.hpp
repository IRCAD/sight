/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_DCMTK_HELPER_DICOMDIR_HPP__
#define __FWDICOMIOEXT_DCMTK_HELPER_DICOMDIR_HPP__

#include <boost/filesystem/path.hpp>

#include "fwDicomIOExt/config.hpp"

namespace fwDicomIOExt
{
namespace dcmtk
{
namespace helper
{

/**
 * @class DicomDir
 * @brief DicomDir Helper. This class is used to extract a list of files from a dicomdir file.
 * @date  2013.
 */
class FWDICOMIOEXT_CLASS_API DicomDir
{
public:
    /// Find Dicom instances in a DicomDir file.
    FWDICOMIOEXT_API static bool readDicomDir(const ::boost::filesystem::path &root,
                                              std::vector<std::string>& dicomFiles);

private:
    /// Create a regex used to find the correct case of a filename as Dicom only store uppercase paths.
    FWDICOMIOEXT_API static std::string createRegex(std::string filename);

    /// Return the filename with the correct case.
    FWDICOMIOEXT_API static ::boost::filesystem::path getRealFilename(
        const ::boost::filesystem::path &root, const std::string &filename);

};

} //dcmtk
} //helper
} //fwDicomIOExt


#endif /* __FWDICOMIOEXT_DCMTK_HELPER_DICOMDIR_HPP__ */
