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

#include "io/dicom/container/dicom_instance.hpp"

#include "io/dicom/helper/dicom_data_reader.hxx"

#include <data/dicom_series.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/model_series.hpp>
#include <data/series.hpp>

#include <gdcmReader.h>
#include <gdcmScanner.h>
#include <gdcmUIDGenerator.h>

#include <utility>

namespace sight::io::dicom::container
{

//------------------------------------------------------------------------------

dicom_instance::dicom_instance() :

    m_logger(nullptr)
{
}

//------------------------------------------------------------------------------

dicom_instance::dicom_instance(
    const data::series::csptr& _series,
    core::log::logger::sptr _logger,
    bool _is_multi_files
) :
    m_is_multi_files(_is_multi_files),
    m_study_instance_uid(_series->get_study_instance_uid()),
    m_series_instance_uid(_series->get_series_instance_uid()),
    m_logger(std::move(_logger))
{
    // Compute sop_classUID
    this->computesop_class_uid(_series);

    // Generate SOPInstanceUIDs
    this->generate_sop_instance_ui_ds(_series);

    // Generate Frame of Reference UID
    gdcm::UIDGenerator uid_generator;
    m_frame_of_reference_uid = uid_generator.Generate();
}

//------------------------------------------------------------------------------

dicom_instance::dicom_instance(
    const data::dicom_series::csptr& _dicom_series,
    core::log::logger::sptr _logger
) :
    m_is_multi_files(_dicom_series->get_dicom_container().size() > 1),
    m_study_instance_uid(_dicom_series->get_study_instance_uid()),
    m_series_instance_uid(_dicom_series->get_series_instance_uid()),
    m_logger(std::move(_logger))
{
    SIGHT_ASSERT("DicomSeries is not instantiated", _dicom_series);

    // Get sop_classUID
    data::dicom_series::sop_class_uid_container_t sop_class_uid_container = _dicom_series->get_sop_class_ui_ds();
    if(!sop_class_uid_container.empty())
    {
        m_sop_class_uid = *(sop_class_uid_container.begin());
    }

    this->read_uid_from_dicom_series(_dicom_series);
}

//------------------------------------------------------------------------------

dicom_instance::dicom_instance(const dicom_instance& _dicom_instance) :
    m_is_multi_files(_dicom_instance.m_is_multi_files),
    m_sop_class_uid(_dicom_instance.m_sop_class_uid),
    m_sop_instance_uid_container(_dicom_instance.m_sop_instance_uid_container),
    m_logger(_dicom_instance.m_logger)
{
}

//------------------------------------------------------------------------------

dicom_instance::~dicom_instance()
= default;

//------------------------------------------------------------------------------

void dicom_instance::computesop_class_uid(const data::series::csptr& _series)
{
    // Retrieve series type
    data::image_series::csptr image_series = std::dynamic_pointer_cast<const data::image_series>(_series);
    data::model_series::csptr model_series = std::dynamic_pointer_cast<const data::model_series>(_series);

    // Create result
    std::string sop_class_uid;

    if(image_series)
    {
        // Compute instance dimension
        unsigned int dimension = get_is_multi_files() ? 2 : static_cast<unsigned int>(image_series->num_dimensions());

        // Define SOP Class UID from the modality
        gdcm::MediaStorage media_storage;
        media_storage.GuessFromModality(_series->get_modality().c_str(), dimension);

        // Identify the sop_classUID from a guess
        if(media_storage != gdcm::MediaStorage::MS_END && media_storage.GetString() != nullptr)
        {
            sop_class_uid = media_storage.GetString();
        }
        // Force sop_classUID to be CTImageStorage
        else
        {
            sop_class_uid = gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::CTImageStorage);
        }
    }
    else if(model_series)
    {
        sop_class_uid = gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::SurfaceSegmentationStorage);
    }

    // Update instance information
    this->set_sop_class_uid(sop_class_uid);
}

//------------------------------------------------------------------------------

void dicom_instance::generate_sop_instance_ui_ds(const data::series::csptr& _series)
{
    // Retrieve ImageSeries
    data::image_series::csptr image_series = std::dynamic_pointer_cast<const data::image_series>(_series);

    // Compute number of instances
    const std::size_t nb_instances = (image_series && m_is_multi_files) ? (image_series->size()[2]) : (1);

    // Create generator
    gdcm::UIDGenerator uid_generator;

    // Generate UIDs
    for(std::size_t i = 0 ; i < nb_instances ; ++i)
    {
        m_sop_instance_uid_container.emplace_back(uid_generator.Generate());
    }
}

//------------------------------------------------------------------------------

void dicom_instance::read_uid_from_dicom_series(const data::dicom_series::csptr& _dicom_series)
{
    const gdcm::Tag sop_instance_uid_tag       = gdcm::Tag(0x0008, 0x0018); // SOP Instance UID
    const gdcm::Tag frame_of_reference_uid_tag = gdcm::Tag(0x0020, 0x0052); // Frame of Reference UID
    std::set<gdcm::Tag> selectedtags;
    selectedtags.insert(sop_instance_uid_tag);
    selectedtags.insert(frame_of_reference_uid_tag);

    std::set<std::string> frame_of_reference_uid_container;
    for(const auto& item : _dicom_series->get_dicom_container())
    {
        const core::memory::buffer_object::sptr buffer_obj          = item.second;
        const core::memory::buffer_manager::stream_info stream_info = buffer_obj->get_stream_info();
        SPTR(std::istream) is = stream_info.stream;

        gdcm::Reader reader;
        reader.SetStream(*is);
        if(!reader.ReadSelectedTags(selectedtags))
        {
            SIGHT_THROW(
                "Unable to read Dicom file '" << buffer_obj->get_stream_info().fs_file.string() << "' "
                << "(slice: '" << item.first << "')"
            );
        }

        const gdcm::DataSet& dataset = reader.GetFile().GetDataSet();
        // SOP Instance UID
        m_sop_instance_uid_container.push_back(
            io::dicom::helper::dicom_data_reader::get_tag_value<0x0008,
                                                                0x0018>(dataset)
        );
        // Retrieve frame of reference UID
        frame_of_reference_uid_container.insert(
            io::dicom::helper::dicom_data_reader::get_tag_value<0x0020,
                                                                0x0052>(dataset)
        );
    }

    if(frame_of_reference_uid_container.size() == 1)
    {
        m_frame_of_reference_uid = *(frame_of_reference_uid_container.begin());
    }
    else if(frame_of_reference_uid_container.size() > 1)
    {
        const std::string msg = "The selected DICOM series contain several Frame of Reference.";
        SIGHT_WARN_IF(msg, !m_logger);
        if(m_logger)
        {
            m_logger->critical(msg);
        }
    }
    else
    {
        const std::string msg = "No Frame of Reference has been found in the selected series.";
        SIGHT_WARN_IF(msg, !m_logger);
        if(m_logger)
        {
            m_logger->critical(msg);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::container
