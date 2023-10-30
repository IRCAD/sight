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

#include "io/dicom/helper/DicomSeries.hpp"

#include "io/dicom/helper/DicomDir.hpp"

#include <core/exceptionmacros.hpp>
#include <core/jobs/aggregator.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/job.hpp>
#include <core/jobs/observer.hpp>
#include <core/spy_log.hpp>

#include <data/dicom_series.hpp>

#include <boost/algorithm/string.hpp>

#include <gdcmMediaStorage.h>
#include <gdcmReader.h>

#include <algorithm>
#include <filesystem>

namespace sight::io::dicom::helper
{

// Series
static const gdcm::Tag MEDIA_STORAGESOP_CLASS_UID(0x0002, 0x0002);
static const gdcm::Tag SPECIFIC_CHARACTER_SET_TAG(0x0008, 0x0005);
static const gdcm::Tag SERIES_INSTANCE_UID_TAG(0x0020, 0x000e);
static const gdcm::Tag SERIES_DATE_TAG(0x0008, 0x0021);
static const gdcm::Tag SERIES_TIME_TAG(0x0008, 0x0031);
static const gdcm::Tag MODALITY_TAG(0x0008, 0x0060);
static const gdcm::Tag SERIES_DESCRIPTION_TAG(0x0008, 0x103e);
static const gdcm::Tag PERFORMING_PHYSICIAN_NAME_TAG(0x0008, 0x1050);
static const gdcm::Tag SOP_CLASS_UID_TAG(0x0008, 0x0016);
static const gdcm::Tag SOP_INSTANCE_UID_TAG(0x0008, 0x0018);

// Equipment
static const gdcm::Tag INSTITUTION_NAME_TAG(0x0008, 0x0080);

// Patient
static const gdcm::Tag PATIENT_NAME_TAG(0x0010, 0x0010);
static const gdcm::Tag PATIENT_ID_TAG(0x0010, 0x0020);
static const gdcm::Tag PATIENT_BIRTH_DATE_TAG(0x0010, 0x0030);
static const gdcm::Tag PATIENT_SEX_TAG(0x0010, 0x0040);

// Study
static const gdcm::Tag STUDY_INSTANCE_UID_TAG(0x0020, 0x000d);
static const gdcm::Tag STUDY_DATE_TAG(0x0008, 0x0020);
static const gdcm::Tag STUDY_TIME_TAG(0x0008, 0x0030);
static const gdcm::Tag REFERRING_PHYSICIAN_NAME_TAG(0x0008, 0x0090);
static const gdcm::Tag STUDY_DESCRIPTION_TAG(0x0008, 0x1030);
static const gdcm::Tag PATIENT_AGE_TAG(0x0010, 0x1010);

//------------------------------------------------------------------------------

std::string get_string_value(
    const gdcm::Scanner& _scanner,
    const std::string& _filename,
    const gdcm::Tag& _tag
)
{
    std::string result;
    const char* value = _scanner.GetValue(_filename.c_str(), _tag);
    if(value != nullptr)
    {
        // Trim buffer
        result = gdcm::LOComp::Trim(value);
    }

    return result;
}

//------------------------------------------------------------------------------

std::string get_string_value(
    const gdcm::DataSet& _dataset,
    const gdcm::Tag& _tag
)
{
    std::string result;
    if(_dataset.FindDataElement(_tag))
    {
        const gdcm::DataElement& data_element = _dataset.GetDataElement(_tag);

        if(!data_element.IsEmpty())
        {
            // Retrieve buffer
            const gdcm::ByteValue* bv = data_element.GetByteValue();
            if(bv != nullptr)
            {
                std::string buffer(bv->GetPointer(), bv->GetLength());

                // Trim buffer
                result = gdcm::LOComp::Trim(buffer.c_str());
            }
        }
    }

    return result;
}

// ----------------------------------------------------------------------------

dicom_series::dicom_series_container_t dicom_series::read(
    filename_container_t& _filenames,
    const SPTR(core::jobs::observer)& _reader_observer,
    const SPTR(core::jobs::observer)& _complete_series_observer
)
{
    dicom_series_container_t series_container = dicom_series::split_files(_filenames, _reader_observer);
    dicom_series::fill_series(series_container, _complete_series_observer);
    return series_container;
}

//------------------------------------------------------------------------------

void dicom_series::complete(
    dicom_series_container_t& _series_container,
    const SPTR(core::jobs::observer)& _complete_series_observer
)
{
    std::set<gdcm::Tag> selectedtags;
    selectedtags.insert(SPECIFIC_CHARACTER_SET_TAG);
    selectedtags.insert(SERIES_INSTANCE_UID_TAG);
    selectedtags.insert(MODALITY_TAG);
    selectedtags.insert(SERIES_DATE_TAG);
    selectedtags.insert(SERIES_TIME_TAG);
    selectedtags.insert(SERIES_DESCRIPTION_TAG);
    selectedtags.insert(PERFORMING_PHYSICIAN_NAME_TAG);
    selectedtags.insert(SOP_CLASS_UID_TAG);
    selectedtags.insert(SOP_INSTANCE_UID_TAG);

    for(const auto& series : _series_container)
    {
        if(series->get_dicom_container().empty())
        {
            SIGHT_ERROR("DicomSeries doesn't not contain any instance.");
            break;
        }

        const auto& first_item                                      = series->get_dicom_container().begin();
        const core::memory::buffer_object::sptr buffer_obj          = first_item->second;
        const core::memory::buffer_manager::stream_info stream_info = buffer_obj->get_stream_info();
        SPTR(std::istream) is = stream_info.stream;

        gdcm::Reader reader;
        reader.SetStream(*is);

        if(!reader.ReadSelectedTags(selectedtags))
        {
            SIGHT_THROW(
                "Unable to read Dicom file '" << buffer_obj->get_stream_info().fs_file.string() << "' "
                << "(slice: '" << first_item->first << "')"
            );
        }

        const gdcm::DataSet& dataset = reader.GetFile().GetDataSet();

        //Modality
        std::string modality = get_string_value(dataset, MODALITY_TAG);
        series->set_modality(modality);

        //Date
        std::string series_date = get_string_value(dataset, SERIES_DATE_TAG);
        series->set_series_date(series_date);

        //Time
        std::string series_time = get_string_value(dataset, SERIES_TIME_TAG);
        series->set_series_time(series_time);

        //Description
        std::string series_description = get_string_value(dataset, SERIES_DESCRIPTION_TAG);
        series->set_series_description(series_description);

        //Performing Physicians Name
        std::string performing_physician_name = get_string_value(dataset, PERFORMING_PHYSICIAN_NAME_TAG);
        series->set_performing_physician_name(performing_physician_name);

        // Add the sop_classUID to the series
        const std::string& sop_class_uid = get_string_value(dataset, SOP_CLASS_UID_TAG);
        series->get_sop_class_ui_ds().insert(sop_class_uid);
    }

    dicom_series::fill_series(_series_container, _complete_series_observer);
}

//------------------------------------------------------------------------------

dicom_series::dicom_series_container_t dicom_series::split_files(
    filename_container_t& _filenames,
    const core::jobs::observer::sptr& _reader_observer
)
{
    gdcm::Scanner series_scanner;
    series_scanner.AddTag(SPECIFIC_CHARACTER_SET_TAG);
    series_scanner.AddTag(SERIES_INSTANCE_UID_TAG);
    series_scanner.AddTag(MODALITY_TAG);
    series_scanner.AddTag(SERIES_DATE_TAG);
    series_scanner.AddTag(SERIES_TIME_TAG);
    series_scanner.AddTag(SERIES_DESCRIPTION_TAG);
    series_scanner.AddTag(PERFORMING_PHYSICIAN_NAME_TAG);
    series_scanner.AddTag(SOP_CLASS_UID_TAG);
    series_scanner.AddTag(SOP_INSTANCE_UID_TAG);
    series_scanner.AddTag(MEDIA_STORAGESOP_CLASS_UID);
    _reader_observer->set_total_work_units(_filenames.size());
    _reader_observer->done_work(0);

    std::vector<std::string> file_vec;
    for(const auto& file : _filenames)
    {
        file_vec.push_back(file.string());
    }

    bool status = series_scanner.Scan(file_vec);
    SIGHT_THROW_IF("Unable to read the files.", !status);

    gdcm::Directory::FilenamesType keys = series_scanner.GetKeys();

    unsigned int progress = 0;

    dicom_series_container_t series_container;

    //Loop through every files available in the scanner
    std::map<std::string, std::filesystem::path> ordered_filenames;
    for(const std::filesystem::path& dicom_file : _filenames)
    {
        ordered_filenames[dicom_file.filename().string()] = dicom_file;
    }

    std::set<std::string> previous_sop_instance_ui_ds;

    for(const auto& dicom_file : ordered_filenames)
    {
        auto filename = dicom_file.second.string();

        SIGHT_ASSERT(
            "The file \"" << dicom_file.second << "\" is not a key of the gdcm scanner",
            series_scanner.IsKey(filename.c_str())
        );

        const std::string& sop_instance_uid = get_string_value(series_scanner, filename, SOP_INSTANCE_UID_TAG);

        if(previous_sop_instance_ui_ds.find(sop_instance_uid) != previous_sop_instance_ui_ds.end())
        {
            SIGHT_WARN(
                "The instance "
                << sop_instance_uid
                << " has already been read, which usually means DICOM files are corrupted."
            );

            continue;
        }

        const std::string& sop_class_uid =
            get_string_value(series_scanner, filename, SOP_CLASS_UID_TAG);
        const std::string& media_storage_sop_class_uid =
            get_string_value(series_scanner, filename, MEDIA_STORAGESOP_CLASS_UID);

        if(sop_class_uid != gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::MediaStorageDirectoryStorage)
           && media_storage_sop_class_uid
           != gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::MediaStorageDirectoryStorage))
        {
            sight::io::dicom::helper::dicom_series::create_series(series_container, series_scanner, dicom_file.second);
            previous_sop_instance_ui_ds.insert(sop_instance_uid);
        }

