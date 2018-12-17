/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __VTKGDCMIO_HELPER_GDCMHELPER_HPP__
#define __VTKGDCMIO_HELPER_GDCMHELPER_HPP__

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
 *
 * @date    2011.
 */
class VTKGDCMIO_CLASS_API DicomSearch
{
public:
    VTKGDCMIO_API static void searchRecursivelyFiles(const ::boost::filesystem::path &dirPath,
                                                     std::vector<std::string>& dicomFiles);
    static bool compare(std::string & _strOrgin, std::vector<std::string> * vecStr);
};

} // namespace helper
} // namespace vtkGdcmIO

#endif /*__VTKGDCMIO_HELPER_GDCMHELPER_HPP__*/
