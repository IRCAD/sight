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

#include "io/dicom/writer/ie/SpatialFiducials.hpp"

#include "io/dicom/helper/DicomDataTools.hpp"
#include "io/dicom/helper/DicomDataWriter.hxx"

#include <core/tools/date_and_time.hpp>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/point.hpp>
#include <data/point_list.hpp>
#include <data/string.hpp>
#include <data/vector.hpp>

#include <gdcmUIDGenerator.h>

namespace sight::io::dicom::writer::ie
{

//------------------------------------------------------------------------------

spatial_fiducials::spatial_fiducials(
    const SPTR(gdcm::Writer)& _writer,
    const SPTR(io::dicom::container::dicom_instance)& _instance,
    const data::image::csptr& _image,
    const core::log::logger::sptr& _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    io::dicom::writer::ie::information_entity<data::image>(_writer, _instance, _image,
                                                           _logger, _progress, _cancel)
{
}

//------------------------------------------------------------------------------

spatial_fiducials::~spatial_fiducials()
= default;

//------------------------------------------------------------------------------

void spatial_fiducials::write_spatial_fiducials_module()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    boost::posix_time::ptime ptime = boost::posix_time::second_clock::local_time();

    // Content Date - Type 1 - FIXME: Keep series date ?
    std::string date = core::tools::get_date(ptime);
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0023>(date, dataset);

    // Content Time - Type 1 - FIXME: Keep series time ?
    std::string time = core::tools::get_time(ptime);
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0033>(time, dataset);

    // Instance Number - Type 1
    io::dicom::helper::dicom_data_writer::set_tag_value<unsigned int, 0x0020, 0x0013>(0, dataset);

    // Content Label - Type 1
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0070, 0x0080>("SF", dataset);

    // Content Description - Type 2
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0070, 0x0081>("Spatial Fiducials", dataset);

    // Content Creator's Name - Type 2 - TODO: Set fiducial creator name ?
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0070, 0x0084>("Unknown^Unknown", dataset);

    // Fiducial Set Sequence - Type 1
    gdcm::SmartPointer<gdcm::SequenceOfItems> fiducial_set_sequence = new gdcm::SequenceOfItems();
    io::dicom::helper::dicom_data_writer::set_sequence_tag_value<0x0070, 0x031C>(fiducial_set_sequence, dataset);

    // Create Fiducial Sequence Item
    gdcm::Item fiducial_set_item;
    fiducial_set_item.SetVLToUndefined();
    gdcm::DataSet& fiducial_set_item_dataset = fiducial_set_item.GetNestedDataSet();

    // Referenced Image Sequence - Type 1C
    gdcm::SmartPointer<gdcm::SequenceOfItems> referenced_image_sequence = new gdcm::SequenceOfItems();
    io::dicom::helper::dicom_data_writer::set_sequence_tag_value<0x0008, 0x1140>(
        referenced_image_sequence,
        fiducial_set_item_dataset
    );

    // Add all referenced image
    for(unsigned int index = 0 ; index < m_instance->get_sop_instance_uid_container().size() ; ++index)
    {
        gdcm::Item referenced_image_item;
        referenced_image_item.SetVLToUndefined();
        gdcm::DataSet& referenced_image_item_dataset = referenced_image_item.GetNestedDataSet();

        // Referenced Frame Number - Type 1C
        int frame_number = int(index) + 1;
        io::dicom::helper::dicom_data_writer::set_tag_values<int, 0x0008, 0x1160>(
            &frame_number,
            1,
            referenced_image_item_dataset
        );

        // Referenced SOP Class UID - Type 1
        io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x1150>(
            m_instance->get_sop_class_uid(),
            referenced_image_item_dataset
        );

        // Referenced SOP Instance UID - Type 1
        io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x1155>(
            m_instance->get_sop_instance_uid_container()[index],
            referenced_image_item_dataset
        );

        // Add referenced image to sequence
        referenced_image_sequence->AddItem(referenced_image_item);
    }

    // Fiducial Sequence - Type 1
    gdcm::SmartPointer<gdcm::SequenceOfItems> fiducial_sequence = new gdcm::SequenceOfItems();
    io::dicom::helper::dicom_data_writer::set_sequence_tag_value<0x0070, 0x031E>(
        fiducial_sequence,
        fiducial_set_item_dataset
    );

    // Write landmarks
    this->write_landmarks(fiducial_sequence);

    // Add Fiducial Set to sequence
    fiducial_set_sequence->AddItem(fiducial_set_item);
}

//------------------------------------------------------------------------------

void spatial_fiducials::write_landmarks(gdcm::SmartPointer<gdcm::SequenceOfItems> _sequence)
{
    data::point_list::sptr point_list = data::helper::medical_image::get_landmarks(*m_object);
    if(point_list)
    {
        unsigned int index = 0;
        for(const data::point::sptr& point : point_list->get_points())
        {
            gdcm::Item fiducial_item;
            fiducial_item.SetVLToUndefined();
            gdcm::DataSet& fiducial_item_dataset = fiducial_item.GetNestedDataSet();

            // Fiducial Identifier - Type 1
            std::stringstream ss_identifier;
            ss_identifier << "landmark-" << index;
            io::dicom::helper::dicom_data_writer::set_tag_value<0x0070, 0x0310>(
                ss_identifier.str(),
                fiducial_item_dataset
            );

            // Fiducial Description - Type 3
            std::string label = point->get_label();
            io::dicom::helper::dicom_data_writer::set_tag_value<0x0070, 0x030F>(label, fiducial_item_dataset);

            // Shape Type - Type 1
            io::dicom::helper::dicom_data_writer::set_tag_value<0x0070, 0x0306>("POINT", fiducial_item_dataset);

            // Graphic Coordinates Data Sequence - Type 1C
            gdcm::SmartPointer<gdcm::SequenceOfItems> graphic_coodinates_data_sequence =
                new gdcm::SequenceOfItems();
            io::dicom::helper::dicom_data_writer::set_sequence_tag_value<0x0070, 0x0318>(
                graphic_coodinates_data_sequence,
                fiducial_item_dataset
            );

            gdcm::Item graphic_data_item;
            graphic_data_item.SetVLToUndefined();
            gdcm::DataSet& graphic_data_item_dataset = graphic_data_item.GetNestedDataSet();

            // Graphic Data - Type 1
            std::array coordinates {static_cast<float>(point->get_coord()[0]), static_cast<float>(point->get_coord()[1])
            };
            io::dicom::helper::dicom_data_writer::set_tag_values<float, 0x0070, 0x0022>(
                coordinates.data(),
                2,
                graphic_data_item_dataset
            );

            // Referenced Image Sequence - Type 1
            gdcm::SmartPointer<gdcm::SequenceOfItems> referenced_image_sequence = new gdcm::SequenceOfItems();
            io::dicom::helper::dicom_data_writer::set_sequence_tag_value<0x0008, 0x1140>(
                referenced_image_sequence,
                graphic_data_item_dataset
            );

            int frame_number =
                static_cast<int>(io::dicom::helper::dicom_data_tools::convert_point_to_frame_number(
                                     m_object,
                                     point
                ));
            this->add_referenced_image(frame_number, referenced_image_sequence);

            // Add graphic data to sequence
            graphic_coodinates_data_sequence->AddItem(graphic_data_item);

            // Add Fiducial to sequence
            _sequence->AddItem(fiducial_item);

            // Increase index
            ++index;
        }
    }
}

//------------------------------------------------------------------------------

void spatial_fiducials::write_common_instance_reference_module()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Referenced Series Sequence - Type 1C
    gdcm::SmartPointer<gdcm::SequenceOfItems> referenced_series_sequence = new gdcm::SequenceOfItems();
    io::dicom::helper::dicom_data_writer::set_sequence_tag_value<0x0008, 0x1115>(referenced_series_sequence, dataset);