        if(!_reader_observer || _reader_observer->cancel_requested())
        {
            break;
        }

        _reader_observer->done_work(static_cast<std::uint64_t>(++progress * 100LL / keys.size()));
    }

    return series_container;
}

//------------------------------------------------------------------------------

void dicom_series::fill_series(
    dicom_series_container_t& _series_container,
    const core::jobs::observer::sptr& _complete_series_observer
)
{
    std::set<gdcm::Tag> selectedtags;
    selectedtags.insert(SPECIFIC_CHARACTER_SET_TAG);
    selectedtags.insert(PATIENT_ID_TAG);
    selectedtags.insert(PATIENT_NAME_TAG);
    selectedtags.insert(PATIENT_BIRTH_DATE_TAG);
    selectedtags.insert(PATIENT_SEX_TAG);
    selectedtags.insert(STUDY_INSTANCE_UID_TAG);
    selectedtags.insert(STUDY_DATE_TAG);
    selectedtags.insert(STUDY_TIME_TAG);
    selectedtags.insert(REFERRING_PHYSICIAN_NAME_TAG);
    selectedtags.insert(STUDY_DESCRIPTION_TAG);
    selectedtags.insert(PATIENT_AGE_TAG);
    selectedtags.insert(INSTITUTION_NAME_TAG);
    selectedtags.insert(SERIES_INSTANCE_UID_TAG);

    std::uint64_t progress = 0;

    // Fill series
    for(const data::dicom_series::sptr& series : _series_container)
    {
        // Compute number of instances
        const std::size_t size = series->get_dicom_container().size();
        series->set_number_of_instances(size);

        if(size == 0U)
        {
            SIGHT_ERROR("The DicomSeries doesn't contain any instance.");
            break;
        }

        // Load first instance
        const auto& first_item                                      = series->get_dicom_container().begin();
        const core::memory::buffer_object::sptr buffer_obj          = first_item->second;
        const core::memory::buffer_manager::stream_info stream_info = buffer_obj->get_stream_info();
        SPTR(std::istream) is = stream_info.stream;

        gdcm::Reader reader;
        reader.SetStream(*is);

        if(!reader.ReadSelectedTags(selectedtags))
        {
            SIGHT_THROW(
                "Unable to read Dicom file '" << buffer_obj->get_stream_info().fs_file.string() << "' "
                << "(slice: '" << first_item->first << "')"
            );
        }

        const gdcm::DataSet& dataset = reader.GetFile().GetDataSet();

        // Fill series
        // Equipment Module
        const auto& institution_name = get_string_value(dataset, INSTITUTION_NAME_TAG);
        series->set_institution_name(institution_name);

        // Patient Module
        const auto& patient_id = get_string_value(dataset, PATIENT_ID_TAG);
        series->set_patient_id(patient_id);

        const auto& patient_name = get_string_value(dataset, PATIENT_NAME_TAG);
        series->set_patient_name(patient_name);

        const auto& patient_birth_date = get_string_value(dataset, PATIENT_BIRTH_DATE_TAG);
        series->set_patient_birth_date(patient_birth_date);

        const auto& patient_sex = get_string_value(dataset, PATIENT_SEX_TAG);
        series->set_patient_sex(patient_sex);

        // Study Module
        const auto& study_instance_uid = get_string_value(dataset, STUDY_INSTANCE_UID_TAG);
        series->set_study_instance_uid(study_instance_uid);

        //Study Date
        const auto& study_date = get_string_value(dataset, STUDY_DATE_TAG);
        series->set_study_date(study_date);

        //Study Time
        const auto& study_time = get_string_value(dataset, STUDY_TIME_TAG);
        series->set_study_time(study_time);

        //Referring Physician Name
        const auto& referring_physician_name = get_string_value(dataset, REFERRING_PHYSICIAN_NAME_TAG);
        series->set_referring_physician_name(referring_physician_name);

        //Study Description
        const auto& study_description = get_string_value(dataset, STUDY_DESCRIPTION_TAG);
        series->set_study_description(study_description);

        //Study Patient Age
        const auto& patient_age = get_string_value(dataset, PATIENT_AGE_TAG);
        series->set_patient_age(patient_age);

        if(_complete_series_observer)
        {
            _complete_series_observer->done_work(
                static_cast<std::uint64_t>(++progress * 100
                                           / _series_container.size())
            );

            if(_complete_series_observer->cancel_requested())
            {
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------

void dicom_series::create_series(
    dicom_series_container_t& _series_container,
    const gdcm::Scanner& _scanner,
    const std::filesystem::path& _filename
)
{
    data::dicom_series::sptr series = data::dicom_series::sptr();

    const std::string string_filename = _filename.string();

    // Get Series Instance UID
    std::string series_instance_uid = get_string_value(_scanner, string_filename, SERIES_INSTANCE_UID_TAG);

    // Check if the series already exists
    for(const auto& dicom_series : _series_container)
    {
        if(dicom_series->get_series_instance_uid() == series_instance_uid)
        {
            series = dicom_series;
            break;
        }
    }

    // If the series doesn't exist we create it
    if(!series)
    {
        series = std::make_shared<data::dicom_series>();

        _series_container.push_back(series);

        //Instance UID
        series->set_series_instance_uid(series_instance_uid);

        //Modality
        std::string modality = get_string_value(_scanner, string_filename, MODALITY_TAG);
        series->set_modality(modality);

        //Date
        std::string series_date = get_string_value(_scanner, string_filename, SERIES_DATE_TAG);
        series->set_series_date(series_date);

        //Time
        std::string series_time = get_string_value(_scanner, string_filename, SERIES_TIME_TAG);
        series->set_series_time(series_time);

        //Description
        std::string series_description = get_string_value(_scanner, string_filename, SERIES_DESCRIPTION_TAG);
        series->set_series_description(series_description);

        //Performing Physicians Name
        std::string performing_physician_name =
            get_string_value(_scanner, string_filename, PERFORMING_PHYSICIAN_NAME_TAG);
        series->set_performing_physician_name(performing_physician_name);
    }

    // Add the sop_classUID to the series
    const std::string& sop_class_uid = get_string_value(
        _scanner,
        string_filename,
        SOP_CLASS_UID_TAG
    );

    series->get_sop_class_ui_ds().insert(sop_class_uid);
    series->add_dicom_path(series->get_dicom_container().size(), _filename);
}

} // namespace sight::io::dicom::helper
