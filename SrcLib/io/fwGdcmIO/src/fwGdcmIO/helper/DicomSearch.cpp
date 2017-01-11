/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomSearch.hpp"

#include <fwCore/base.hpp>

#include <fwJobs/Observer.hpp>

#include <fwTools/System.hpp>

#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/regex.hpp>

#include <fstream>
#include <set>
#include <string>

namespace fwGdcmIO
{
namespace helper
{

bool isDICOM(const std::string& filepath)
{
    std::ifstream ifs( filepath, std::ios::binary );
    ifs.seekg(128);
    char DICM[5] = {0};
    ifs.read(DICM, 4);
    ifs.close();
    return strcmp(DICM, "DICM") == 0;
}

//------------------------------------------------------------------------------

void DicomSearch::searchRecursively(const ::boost::filesystem::path& dirPath,
                                    std::vector<std::string>& dicomFiles, bool checkIsDicom,
                                    const ::fwJobs::Observer::sptr& fileLookupObserver)
{
    std::vector<std::string> fileVect;
    checkFilenameExtension(dirPath, fileVect, fileLookupObserver);

    if(checkIsDicom)
    {
        if(fileLookupObserver)
        {
            fileLookupObserver->setTotalWorkUnits(fileVect.size());
        }

        std::uint64_t progress = 0;
        for(auto file: fileVect)
        {
            if(fileLookupObserver)
            {
                fileLookupObserver->doneWork(++progress);

                if(fileLookupObserver->cancelRequested())
                {
                    dicomFiles.clear();
                    break;
                }
            }

            bool isDicom = isDICOM(file);
            if(isDicom)
            {
                dicomFiles.push_back( file );
            }
            SLM_WARN_IF("Failed to read: " + file, !isDicom);
        }
    }
    else
    {
        dicomFiles = fileVect;
    }
}

//------------------------------------------------------------------------------

void DicomSearch::checkFilenameExtension(const ::boost::filesystem::path& dirPath,
                                         std::vector<std::string>& dicomFiles,
                                         const ::fwJobs::Observer::sptr& fileLookupObserver)
{
    dicomFiles.clear();

    std::set<std::string> extensions = ::boost::assign::list_of(".jpg")(".jpeg")(".htm")(".html")(".txt")(".xml")
                                           (".stm")(".str")(".lst")(".ifo")(".pdf")(".gif")
                                           (".png")(".exe")(".zip")(".gz")(".dir")(".dll")(".inf")
                                           (".DS_Store");

    for(::boost::filesystem::recursive_directory_iterator it(dirPath);
        it != ::boost::filesystem::recursive_directory_iterator(); ++it)
    {
        if(fileLookupObserver && fileLookupObserver->cancelRequested())
        {
            dicomFiles.clear();
            break;
        }

        if(!::boost::filesystem::is_directory(*it))
        {
            auto path       = it->path();
            std::string ext = path.extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

            if(extensions.find(ext) == extensions.end())
            {
                std::string stem = path.stem().string();
                std::transform(stem.begin(), stem.end(), stem.begin(), tolower);

                if(stem != "dicomdir")
                {
                    dicomFiles.push_back(path.string());
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} //namespace helper
} //namespace fwGdcmIO


