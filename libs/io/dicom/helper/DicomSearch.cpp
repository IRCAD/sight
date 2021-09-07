/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/dicom/helper/DicomSearch.hpp"

#include <core/base.hpp>
#include <core/jobs/Observer.hpp>

#include <fstream>

namespace sight::io::dicom
{

namespace helper
{

//------------------------------------------------------------------------------

bool isDICOM(const std::filesystem::path& filepath)
{
    std::ifstream ifs(filepath, std::ios::binary);
    ifs.seekg(128);
    char DICM[5] = {0};
    ifs.read(DICM, 4);
    ifs.close();
    return strcmp(DICM, "DICM") == 0;
}

//------------------------------------------------------------------------------

void DicomSearch::searchRecursively(
    const std::filesystem::path& dirPath,
    std::vector<std::filesystem::path>& dicomFiles,
    bool checkIsDicom,
    const core::jobs::Observer::sptr& readerObserver
)
{
    std::vector<std::filesystem::path> fileVect;
    checkFilenameExtension(dirPath, fileVect, readerObserver);

    if(checkIsDicom)
    {
        if(readerObserver)
        {
            readerObserver->setTotalWorkUnits(fileVect.size());
        }

        std::uint64_t progress = 0;
        for(auto file : fileVect)
        {
            if(readerObserver)
            {
                readerObserver->doneWork(++progress);

                if(readerObserver->cancelRequested())
                {
                    dicomFiles.clear();
                    break;
                }
            }

            bool isDicom = isDICOM(file);
            if(isDicom)
            {
                dicomFiles.push_back(file);
            }

            SIGHT_WARN_IF("Failed to read: " + file.string(), !isDicom);
        }
    }
    else
    {
        dicomFiles = fileVect;
    }
}

//------------------------------------------------------------------------------

void DicomSearch::checkFilenameExtension(
    const std::filesystem::path& dirPath,
    std::vector<std::filesystem::path>& dicomFiles,
    const core::jobs::Observer::sptr& fileLookupObserver
)
{
    dicomFiles.clear();

    std::set<std::string> extensions = {".jpg", ".jpeg", ".htm", ".html", ".txt", ".xml",
                                        ".stm", ".str", ".lst", ".ifo", ".pdf", ".gif",
                                        ".png", ".exe", ".zip", ".gz", ".dir", ".dll", ".inf",
                                        ".DS_Store"
    };

    for(std::filesystem::recursive_directory_iterator it(dirPath) ;
        it != std::filesystem::recursive_directory_iterator() ; ++it)
    {
        if(fileLookupObserver && fileLookupObserver->cancelRequested())
        {
            dicomFiles.clear();
            break;
        }

        if(!std::filesystem::is_directory(*it))
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

} //namespace sight::io::dicom
