/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwDcmtkIO/helper/DicomSearch.hpp"

#include <fwCore/base.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>

#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/ofstd/ofcond.h>

namespace fwDcmtkIO
{
namespace helper
{

//------------------------------------------------------------------------------

void DicomSearch::searchRecursively(const ::boost::filesystem::path& dirPath, std::vector<std::string>& dicomFiles)
{
    std::vector<std::string> extensions;
    std::string strIgnoreFile = ".zip|.txt|.htm|.html|.xml|.exe|.gz|.dir|.gif|.jpeg|.jpg|dicomdir|.DS_Store";
    ::boost::algorithm::split( extensions, strIgnoreFile, ::boost::algorithm::is_any_of("|"),
                               ::boost::algorithm::token_compress_on);

    std::string filePath;
    std::string filename;
    for( ::boost::filesystem::recursive_directory_iterator it(dirPath);
         it != ::boost::filesystem::recursive_directory_iterator(); ++it)
    {
        if(!::boost::filesystem::is_directory(*it))
        {
            filePath = it->path().string();
            filename = it->path().filename().string();

            if(!DicomSearch::checkFilenameExtension( filePath, &extensions) )
            {
                DcmFileFormat fileFormat;
                OFCondition ofCondition = fileFormat.loadFile(filePath.c_str());
                if(ofCondition.good())
                {
                    dicomFiles.push_back( filePath.c_str() );
                }
                else
                {
                    throw std::runtime_error("failed to read " + filename + "\n"
                                             + "dcmtk error: " + ofCondition.text());
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

bool DicomSearch::checkFilenameExtension(const std::string& filename, std::vector<std::string>* extensions)
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
