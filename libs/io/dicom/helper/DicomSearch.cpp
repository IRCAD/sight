/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <boost/algorithm/string.hpp>

#include <array>
#include <fstream>

/**
 * Do not mark `DICM` as incorrect.
 * cspell:ignore DICM
 */
namespace sight::io::dicom::helper
{

//------------------------------------------------------------------------------

bool isDICOM(const std::filesystem::path& filepath)
{
    std::ifstream ifs(filepath, std::ios::binary);
    ifs.seekg(128);
    std::array<char, 5> DICOM {};
    ifs.read(DICOM.data(), 4);
    ifs.close();
    return strcmp(DICOM.data(), "DICM") == 0;
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
        for(const auto& file : fileVect)
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
            std::string ext = boost::to_lower_copy(path.extension().string());

            if(extensions.find(ext) == extensions.end())
            {
                std::string stem = boost::to_lower_copy(path.stem().string());

                if(stem != "dicomdir")
                {
                    dicomFiles.emplace_back(path.string());
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
