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

#include "io/dicom/reader/series.hpp"

#include "io/dicom/helper/dicom_data_reader.hxx"
#include "io/dicom/reader/iod/comprehensive_srio_d.hpp"
#include "io/dicom/reader/iod/ctmr_image_io_d.hpp"
#include "io/dicom/reader/iod/spatial_fiducials_io_d.hpp"
#include "io/dicom/reader/iod/surface_segmentation_io_d.hpp"

#include <data/dicom/series.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/model_series.hpp>

#include <gdcmImageReader.h>
#include <gdcmScanner.h>

#include <memory>

namespace sight::io::dicom::reader
{

//------------------------------------------------------------------------------

series::series()
= default;

//------------------------------------------------------------------------------

series::~series()
= default;

// ----------------------------------------------------------------------------

data::series::sptr series::read(const data::dicom_series::csptr& _dicom_series)
{
    SIGHT_ASSERT("DicomSeries should not be null.", _dicom_series);
    SIGHT_ASSERT("Logger should not be null.", m_logger);

    // Create instance
    SPTR(io::dicom::container::dicom_instance) instance =
        std::make_shared<io::dicom::container::dicom_instance>(_dicom_series);

    // Create result
    data::series::sptr result;

    if(!_dicom_series->get_dicom_container().empty())
    {
        // Get sop_classUID
        data::dicom_series::sop_class_uid_container_t sop_class_uid_container = _dicom_series->get_sop_class_ui_ds();
        const std::string sop_class_uid                                       = *sop_class_uid_container.begin();

        // If the DicomSeries contains an image (ImageSeries)
        if(gdcm::MediaStorage::IsImage(gdcm::MediaStorage::GetMSType(sop_class_uid.c_str()))
           && gdcm::MediaStorage::GetMSType(sop_class_uid.c_str()) != gdcm::MediaStorage::SpacialFiducialsStorage)
        {
            // Read the image
            data::image_series::sptr image_series = data::dicom::series::convert_to_image_series(_dicom_series);
            image_series->set_dicom_reference(_dicom_series);

            // Create IOD Reader
            io::dicom::reader::iod::ctmr_image_iod iod(_dicom_series, instance, m_logger,
                                                       m_progress_callback, m_cancel_requested_callback);
            iod.set_buffer_rotation_enabled(m_enable_buffer_rotation);

            try
            {
                iod.read(image_series);
            }
            catch(const io::dicom::exception::failed& e)
            {
                // NOTE : if there is no image, reading is stopped.
                m_logger->critical(e.what());
                image_series = data::image_series::sptr();
            }

            // Set result
            result = image_series;
        }
        // Get the RT file names (ModelSeries)
        else if(gdcm::MediaStorage::GetMSType(sop_class_uid.c_str())
                == gdcm::MediaStorage::SurfaceSegmentationStorage)
        {
            data::model_series::sptr model_series = data::dicom::series::convert_to_model_series(_dicom_series);
            model_series->set_dicom_reference(_dicom_series);
            // Create IOD Reader
            io::dicom::reader::iod::surface_segmentation_iod iod(_dicom_series, instance, m_logger,
                                                                 m_progress_callback, m_cancel_requested_callback);

            try
            {
                iod.read(model_series);
            }
            catch(const io::dicom::exception::failed& e)
            {
                // NOTE : if there is no image, reading is stopped.
                m_logger->critical(e.what());
                model_series = data::model_series::sptr();
            }

            // Set result
            result = model_series;
        }
        // If the DicomSeries contains a Spatial Fiducials
        else if(gdcm::MediaStorage::GetMSType(sop_class_uid.c_str()) == gdcm::MediaStorage::SpacialFiducialsStorage)
        {
            // Retrieve referenced image instance
            SPTR(io::dicom::container::dicom_instance) image_instance =
                this->get_spatial_fiducials_referenced_series_instance(_dicom_series);

            if(image_instance)
            {
                data::image_series::sptr image_series =
                    std::dynamic_pointer_cast<data::image_series>(m_series_container_map[image_instance]);

                image_series->set_dicom_reference(_dicom_series);

                // Create IOD Reader
                io::dicom::reader::iod::spatial_fiducials_iod iod(_dicom_series, instance, m_logger,
                                                                  m_progress_callback, m_cancel_requested_callback);

                try
                {
                    iod.read(image_series);
                }
                catch(const io::dicom::exception::failed& e)
                {
                    //NOTE: no throw for reading error for SR and RT doc
                    m_logger->critical("Spatial Fiducials reading failed: " + std::string(e.what()));
                }
            }
            else
            {
                m_logger->critical(
                    "The spatial fiducials series \"" + _dicom_series->get_series_instance_uid()
                    + "\" could not be read as it refers to an unknown series UID."
                );
            }
        }
        // If the DicomSeries contains a SR
        else if(gdcm::MediaStorage::GetMSType(sop_class_uid.c_str()) == gdcm::MediaStorage::EnhancedSR
                || gdcm::MediaStorage::GetMSType(sop_class_uid.c_str()) == gdcm::MediaStorage::ComprehensiveSR
                || sop_class_uid == "1.2.840.10008.5.1.4.1.1.88.34") // FIXME Replace hard coded string by
                                                                     // "gdcm::MediaStorage::GetMSType(sopClassUID.c_str())
                                                                     // == gdcm::MediaStorage::Comprehensive3DSR"
        {
            // Retrieve referenced image instance
            SPTR(io::dicom::container::dicom_instance) referenced_instance =
                this->get_structured_report_referenced_series_instance(_dicom_series);

            data::image_series::sptr image_series;
            const auto& iter = m_series_container_map.find(referenced_instance);
            if(iter != m_series_container_map.end())
            {
                image_series = std::dynamic_pointer_cast<data::image_series>(iter->second);
            }

            if(referenced_instance && image_series)
            {
                image_series->set_dicom_reference(_dicom_series);

                // Create readers
                io::dicom::reader::iod::comprehensive_sriod iod(_dicom_series, referenced_instance, m_logger,
                                                                m_progress_callback, m_cancel_requested_callback);

                try
                {
                    iod.read(image_series);
                }
                catch(const io::dicom::exception::failed& e)
                {
                    //NOTE: no throw for reading error for SR and RT doc
                    m_logger->critical("Structured Report reading failed: " + std::string(e.what()));
                }
            }
            else
            {
                m_logger->critical(
                    "The structured report series \"" + _dicom_series->get_series_instance_uid()
                    + "\" could not be read as it refers to an unknown series UID."
                );
            }
        }
        else
        {
            m_logger->critical(
                "DICOM SOP Class UID \"" + sop_class_uid
                + "\" is not supported by the selected reader."
            );
        }
    }

    // Store series in instance map
    if(result)
    {
        m_series_container_map[instance] = result;
    }

    return result;
}

//------------------------------------------------------------------------------

SPTR(io::dicom::container::dicom_instance) series::get_spatial_fiducials_referenced_series_instance(
    const data::dicom_series::csptr& _dicom_series
)
{
    SPTR(io::dicom::container::dicom_instance) result;

    // Dicom container
    data::dicom_series::dicom_container_t dicom_container = _dicom_series->get_dicom_container();

    // Create Reader
    std::shared_ptr<gdcm::Reader> reader =
        std::make_shared<gdcm::Reader>();
    const core::memory::buffer_object::sptr buffer_obj          = dicom_container.begin()->second;
    const core::memory::buffer_manager::stream_info stream_info = buffer_obj->get_stream_info();
    SPTR(std::istream) is = stream_info.stream;
    reader->SetStream(*is);

    // series Instance UID of the referenced series
    std::string series_instance_uid;

    if(reader->Read())
    {
        // Retrieve dataset
        const gdcm::DataSet& dataset_root = reader->GetFile().GetDataSet();

        if(dataset_root.FindDataElement(gdcm::Tag(0x0008, 0x1115)))
        {
            // Get the content sequence
            gdcm::SmartPointer<gdcm::SequenceOfItems> sequence =
                dataset_root.GetDataElement(gdcm::Tag(0x0008, 0x1115)).GetValueAsSQ();

            if(sequence->GetNumberOfItems() > 0)
            {
                gdcm::Item referenced_series_item                   = sequence->GetItem(1);
                const gdcm::DataSet& referenced_series_item_dataset = referenced_series_item.GetNestedDataSet();

                // series Instance UID - Type 1
                series_instance_uid =
                    io::dicom::helper::dicom_data_reader::get_tag_value<0x0020, 0x000E>(referenced_series_item_dataset);
            }
        }
    }

    if(!series_instance_uid.empty())
    {
        for(const auto& v : m_series_container_map)
        {
            if(v.first->get_series_instance_uid() == series_instance_uid)
            {
                result = v.first;
                break;
            }
        }
    }

    return result;
}

//------------------------------------------------------------------------------

SPTR(io::dicom::container::dicom_instance) series::get_structured_report_referenced_series_instance(
    const data::dicom_series::csptr& _dicom_series
)
{
    SPTR(io::dicom::container::dicom_instance) result;

    // Dicom container
    data::dicom_series::dicom_container_t dicom_container = _dicom_series->get_dicom_container();

    // Create Reader
    std::shared_ptr<gdcm::Reader> reader =
        std::make_shared<gdcm::Reader>();
    const core::memory::buffer_object::sptr buffer_obj          = dicom_container.begin()->second;
    const core::memory::buffer_manager::stream_info stream_info = buffer_obj->get_stream_info();
    SPTR(std::istream) is = stream_info.stream;
    reader->SetStream(*is);

    // series Instance UID of the referenced series
    std::string series_instance_uid;

    if(reader->Read())
    {
        // Retrieve dataset
        const gdcm::DataSet& dataset_root = reader->GetFile().GetDataSet();

        // Pertinent Other Evidence Sequence - Type 1C
        if(dataset_root.FindDataElement(gdcm::Tag(0x0040, 0xa385)))
        {
            // Get the content sequence
            gdcm::SmartPointer<gdcm::SequenceOfItems> sequence =
                dataset_root.GetDataElement(gdcm::Tag(0x0040, 0xa385)).GetValueAsSQ();

            if(sequence->GetNumberOfItems() > 0)
            {
                gdcm::Item study_item                   = sequence->GetItem(1);
                const gdcm::DataSet& study_item_dataset = study_item.GetNestedDataSet();

                if(study_item_dataset.FindDataElement(gdcm::Tag(0x0008, 0x1115)))
                {
                    // Get the series sequence
                    gdcm::SmartPointer<gdcm::SequenceOfItems> series_sequence =
                        study_item_dataset.GetDataElement(gdcm::Tag(0x0008, 0x1115)).GetValueAsSQ();

                    if(series_sequence->GetNumberOfItems() > 0)
                    {
                        gdcm::Item series_item                   = series_sequence->GetItem(1);
                        const gdcm::DataSet& series_item_dataset = series_item.GetNestedDataSet();
                        series_instance_uid = io::dicom::helper::dicom_data_reader::get_tag_value<0x0020, 0x000E>(
                            series_item_dataset
                        );
                    }
                }
            }
        }
    }

    if(!series_instance_uid.empty())
    {
        for(const auto& v : m_series_container_map)
        {
            if(v.first->get_series_instance_uid() == series_instance_uid)
            {
                result = v.first;
                break;
            }
        }
    }

    return result;
}
//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader
