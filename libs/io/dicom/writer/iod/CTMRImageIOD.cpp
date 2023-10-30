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

#include "io/dicom/writer/iod/CTMRImageIOD.hpp"

#include "io/dicom/helper/FileWriter.hpp"
#include "io/dicom/writer/ie/Equipment.hpp"
#include "io/dicom/writer/ie/FrameOfReference.hpp"
#include "io/dicom/writer/ie/image.hpp"
#include "io/dicom/writer/ie/Patient.hpp"
#include "io/dicom/writer/ie/series.hpp"
#include "io/dicom/writer/ie/Study.hpp"

#include <core/spy_log.hpp>

#include <data/image.hpp>
#include <data/image_series.hpp>

#include <gdcmImageWriter.h>

namespace sight::io::dicom::writer::iod
{

//------------------------------------------------------------------------------

ctmr_image_iod::ctmr_image_iod(
    const SPTR(io::dicom::container::dicom_instance)& _instance,
    const std::filesystem::path& _destination_path,
    const core::log::logger::sptr& _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    io::dicom::writer::iod::information_object_definition(_instance, _destination_path, _logger,
                                                          _progress, _cancel)
{
}

//------------------------------------------------------------------------------

ctmr_image_iod::~ctmr_image_iod()
= default;

//------------------------------------------------------------------------------

void ctmr_image_iod::write(const data::series::csptr& _series)
{
    // Retrieve image series
    data::image_series::csptr image_series = std::dynamic_pointer_cast<const data::image_series>(_series);
    SIGHT_ASSERT("Image series should not be null.", image_series);

    // Create writer
    SPTR(gdcm::ImageWriter) writer = std::make_shared<gdcm::ImageWriter>();

    // Create Information Entity helpers
    io::dicom::writer::ie::patient patient_ie(writer, m_instance, _series);
    io::dicom::writer::ie::study study_ie(writer, m_instance, _series);
    io::dicom::writer::ie::series series_ie(writer, m_instance, _series);
    io::dicom::writer::ie::frame_of_reference frame_of_reference_ie(writer, m_instance, _series);
    io::dicom::writer::ie::equipment equipment_ie(writer, m_instance, _series);
    io::dicom::writer::ie::image image_ie(writer, m_instance, image_series);

    // Write Patient Module - PS 3.3 C.7.1.1
    patient_ie.write_patient_module();

    // Write General Study Module - PS 3.3 C.7.2.1
    study_ie.write_general_study_module();

    // Write Patient Study Module - PS 3.3 C.7.2.2
    study_ie.write_patient_study_module();

    // Write General Series Module - PS 3.3 C.7.3.1
    series_ie.write_general_series_module();

    // Write Frame of Reference Module - PS 3.3 C.7.4.1
    frame_of_reference_ie.write_frame_of_reference_module();

    // Write General Equipment Module - PS 3.3 C.7.5.1
    equipment_ie.write_general_equipment_module();

    // Write General Image Module - PS 3.3 C.7.6.1
    image_ie.write_general_image_module();

    // Write Image Plane Module - PS 3.3 C.7.6.2
    image_ie.write_image_plane_module();

    // Write Image Pixel Module - PS 3.3 C.7.6.3
    image_ie.write_image_pixel_module();

    if(m_instance->get_sop_class_uid() == gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::CTImageStorage))
    {
        // Write CT Image Module - PS 3.3 C.8.2.1
        image_ie.write_ct_image_module();
    }
    else if(m_instance->get_sop_class_uid() == gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::MRImageStorage))
    {
        // Write MR Image Module - PS 3.3 C.8.3.1
        image_ie.write_mr_image_module();
    }

    // Write VOI LUT Module - PS 3.3 C.11.2
    image_ie.write_voilut_module();

    // Write SOP Common Module - PS 3.3 C.12.1
    image_ie.write_sop_common_module();

    // Copy dataset to avoid writing conflict with GDCM
    const gdcm::DataSet dataset_copy = writer->GetFile().GetDataSet();

    // Compute number of frames
    std::size_t nb_frames = (m_instance->get_is_multi_files()) ? (image_series->size()[2]) : 1;

    // Write specific tags according to frame number
    for(unsigned int i = 0 ; i < nb_frames ; ++i)
    {
        // Reset dataset
        writer->GetFile().SetDataSet(dataset_copy);

        // Write SOP Common Module specific tags - PS 3.3 C.12.1
        image_ie.write_sop_common_module_specific_tags(i);

        // Write General Image Module specific tags - PS 3.3 C.7.6.1
        image_ie.write_general_image_module_specific_tags(i);

        // Write Image Plane Module specific tags - PS 3.3 C.7.6.2
        image_ie.write_image_plane_module_specific_tags(i);

        // Write Image Pixel Module specific tags - PS 3.3 C.7.6.3
        image_ie.write_image_pixel_module_specific_tags(i);

        // Write file
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(5) << i;
        auto frame_path = m_destination_path;
        frame_path += ss.str();
        io::dicom::helper::file_writer::write(frame_path, writer);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::iod
