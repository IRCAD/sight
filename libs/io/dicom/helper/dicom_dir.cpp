/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "io/dicom/helper/dicom_dir.hpp"

#include "io/dicom/helper/dicom_data_reader.hxx"

#include <core/exceptionmacros.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>
#include <core/log/logger.hpp>
#include <core/spy_log.hpp>

#include <data/dicom_series.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <gdcmMediaStorage.h>
#include <gdcmReader.h>

#include <filesystem>

namespace sight::io::dicom::helper
{

// ----------------------------------------------------------------------------

std::filesystem::path dicom_dir::find_dicom_dir(const std::filesystem::path& _root)
{
    std::filesystem::path current = _root;

    while(std::filesystem::exists(current) && current != current.root_path())
    {
        std::filesystem::path dicom_dir_path = current / "dicomdir";
        if(std::filesystem::exists(dicom_dir_path) && !std::filesystem::is_directory(dicom_dir_path))
        {
            return dicom_dir_path;
        }

        dicom_dir_path = current / "DICOMDIR";
        if(std::filesystem::exists(dicom_dir_path) && !std::filesystem::is_directory(dicom_dir_path))
        {
            return dicom_dir_path;
        }

        current = current.parent_path();
    }

    return {};
}

// ----------------------------------------------------------------------------

void process_dir_information(
    const std::filesystem::path& _dicomdir,
    const std::filesystem::path& _root_dicom_dir_path,
    data::dicom_series::sptr _current_series,
    std::map<std::string, data::dicom_series::sptr>& _dicom_series_map,
    const core::log::logger::sptr& _logger,
    std::function<void(std::uint64_t)>& _progress,
    std::function<bool()>& _cancel,
    double& _p,
    double& _ptotal
)
{
    SIGHT_ASSERT(
        "You must specify a valid dicomdir.",
        std::filesystem::exists(_dicomdir)
        && !std::filesystem::is_directory(_dicomdir)
    );

    // Try to read the file
    gdcm::Reader reader;
    reader.SetFileName(_dicomdir.string().c_str());
    if(!reader.Read())
    {
        return;
    }

    const gdcm::File& gdcm_file  = reader.GetFile();
    const gdcm::DataSet& dataset = gdcm_file.GetDataSet();

    // Check if the file is a DICOMDIR
    gdcm::MediaStorage media_storage;
    media_storage.SetFromFile(gdcm_file);
    if(media_storage != gdcm::MediaStorage::MediaStorageDirectoryStorage)
    {
        SIGHT_ERROR("This file is not a DICOMDIR");
        return;
    }

    // Check the MediaStorageSOPClass
    const gdcm::FileMetaInformation& file_meta_information = gdcm_file.GetHeader();
    const std::string& media_storage_sop                   =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0002, 0x0002>(file_meta_information);

    if(media_storage_sop != gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::MediaStorageDirectoryStorage))
    {
        SIGHT_ERROR("This file is not a DICOMDIR");
        return;
    }

