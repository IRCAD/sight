/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "io/dicom/reader/ie/SpatialFiducials.hpp"

#include "io/dicom/helper/DicomDataReader.hxx"
#include "io/dicom/helper/DicomDataTools.hpp"

#include <data/dicom_series.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/point_list.hpp>
#include <data/string.hpp>

namespace sight::io::dicom::reader::ie
{

//------------------------------------------------------------------------------

spatial_fiducials::spatial_fiducials(
    const data::dicom_series::csptr& _dicom_series,
    const SPTR(gdcm::Reader)& _reader,
    const io::dicom::container::dicom_instance::sptr& _instance,
    const data::image::sptr& _image,
    const core::log::logger::sptr& _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    io::dicom::reader::ie::information_entity<data::image>(_dicom_series, _reader, _instance, _image,
                                                           _logger, _progress, _cancel)
{
}

//------------------------------------------------------------------------------

spatial_fiducials::~spatial_fiducials()
= default;

//------------------------------------------------------------------------------

void spatial_fiducials::read_landmark(const gdcm::DataSet& _fiducial_dataset)
{
    data::point_list::sptr point_list = data::helper::medical_image::get_landmarks(*m_object);

    if(!point_list)
    {
        point_list = std::make_shared<data::point_list>();
        data::helper::medical_image::set_landmarks(*m_object, point_list);
    }

    const gdcm::DataElement& graphic_coordinates_data_element =
        _fiducial_dataset.GetDataElement(gdcm::Tag(0x0070, 0x0318));
    const gdcm::SmartPointer<gdcm::SequenceOfItems> graphic_coordinates_data_sequence =
        graphic_coordinates_data_element.GetValueAsSQ();

    const std::string label =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0070, 0x030F>(_fiducial_dataset);

    for(unsigned int i = 1 ; i <= graphic_coordinates_data_sequence->GetNumberOfItems() ; ++i)
    {
        gdcm::Item graphic_coordinates_item              = graphic_coordinates_data_sequence->GetItem(i);
        const gdcm::DataSet& graphic_coordinates_dataset = graphic_coordinates_item.GetNestedDataSet();

        // 2D Points
        gdcm::Attribute<0x0070, 0x0022> coordinates_attribute;
        coordinates_attribute.SetFromDataElement(graphic_coordinates_dataset.GetDataElement(gdcm::Tag(0x0070, 0x0022)));
        const float* point_values = coordinates_attribute.GetValues();

        // Frame number
        const gdcm::DataElement& referenced_image_data_element =
            graphic_coordinates_dataset.GetDataElement(gdcm::Tag(0x0008, 0x1140));
        const gdcm::SmartPointer<gdcm::SequenceOfItems> referenced_image_data_sequence =
            referenced_image_data_element.GetValueAsSQ();
        gdcm::Item referenced_image_item        = referenced_image_data_sequence->GetItem(1);
        gdcm::DataSet& referenced_image_dataset = referenced_image_item.GetNestedDataSet();

        int frame_number =
            io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x1160, int>(referenced_image_dataset);
        double z_coordinate =
            io::dicom::helper::dicom_data_tools::convert_frame_number_to_z_coordinate(
                m_object,
                std::size_t(frame_number)
            );

        data::point::sptr point = std::make_shared<data::point>(
            static_cast<double>(point_values[0]),
            static_cast<double>(point_values[1]),
            z_coordinate
        );
        point->set_label(label);
        point_list->get_points().push_back(point);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::ie
