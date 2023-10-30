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

#include "io/dicom/reader/iod/CTMRImageIOD.hpp"

#include "io/dicom/helper/DicomDataReader.hxx"
#include "io/dicom/reader/ie/Equipment.hpp"
#include "io/dicom/reader/ie/image.hpp"
#include "io/dicom/reader/ie/Patient.hpp"
#include "io/dicom/reader/ie/series.hpp"
#include "io/dicom/reader/ie/Study.hpp"

#include <data/image_series.hpp>

#include <gdcmImageReader.h>

#include <memory>
#include <utility>

namespace sight::io::dicom::reader::iod
{

//------------------------------------------------------------------------------

ctmr_image_iod::ctmr_image_iod(
    const data::dicom_series::csptr& _dicom_series,
    const SPTR(io::dicom::container::dicom_instance)& _instance,
    const core::log::logger::sptr& _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    io::dicom::reader::iod::information_object_definition(_dicom_series, _instance, _logger, _progress, _cancel)
{
}

//------------------------------------------------------------------------------

ctmr_image_iod::~ctmr_image_iod()
= default;

//------------------------------------------------------------------------------

void ctmr_image_iod::read(data::series::sptr _series)
{
    // Retrieve image series
    data::image_series::sptr image_series = std::dynamic_pointer_cast<data::image_series>(_series);
    SIGHT_ASSERT("Image series should not be null.", image_series);

    // Create GDCM reader
    SPTR(gdcm::ImageReader) reader = std::make_shared<gdcm::ImageReader>();

    // Read the first file
    const core::memory::buffer_object::sptr buffer_obj          = m_dicom_series->get_dicom_container().begin()->second;
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
    io::dicom::reader::ie::patient patient_ie(m_dicom_series, reader, m_instance, _series, m_logger,
                                              m_progress_callback, m_cancel_requested_callback);
    io::dicom::reader::ie::study study_ie(m_dicom_series, reader, m_instance, _series, m_logger,
                                          m_progress_callback, m_cancel_requested_callback);
    io::dicom::reader::ie::series series_ie(m_dicom_series, reader, m_instance, _series, m_logger, m_progress_callback,
                                            m_cancel_requested_callback);
    io::dicom::reader::ie::equipment equipment_ie(m_dicom_series, reader, m_instance, _series, m_logger,
                                                  m_progress_callback, m_cancel_requested_callback);
    io::dicom::reader::ie::image image_ie(m_dicom_series, reader, m_instance, image_series, m_logger,
                                          m_progress_callback,
                                          m_cancel_requested_callback);
    image_ie.set_buffer_rotation_enabled(m_enable_buffer_rotation);

    // Read Patient Module - PS 3.3 C.7.1.1
    patient_ie.read_patient_module();
    unsigned int progress = 3;
    m_progress_callback(progress);

    // Read General Study Module - PS 3.3 C.7.2.1
    study_ie.read_general_study_module();
    progress += 3;
    m_progress_callback(progress);

    // Read Patient Study Module - PS 3.3 C.7.2.2
    study_ie.read_patient_study_module();
    progress += 3;
    m_progress_callback(progress);

    // Read General Series Module - PS 3.3 C.7.3.1
    series_ie.read_general_series_module();
    progress += 3;
    m_progress_callback(progress);

    // Read Frame of Reference Module - PS 3.3 C.7.4.1
    // NOTE: Not used in Sight

    // Read General Equipment Module - PS 3.3 C.7.5.1
    equipment_ie.read_general_equipment_module();
    progress += 3;
    m_progress_callback(progress);

    // Read General Image Module - PS 3.3 C.7.6.1
    // NOTE: Not used in Sight

    // Read Image Plane Module - PS 3.3 C.7.6.2
    image_ie.read_image_plane_module();
    progress += 3;
    m_progress_callback(progress);

    // Read Image Pixel Module - PS 3.3 C.7.6.3
    image_ie.read_image_pixel_module();
    progress = 98;
    m_progress_callback(progress);

    // Read CT & MR Image Module - PS 3.3 C.8.2.1 - PS 3.3 C.8.3.1
    // NOTE: Not used in Sight

    // Read VOI LUT Module - PS 3.3 C.11.2
    image_ie.read_voilut_module();
    progress = 100;
    m_progress_callback(progress);

    // Read SOP Common Module - PS 3.3 C.12.1
    // NOTE: Not used in Sight
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::iod
