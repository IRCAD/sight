/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDcmtkIO/helper/DicomSearch.hpp"

#include <fwCore/base.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>

#include <dcmtk/dcmdata/dcfilefo.h>

namespace fwDcmtkIO
{
namespace helper
{

void DicomSearch::searchRecursively(const ::boost::filesystem::path &dirPath, std::vector<std::string>& dicomFiles)
{
    std::vector<std::string> extensions;
    std::string strIgnoreFile = ".zip|.txt|.htm|.html|.xml|.exe|.gz|.dir|.gif|.jpeg|.jpg|dicomdir|.DS_Store";
    ::boost::algorithm::split( extensions, strIgnoreFile, ::boost::algorithm::is_any_of("|"),
                               ::boost::algorithm::token_compress_on);

    std::string lowerFilename;
    std::string filename;
    DcmFileFormat fileFormat;
    for( ::boost::filesystem::recursive_directory_iterator it(dirPath);
         it != ::boost::filesystem::recursive_directory_iterator(); ++it)
    {
        if(!::boost::filesystem::is_directory(*it))
        {
#if BOOST_FILESYSTEM_VERSION > 2
            lowerFilename = filename = it->path().string();
#else
            lowerFilename = filename = it->string();
#endif
            std::transform ( lowerFilename.begin(), lowerFilename.end(), lowerFilename.begin(), tolower );
            if(!DicomSearch::checkFilenameExtension( lowerFilename, &extensions) )
            {
                try
                {
                    if(fileFormat.loadFile(filename.c_str()).good())
                    {
                        dicomFiles.push_back( filename.c_str() );
                    }
                    else
                    {
                        SLM_WARN("Failed to read: " + filename );
                    }
                }
                catch (...)
                {
                    OSLM_ERROR("Try with another reader for this file : " << filename.c_str());
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

bool DicomSearch::checkFilenameExtension(const std::string &filename, std::vector<std::string> *extensions)
{
    bool result = false;
    for (size_t i = 0; i < extensions->size() && !result; ++i)
    {
        result = ::boost::ends_with(filename, extensions->at(i));
    }
    return result;
}

//------------------------------------------------------------------------------

} //namespace helper
} //namespace fwDcmtkIO