    // For each root elements
    for(const auto& it : dataset.GetDES())
    {
        // Directory Record Sequence
        if(it.GetTag() == gdcm::Tag(0x0004, 0x1220))
        {
            gdcm::SmartPointer<gdcm::SequenceOfItems> sequence = it.GetValueAsSQ();
            _ptotal += static_cast<double>(sequence->GetNumberOfItems());

            for(unsigned int index = 1 ; index <= sequence->GetNumberOfItems() ; ++index)
            {
                // Retrieve item
                gdcm::Item& item = sequence->GetItem(index);

                // Directory Record Type
                const std::string record_type =
                    io::dicom::helper::dicom_data_reader::get_tag_value<0x0004, 0x1430>(item.GetNestedDataSet());

                // Check Referenced File ID
                std::string ref_file_id =
                    io::dicom::helper::dicom_data_reader::get_tag_value<0x0004, 0x1500>(item.GetNestedDataSet());

                if(record_type == "IMAGE")
                {
                    // Read file path
                    std::string file = io::dicom::helper::dicom_data_reader::get_tag_value<0x0004, 0x1500>(
                        item.GetNestedDataSet()
                    );
                    std::replace(file.begin(), file.end(), '\\', '/');
                    SIGHT_WARN_IF("Dicom instance doesn't have a referenced file id.", file.empty());

                    const std::filesystem::path path = _root_dicom_dir_path / file;
                    SIGHT_WARN_IF("Unable to find path :" << path, !std::filesystem::exists(path));
                    SIGHT_WARN_IF("Dicomdir is badly formatted. Skipping path :" << path, !_current_series);

                    if(!_current_series || file.empty())
                    {
                        _logger->warning("DICOMDIR file is badly formatted. Instances may be missing");
                    }
                    else if(std::filesystem::exists(path))
                    {
                        auto instance_number = boost::lexical_cast<unsigned int>(
                            io::dicom::helper::dicom_data_reader::get_tag_value<0x0020,
                                                                                0x0013>(item.GetNestedDataSet())
                        );
                        _current_series->add_dicom_path(instance_number, path);
                    }
                    else
                    {
                        _logger->warning("Unable to retrieve file :" + path.string());
                    }
                }
                else
                {
                    // If the record is a series, we select the current series
                    if(record_type == "SERIES")
                    {
                        const std::string& series_uid =
                            io::dicom::helper::dicom_data_reader::get_tag_value<0x0020,
                                                                                0x000e>(item.GetNestedDataSet());
                        if(_dicom_series_map.find(series_uid) == _dicom_series_map.end())
                        {
                            data::dicom_series::sptr series = std::make_shared<data::dicom_series>();
                            series->set_series_instance_uid(series_uid);
                            _dicom_series_map[series_uid] = series;
                        }

                        _current_series = _dicom_series_map[series_uid];
                    }

                    std::replace(ref_file_id.begin(), ref_file_id.end(), '\\', '/');
                    auto ref_file_path = _dicomdir.parent_path() / ref_file_id;
                    if(!ref_file_id.empty() && std::filesystem::exists(ref_file_path))
                    {
                        process_dir_information(
                            ref_file_path,
                            _root_dicom_dir_path,
                            _current_series,
                            _dicom_series_map,
                            _logger,
                            _progress,
                            _cancel,
                            _p,
                            _ptotal
                        );
                    }
                }

                if(_progress)
                {
                    _progress(std::size_t(++_p));
                }

                if(_cancel && _cancel())
                {
                    break;
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------

void dicom_dir::retrieve_dicom_series(
    const std::filesystem::path& _dicomdir,
    std::vector<SPTR(data::dicom_series)>& _series_set,
    const core::log::logger::sptr& _logger,
    std::function<void(std::uint64_t)> _progress,
    std::function<bool()> _cancel
)
{
    SIGHT_ASSERT(
        "You must specify a valid dicomdir.",
        std::filesystem::exists(_dicomdir)
        && !std::filesystem::is_directory(_dicomdir)
    );

    // Try to read the file
    gdcm::Reader reader;
    reader.SetFileName(_dicomdir.string().c_str());
    if(!reader.Read())
    {
        return;
    }

    const gdcm::File& gdcm_file  = reader.GetFile();
    const gdcm::DataSet& dataset = gdcm_file.GetDataSet();

    // Check if the file is a DICOMDIR
    gdcm::MediaStorage media_storage;
    media_storage.SetFromFile(gdcm_file);
    if(media_storage != gdcm::MediaStorage::MediaStorageDirectoryStorage)
    {
        SIGHT_ERROR("This file is not a DICOMDIR");
        return;
    }

    // Check the MediaStorageSOPClass
    const gdcm::FileMetaInformation& file_meta_information = gdcm_file.GetHeader();
    const std::string& media_storage_sop                   =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0002, 0x0002>(file_meta_information);

    if(media_storage_sop != gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::MediaStorageDirectoryStorage))
    {
        SIGHT_ERROR("This file is not a DICOMDIR");
        return;
    }

    // For each root elements
    double p      = 0.;
    double ptotal = 0.;

    if(_progress)
    {
        // Compute total progress
        for(const auto& it : dataset.GetDES())
        {
            // Directory Record Sequence
            if(it.GetTag() == gdcm::Tag(0x0004, 0x1220))
            {
                gdcm::SmartPointer<gdcm::SequenceOfItems> sequence = it.GetValueAsSQ();

                ptotal += static_cast<double>(sequence->GetNumberOfItems());
            }
        }
    }

    if(0. == ptotal)
    {
        ptotal = 1.;
    }

    std::map<std::string, data::dicom_series::sptr> dicom_series_map;
    process_dir_information(
        _dicomdir,
        _dicomdir.parent_path(),
        nullptr,
        dicom_series_map,
        _logger,
        _progress,
        _cancel,
        p,
        ptotal
    );

    if(_cancel && _cancel())
    {
        return;
    }

    for(const auto& entry : dicom_series_map)
    {
        auto series            = entry.second;
        const std::size_t size = series->get_dicom_container().size();
        if(size != 0U)
        {
            series->set_number_of_instances(size);
            _series_set.push_back(series);
        }
        else
        {
            _logger->critical("Unable to retrieve instances for this series : " + series->get_series_instance_uid());
        }
    }
}

// ----------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
