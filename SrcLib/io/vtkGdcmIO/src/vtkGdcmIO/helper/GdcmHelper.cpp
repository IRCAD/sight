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

#include "vtkGdcmIO/helper/GdcmHelper.hpp"

#include <fwCore/base.hpp>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <gdcmImageHelper.h>
#include <gdcmIPPSorter.h>
#include <gdcmFilename.h>
#include <gdcmTesting.h>
#include <gdcmSystem.h>
#include <gdcmTrace.h>
#include <gdcmDirectory.h>
#include <gdcmScanner.h>
#include <gdcmReader.h>
#include <gdcmIPPSorter.h>
#include <gdcmFilenameGenerator.h>
#include <gdcmAttribute.h>
#include <gdcmFile.h>

namespace vtkGdcmIO
{
namespace helper
{

void DicomSearch::searchRecursivelyFiles(const ::boost::filesystem::path &dirPath, std::vector<std::string>& dicomFiles)
{
    std::vector<std::string> vecStr;
    std::string strIgnoreFile = ".zip|.txt|.htm|.html|.xml|.exe|.gz|.dir|.gif|.jpeg|.jpg|dicomdir|.DS_Store";
    ::boost::algorithm::split( vecStr, strIgnoreFile, ::boost::algorithm::is_any_of(
                                   "|"), ::boost::algorithm::token_compress_on );

    std::string lowerFilename;
    std::string filename;
    for( ::boost::filesystem::recursive_directory_iterator it(dirPath);
         it != ::boost::filesystem::recursive_directory_iterator(); ++it)
    {
        if(!::boost::filesystem::is_directory(*it))
        {
            lowerFilename = filename = it->path().string();
            std::transform ( lowerFilename.begin(), lowerFilename.end(), lowerFilename.begin(), tolower );
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
                    OSLM_ERROR ( "Try with another reader for this file : " << filename.c_str());
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

bool DicomSearch::compare(std::string & _strOrgin, std::vector<std::string> * vecStr)
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


