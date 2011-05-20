/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
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

#include <fwCore/base.hpp>

#include "vtkGdcmIO/helper/GdcmHelper.hpp"

namespace vtkGdcmIO
{
namespace helper
{

void DicomSearch::searchRecursivelyFiles(const ::boost::filesystem::path &dirPath, std::vector<std::string>& dicomFiles)
{
    std::vector<std::string> vecStr;
    std::string strIgnoreFile = ".zip|.txt|.htm|.html|.xml|.exe|.gz|.dir|.gif|.jpeg|.jpg|dicomdir|.DS_Store";
    ::boost::algorithm::split( vecStr, strIgnoreFile, ::boost::algorithm::is_any_of("|"), ::boost::algorithm::token_compress_on );

    for( ::boost::filesystem::recursive_directory_iterator it(dirPath);
            it != ::boost::filesystem::recursive_directory_iterator(); ++it)
    {
        if(! ::boost::filesystem::is_directory(*it))
        {
            std::string sString = it->string();
            std::transform ( sString.begin(), sString.end(), sString.begin(), tolower );
            if(DicomSearch::compare( sString, &vecStr) )
            {
                try
                {
                    ::gdcm::Reader reader;
                    reader.SetFileName( it->string().c_str() );
                    if( !reader.Read() )
                    {
                        OSLM_WARN("Failed to read: " << it->string() );
                    }
                    else
                    {
                        dicomFiles.push_back( it->string().c_str() );
                    }
                }
                catch (std::exception& e)
                {
                    OSLM_ERROR ( "Try with another reader for this file : " << it->string().c_str());
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
        res = ! ::boost::ends_with(_strOrgin, vecStr->at(i));
    }
    return res;
}

//------------------------------------------------------------------------------

} //namespace helper
} //namespace vtkGdcmIO