    // Create Referenced Series
    gdcm::Item referenced_series_item;
    referenced_series_item.SetVLToUndefined();
    gdcm::DataSet& referenced_series_item_dataset = referenced_series_item.GetNestedDataSet();

    // Series Instance UID - Type 1
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0020, 0x000E>(
        m_instance->get_series_instance_uid(),
        referenced_series_item_dataset
    );

    // Referenced Instance Sequence - Type 1
    gdcm::SmartPointer<gdcm::SequenceOfItems> referenced_instance_sequence = new gdcm::SequenceOfItems();
    io::dicom::helper::dicom_data_writer::set_sequence_tag_value<0x0008, 0x114A>(
        referenced_instance_sequence,
        referenced_series_item_dataset
    );

    // Add all referenced image
    for(auto& index : m_instance->get_sop_instance_uid_container())
    {
        gdcm::Item referenced_instance_item;
        referenced_instance_item.SetVLToUndefined();
        gdcm::DataSet& referenced_instance_item_dataset = referenced_instance_item.GetNestedDataSet();

        // Referenced SOP Class UID - Type 1
        io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x1150>(
            m_instance->get_sop_class_uid(),
            referenced_instance_item_dataset
        );

        // Referenced SOP Instance UID - Type 1
        io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x1155>(
            index,
            referenced_instance_item_dataset
        );

        // Add referenced image to sequence
        referenced_instance_sequence->AddItem(referenced_instance_item);
    }

