/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKGDCMIO_GDCMHELPER_HPP_
#define _VTKGDCMIO_GDCMHELPER_HPP_

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include "vtkGdcmIO/config.hpp"

namespace vtkGdcmIO
{
namespace helper
{

/**
 * @brief   This class contains helpers to search dicom files on filesystem...
 * @class   DicomSearch
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class VTKGDCMIO_CLASS_API DicomSearch
{
public:
    VTKGDCMIO_API static void searchRecursivelyFiles(const ::boost::filesystem::path &dirPath, std::vector<std::string>& dicomFiles);
    static bool compare(std::string & _strOrgin, std::vector<std::string> * vecStr);
};

} // namespace helper
} // namespace vtkGdcmIO

#endif /*_VTKGDCMIO_GDCMHELPER_HPP_*/
