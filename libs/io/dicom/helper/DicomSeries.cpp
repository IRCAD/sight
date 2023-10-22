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
static const gdcm::Tag s_MediaStoragesop_classUID(0x0002, 0x0002);
static const gdcm::Tag s_SpecificCharacterSetTag(0x0008, 0x0005);
static const gdcm::Tag s_SeriesInstanceUIDTag(0x0020, 0x000e);
static const gdcm::Tag s_SeriesDateTag(0x0008, 0x0021);
static const gdcm::Tag s_SeriesTimeTag(0x0008, 0x0031);
static const gdcm::Tag s_ModalityTag(0x0008, 0x0060);
static const gdcm::Tag s_SeriesDescriptionTag(0x0008, 0x103e);
static const gdcm::Tag s_PerformingPhysicianNameTag(0x0008, 0x1050);
static const gdcm::Tag s_sop_classUIDTag(0x0008, 0x0016);
static const gdcm::Tag s_SOPInstanceUIDTag(0x0008, 0x0018);

// Equipment
static const gdcm::Tag s_InstitutionNameTag(0x0008, 0x0080);

// Patient
static const gdcm::Tag s_PatientNameTag(0x0010, 0x0010);
static const gdcm::Tag s_PatientIDTag(0x0010, 0x0020);
static const gdcm::Tag s_PatientBirthDateTag(0x0010, 0x0030);
static const gdcm::Tag s_PatientSexTag(0x0010, 0x0040);

// Study
static const gdcm::Tag s_StudyInstanceUIDTag(0x0020, 0x000d);
static const gdcm::Tag s_StudyDateTag(0x0008, 0x0020);
static const gdcm::Tag s_StudyTimeTag(0x0008, 0x0030);
static const gdcm::Tag s_ReferringPhysicianNameTag(0x0008, 0x0090);
static const gdcm::Tag s_StudyDescriptionTag(0x0008, 0x1030);
static const gdcm::Tag s_PatientAgeTag(0x0010, 0x1010);

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

DicomSeries::dicom_series_container_t DicomSeries::read(
    filename_container_t& _filenames,
    const SPTR(core::jobs::observer)& _reader_observer,
    const SPTR(core::jobs::observer)& _complete_series_observer
)
{
    dicom_series_container_t series_container = DicomSeries::splitFiles(_filenames, _reader_observer);
    DicomSeries::fillSeries(series_container, _complete_series_observer);
    return series_container;
}

//------------------------------------------------------------------------------

void DicomSeries::complete(
    dicom_series_container_t& _series_container,
    const SPTR(core::jobs::observer)& _complete_series_observer
)
{
    std::set<gdcm::Tag> selectedtags;
    selectedtags.insert(s_SpecificCharacterSetTag);
    selectedtags.insert(s_SeriesInstanceUIDTag);
    selectedtags.insert(s_ModalityTag);
    selectedtags.insert(s_SeriesDateTag);
    selectedtags.insert(s_SeriesTimeTag);
    selectedtags.insert(s_SeriesDescriptionTag);
    selectedtags.insert(s_PerformingPhysicianNameTag);
    selectedtags.insert(s_sop_classUIDTag);
    selectedtags.insert(s_SOPInstanceUIDTag);

    for(const auto& series : _series_container)
    {
        if(series->getDicomContainer().empty())
        {
            SIGHT_ERROR("DicomSeries doesn't not contain any instance.");
            break;
        }

        const auto& first_item                                      = series->getDicomContainer().begin();
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
        std::string modality = get_string_value(dataset, s_ModalityTag);
        series->setModality(modality);

        //Date
        std::string series_date = get_string_value(dataset, s_SeriesDateTag);
        series->setSeriesDate(series_date);

        //Time
        std::string series_time = get_string_value(dataset, s_SeriesTimeTag);
        series->setSeriesTime(series_time);

        //Description
        std::string series_description = get_string_value(dataset, s_SeriesDescriptionTag);
        series->setSeriesDescription(series_description);

        //Performing Physicians Name
        std::string performing_physician_name = get_string_value(dataset, s_PerformingPhysicianNameTag);
        series->setPerformingPhysicianName(performing_physician_name);

        // Add the sop_classUID to the series
        const std::string& sop_class_uid = get_string_value(dataset, s_sop_classUIDTag);
        series->getSOPClassUIDs().insert(sop_class_uid);
    }

    DicomSeries::fillSeries(_series_container, _complete_series_observer);
}

//------------------------------------------------------------------------------