    referenced_series_sequence->AddItem(referenced_series_item);

    // Studies Containing Other Referenced Instances Sequence - Type 1C
    gdcm::SmartPointer<gdcm::SequenceOfItems> studies_sequence = new gdcm::SequenceOfItems();
    io::dicom::helper::dicom_data_writer::set_sequence_tag_value<0x0008, 0x1200>(studies_sequence, dataset);

    gdcm::Item studies_item;
    studies_item.SetVLToUndefined();
    gdcm::DataSet& studies_item_dataset = studies_item.GetNestedDataSet();

    // Study Instance UID - Type 1
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0020, 0x000D>(
        m_instance->get_study_instance_uid(),
        studies_item_dataset
    );

    // Referenced Series Sequence - Type 1
    io::dicom::helper::dicom_data_writer::set_sequence_tag_value<0x0008, 0x1115>(
        referenced_series_sequence,
        studies_item_dataset
    );

    studies_sequence->AddItem(studies_item);
}

//------------------------------------------------------------------------------

void spatial_fiducials::write_sop_common_module()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // SOP Class UID
    std::string sop_class_uid = gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::SpacialFiducialsStorage);
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0016>(sop_class_uid, dataset);

    // SOP Instance UID
    gdcm::UIDGenerator uid_generator;
    std::string sop_instance_uid = uid_generator.Generate();
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0018>(sop_instance_uid, dataset);
}

//------------------------------------------------------------------------------

void spatial_fiducials::add_referenced_image(
    int _frame_number,
    gdcm::SmartPointer<gdcm::SequenceOfItems> _referenced_image_sequence
)
{
    gdcm::Item referenced_image_item;
    referenced_image_item.SetVLToUndefined();
    gdcm::DataSet& referenced_image_item_dataset = referenced_image_item.GetNestedDataSet();

    // Referenced Frame Number - Type 1C
    io::dicom::helper::dicom_data_writer::set_tag_values<int, 0x0008, 0x1160>(
        &_frame_number,
        1,
        referenced_image_item_dataset
    );

    // Referenced SOP Class UID - Type 1
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x1150>(
        m_instance->get_sop_class_uid(),
        referenced_image_item_dataset
    );

    // Referenced SOP Instance UID - Type 1
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x1155>(
        m_instance->get_sop_instance_uid_container()[std::size_t(_frame_number - 1)],
        referenced_image_item_dataset
    );

    // Add referenced image to sequence
    _referenced_image_sequence->AddItem(referenced_image_item);
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::ie
