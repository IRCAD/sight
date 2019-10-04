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

#include "vtkGdcmIO/helper/GdcmHelper.hpp"

#include <fwCore/base.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>

#include <gdcmAttribute.h>
#include <gdcmDirectory.h>
#include <gdcmFile.h>
#include <gdcmFilename.h>
#include <gdcmFilenameGenerator.h>
#include <gdcmImageHelper.h>
#include <gdcmIPPSorter.h>
#include <gdcmReader.h>
#include <gdcmScanner.h>
#include <gdcmSystem.h>
#include <gdcmTesting.h>
#include <gdcmTrace.h>

namespace vtkGdcmIO
{
namespace helper
{

//------------------------------------------------------------------------------

void DicomSearch::searchRecursivelyFiles(const std::filesystem::path& dirPath, std::vector<std::string>& dicomFiles)
{
    std::vector<std::string> vecStr;
    std::string strIgnoreFile = ".zip|.txt|.htm|.html|.xml|.exe|.gz|.dir|.gif|.jpeg|.jpg|dicomdir|.DS_Store";
    ::boost::algorithm::split( vecStr, strIgnoreFile, ::boost::algorithm::is_any_of(
                                   "|"), ::boost::algorithm::token_compress_on );

    std::string lowerFilename;
    std::string filename;
    for( std::filesystem::recursive_directory_iterator it(dirPath);
         it != std::filesystem::recursive_directory_iterator(); ++it)
    {
        if(!std::filesystem::is_directory(*it))
        {
            lowerFilename = filename = it->path().string();
            std::transform( lowerFilename.begin(), lowerFilename.end(), lowerFilename.begin(), tolower );
            if(DicomSearch::compare( lowerFilename, &vecStr) )
            {
                try
                {
                    ::gdcm::Reader reader;
                    reader.SetFileName( filename.c_str() );
                    if( !reader.CanRead() )// with GDCM2.0.18 use !reader.CanRead()
                    {
                        OSLM_WARN("Failed to read: " << filename );
                    }
                    else
                    {
                        dicomFiles.push_back( filename.c_str() );
                    }
                }
                catch (std::exception& e)
                {
                    OSLM_ERROR( "Try with another reader for this file : " << filename.c_str());
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

bool DicomSearch::compare(std::string& _strOrgin, std::vector<std::string>* vecStr)
{
    bool res = true;
    for (size_t i = 0; i < vecStr->size() && res; ++i)
    {
        res = !::boost::ends_with(_strOrgin, vecStr->at(i));
    }
    return res;
}

//------------------------------------------------------------------------------

} //namespace helper
} //namespace vtkGdcmIO