DicomSeries::dicom_series_container_t DicomSeries::splitFiles(
    filename_container_t& _filenames,
    const core::jobs::observer::sptr& _reader_observer
)
{
    gdcm::Scanner series_scanner;
    series_scanner.AddTag(s_SpecificCharacterSetTag);
    series_scanner.AddTag(s_SeriesInstanceUIDTag);
    series_scanner.AddTag(s_ModalityTag);
    series_scanner.AddTag(s_SeriesDateTag);
    series_scanner.AddTag(s_SeriesTimeTag);
    series_scanner.AddTag(s_SeriesDescriptionTag);
    series_scanner.AddTag(s_PerformingPhysicianNameTag);
    series_scanner.AddTag(s_sop_classUIDTag);
    series_scanner.AddTag(s_SOPInstanceUIDTag);
    series_scanner.AddTag(s_MediaStoragesop_classUID);
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

        const std::string& sop_instance_uid = get_string_value(series_scanner, filename, s_SOPInstanceUIDTag);

        if(previous_sop_instance_ui_ds.find(sop_instance_uid) != previous_sop_instance_ui_ds.end())
        {
            SIGHT_WARN(
                "The instance "
                << sop_instance_uid
                << " has already been read, which usually means DICOM files are corrupted."
            );

            continue;
        }

        const std::string& sop_class_uid               = get_string_value(series_scanner, filename, s_sop_classUIDTag);
        const std::string& media_storage_sop_class_uid =
            get_string_value(series_scanner, filename, s_MediaStoragesop_classUID);

        if(sop_class_uid != gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::MediaStorageDirectoryStorage)
           && media_storage_sop_class_uid
           != gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::MediaStorageDirectoryStorage))
        {
            sight::io::dicom::helper::DicomSeries::createSeries(series_container, series_scanner, dicom_file.second);
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

void DicomSeries::fillSeries(
    dicom_series_container_t& _series_container,
    const core::jobs::observer::sptr& _complete_series_observer
)
{
    std::set<gdcm::Tag> selectedtags;
    selectedtags.insert(s_SpecificCharacterSetTag);
    selectedtags.insert(s_PatientIDTag);
    selectedtags.insert(s_PatientNameTag);
    selectedtags.insert(s_PatientBirthDateTag);
    selectedtags.insert(s_PatientSexTag);
    selectedtags.insert(s_StudyInstanceUIDTag);
    selectedtags.insert(s_StudyDateTag);
    selectedtags.insert(s_StudyTimeTag);
    selectedtags.insert(s_ReferringPhysicianNameTag);
    selectedtags.insert(s_StudyDescriptionTag);
    selectedtags.insert(s_PatientAgeTag);
    selectedtags.insert(s_InstitutionNameTag);
    selectedtags.insert(s_SeriesInstanceUIDTag);

    std::uint64_t progress = 0;

    // Fill series
    for(const data::dicom_series::sptr& series : _series_container)
    {
        // Compute number of instances
        const std::size_t size = series->getDicomContainer().size();
        series->setNumberOfInstances(size);

        if(size == 0U)
        {
            SIGHT_ERROR("The DicomSeries doesn't contain any instance.");
            break;
        }

        // Load first instance
        const auto& first_item                                      = series->getDicomContainer().begin();
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
        const auto& institution_name = get_string_value(dataset, s_InstitutionNameTag);
        series->setInstitutionName(institution_name);

        // Patient Module
        const auto& patient_id = get_string_value(dataset, s_PatientIDTag);
        series->setPatientID(patient_id);

        const auto& patient_name = get_string_value(dataset, s_PatientNameTag);
        series->setPatientName(patient_name);

        const auto& patient_birth_date = get_string_value(dataset, s_PatientBirthDateTag);
        series->setPatientBirthDate(patient_birth_date);

        const auto& patient_sex = get_string_value(dataset, s_PatientSexTag);
        series->setPatientSex(patient_sex);

        // Study Module
        const auto& study_instance_uid = get_string_value(dataset, s_StudyInstanceUIDTag);
        series->setStudyInstanceUID(study_instance_uid);

        //Study Date
        const auto& study_date = get_string_value(dataset, s_StudyDateTag);
        series->setStudyDate(study_date);

        //Study Time
        const auto& study_time = get_string_value(dataset, s_StudyTimeTag);
        series->setStudyTime(study_time);

        //Referring Physician Name
        const auto& referring_physician_name = get_string_value(dataset, s_ReferringPhysicianNameTag);
        series->setReferringPhysicianName(referring_physician_name);

        //Study Description
        const auto& study_description = get_string_value(dataset, s_StudyDescriptionTag);
        series->setStudyDescription(study_description);

        //Study Patient Age
        const auto& patient_age = get_string_value(dataset, s_PatientAgeTag);
        series->setPatientAge(patient_age);

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

void DicomSeries::createSeries(
    dicom_series_container_t& _series_container,
    const gdcm::Scanner& _scanner,
    const std::filesystem::path& _filename
)
{
    data::dicom_series::sptr series = data::dicom_series::sptr();

    const std::string string_filename = _filename.string();

    // Get Series Instance UID
    std::string series_instance_uid = get_string_value(_scanner, string_filename, s_SeriesInstanceUIDTag);

    // Check if the series already exists
    for(const auto& dicom_series : _series_container)
    {
        if(dicom_series->getSeriesInstanceUID() == series_instance_uid)
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
        series->setSeriesInstanceUID(series_instance_uid);

        //Modality
        std::string modality = get_string_value(_scanner, string_filename, s_ModalityTag);
        series->setModality(modality);

        //Date
        std::string series_date = get_string_value(_scanner, string_filename, s_SeriesDateTag);
        series->setSeriesDate(series_date);

        //Time
        std::string series_time = get_string_value(_scanner, string_filename, s_SeriesTimeTag);
        series->setSeriesTime(series_time);

        //Description
        std::string series_description = get_string_value(_scanner, string_filename, s_SeriesDescriptionTag);
        series->setSeriesDescription(series_description);

        //Performing Physicians Name
        std::string performing_physician_name =
            get_string_value(_scanner, string_filename, s_PerformingPhysicianNameTag);
        series->setPerformingPhysicianName(performing_physician_name);
    }

    // Add the sop_classUID to the series
    const std::string& sop_class_uid = get_string_value(
        _scanner,
        string_filename,
        s_sop_classUIDTag
    );

    series->getSOPClassUIDs().insert(sop_class_uid);
    series->addDicomPath(series->getDicomContainer().size(), _filename);
}

} // namespace sight::io::dicom::helper
