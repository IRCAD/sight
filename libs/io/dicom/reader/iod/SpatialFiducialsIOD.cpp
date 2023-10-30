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

#include "io/dicom/reader/iod/SpatialFiducialsIOD.hpp"

#include "io/dicom/helper/DicomDataReader.hxx"
#include "io/dicom/reader/ie/SpatialFiducials.hpp"

#include <data/image_series.hpp>
#include <data/point_list.hpp>
#include <data/vector.hpp>

#include <memory>
#include <utility>

namespace sight::io::dicom::reader::iod
{

//------------------------------------------------------------------------------

spatial_fiducials_iod::spatial_fiducials_iod(
    const data::dicom_series::csptr& _dicom_series,
    const io::dicom::container::dicom_instance::sptr& _instance,
    const core::log::logger::sptr& _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    io::dicom::reader::iod::information_object_definition(_dicom_series, _instance, _logger, _progress, _cancel)
{
}

//------------------------------------------------------------------------------

spatial_fiducials_iod::~spatial_fiducials_iod()
= default;

//------------------------------------------------------------------------------

void spatial_fiducials_iod::read(data::series::sptr _series)
{
    // Retrieve images
    data::image_series::sptr image_series = std::dynamic_pointer_cast<data::image_series>(_series);
    SIGHT_ASSERT("ImageSeries should not be null.", image_series);

    // Create GDCM Reader
    SPTR(gdcm::Reader) reader = std::make_shared<gdcm::Reader>();

    // Read the first file
    data::dicom_series::dicom_container_t dicom_container = m_dicom_series->get_dicom_container();

    if(dicom_container.size() > 1)
    {
        m_logger->warning(
            "More than one Spatial Fiducials item have been found in the series. "
            "Only the first one will be read."
        );
    }

    const core::memory::buffer_object::sptr buffer_obj          = dicom_container.begin()->second;
    const core::memory::buffer_manager::stream_info stream_info = buffer_obj->get_stream_info();
    SPTR(std::istream) is = stream_info.stream;
    reader->SetStream(*is);

    const bool success = reader->Read();
    SIGHT_THROW_EXCEPTION_IF(
        io::dicom::exception::failed(
            "Unable to read the DICOM instance \""
            + buffer_obj->get_stream_info().fs_file.string()
            + "\" using the GDCM Reader."
        ),
        !success
    );

    // Create Information Entity helpers
    io::dicom::reader::ie::spatial_fiducials spatial_fiducials_ie(
        m_dicom_series, reader, m_instance, image_series,
        m_logger, m_progress_callback, m_cancel_requested_callback);

    // Retrieve dataset
    const gdcm::DataSet& dataset_root = reader->GetFile().GetDataSet();

    // Retrieve Fiducial Set Sequence
    const gdcm::DataElement& fiducial_set_sequence_data_element =
        dataset_root.GetDataElement(gdcm::Tag(0x0070, 0x031C));
    const gdcm::SmartPointer<gdcm::SequenceOfItems> fiducial_set_sequence =
        fiducial_set_sequence_data_element.GetValueAsSQ();

    for(unsigned int i = 1 ; i <= fiducial_set_sequence->GetNumberOfItems() ; ++i)
    {
        gdcm::Item sequence_set_item              = fiducial_set_sequence->GetItem(i);
        const gdcm::DataSet& sequence_set_dataset = sequence_set_item.GetNestedDataSet();

        const gdcm::DataElement& fiducial_sequence_data_element =
            sequence_set_dataset.GetDataElement(gdcm::Tag(0x0070, 0x031E));
        const gdcm::SmartPointer<gdcm::SequenceOfItems> fiducial_sequence =
            fiducial_sequence_data_element.GetValueAsSQ();

        for(unsigned int j = 1 ; j <= fiducial_sequence->GetNumberOfItems() ; ++j)
        {
            gdcm::Item fiducial_item              = fiducial_sequence->GetItem(j);
            const gdcm::DataSet& fiducial_dataset = fiducial_item.GetNestedDataSet();
            const std::string shape_type          =
                io::dicom::helper::dicom_data_reader::get_tag_value<0x0070, 0x0306>(fiducial_dataset);

            if(shape_type == "POINT")
            {
                spatial_fiducials_ie.read_landmark(fiducial_dataset);
            }
            else
            {
                m_logger->warning("Fiducial shape type not supported: \"" + shape_type + "\"");
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::iod
