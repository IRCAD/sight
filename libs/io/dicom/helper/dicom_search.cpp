/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/dicom/helper/dicom_search.hpp"

#include <core/base.hpp>
#include <core/jobs/observer.hpp>

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

bool is_dicom(const std::filesystem::path& _filepath)
{
    std::ifstream ifs(_filepath, std::ios::binary);
    ifs.seekg(128);
    std::array<char, 5> dicom {};
    ifs.read(dicom.data(), 4);
    ifs.close();
    return strcmp(dicom.data(), "DICM") == 0;
}

//------------------------------------------------------------------------------

void dicom_search::search_recursively(
    const std::filesystem::path& _dir_path,
    std::vector<std::filesystem::path>& _dicom_files,
    bool _check_is_dicom,
    const core::jobs::observer::sptr& _reader_observer
)
{
    std::vector<std::filesystem::path> file_vect;
    check_filename_extension(_dir_path, file_vect, _reader_observer);

    if(_check_is_dicom)
    {
        if(_reader_observer)
        {
            _reader_observer->set_total_work_units(file_vect.size());
        }

        std::uint64_t progress = 0;
        for(const auto& file : file_vect)
        {
            if(_reader_observer)
            {
                _reader_observer->done_work(++progress);

                if(_reader_observer->cancel_requested())
                {
                    _dicom_files.clear();
                    break;
                }
            }

            if(is_dicom(file))
            {
                _dicom_files.push_back(file);
            }
            else
            {
                SIGHT_WARN("Failed to read: " + file.string());
            }
        }
    }
    else
    {
        _dicom_files = file_vect;
    }
}

//------------------------------------------------------------------------------

void dicom_search::check_filename_extension(
    const std::filesystem::path& _dir_path,
    std::vector<std::filesystem::path>& _dicom_files,
    const core::jobs::observer::sptr& _file_lookup_observer
)
{
    _dicom_files.clear();

    std::set<std::string> extensions = {".jpg", ".jpeg", ".htm", ".html", ".txt", ".xml",
                                        ".stm", ".str", ".lst", ".ifo", ".pdf", ".gif",
                                        ".png", ".exe", ".zip", ".gz", ".dir", ".dll", ".inf",
                                        ".DS_Store"
    };

    for(std::filesystem::recursive_directory_iterator it(_dir_path) ;
        it != std::filesystem::recursive_directory_iterator() ; ++it)
    {
        if(_file_lookup_observer && _file_lookup_observer->cancel_requested())
        {
            _dicom_files.clear();
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
                    _dicom_files.emplace_back(path.string());
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
