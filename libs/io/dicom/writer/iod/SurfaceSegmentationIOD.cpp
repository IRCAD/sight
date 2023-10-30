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

#include "io/dicom/writer/iod/SurfaceSegmentationIOD.hpp"

#include "io/dicom/exception/Failed.hpp"
#include "io/dicom/helper/FileWriter.hpp"
#include "io/dicom/writer/ie/Equipment.hpp"
#include "io/dicom/writer/ie/FrameOfReference.hpp"
#include "io/dicom/writer/ie/Patient.hpp"
#include "io/dicom/writer/ie/series.hpp"
#include "io/dicom/writer/ie/Study.hpp"
#include "io/dicom/writer/ie/Surface.hpp"

#include <core/runtime/path.hpp>
#include <core/spy_log.hpp>

#include <data/model_series.hpp>

#include <gdcmSurfaceWriter.h>

namespace sight::io::dicom::writer::iod
{

//------------------------------------------------------------------------------

surface_segmentation_iod::surface_segmentation_iod(
    const SPTR(io::dicom::container::dicom_instance)& _instance,
    SPTR(io::dicom::container::dicom_instance)_image_instance,
    const std::filesystem::path& _destination_path,
    const core::log::logger::sptr& _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    io::dicom::writer::iod::information_object_definition(_instance,
                                                          _destination_path,
                                                          _logger,
                                                          std::move(_progress),
                                                          std::move(_cancel)),
    m_image_instance(std::move(_image_instance))
{
}

//------------------------------------------------------------------------------

surface_segmentation_iod::~surface_segmentation_iod()
= default;

//------------------------------------------------------------------------------

void surface_segmentation_iod::write(const data::series::csptr& _series)
{
    // Retrieve model series
    data::model_series::csptr model_series = std::dynamic_pointer_cast<const data::model_series>(_series);
    SIGHT_ASSERT("Image series should not be null.", model_series);

    // Create writer
    SPTR(gdcm::SurfaceWriter) writer = std::make_shared<gdcm::SurfaceWriter>();

    // Create Information Entity helpers
    io::dicom::writer::ie::patient patient_ie(writer, m_instance, _series);
    io::dicom::writer::ie::study study_ie(writer, m_instance, _series);
    io::dicom::writer::ie::series series_ie(writer, m_instance, _series);
    // Use Image as frame of reference
    io::dicom::writer::ie::frame_of_reference frame_of_reference_ie(writer, m_image_instance, _series);
    io::dicom::writer::ie::equipment equipment_ie(writer, m_instance, _series);
    io::dicom::writer::ie::surface surface_ie(writer, m_instance, m_image_instance, model_series, m_logger);

    // Load Segmented Property Registry
    const std::filesystem::path filepath = core::runtime::get_library_resource_file_path(
        "io_dicom/SegmentedPropertyRegistry.csv"
    );

    if(!surface_ie.load_segmented_property_registry(filepath))
    {
        throw io::dicom::exception::failed(
                  "Unable to load segmented property registry: '"
                  + filepath.string() + "'. File does not exist."
        );
    }

    // Write Patient Module - PS 3.3 C.7.1.1
    patient_ie.write_patient_module();

    // Write General Study Module - PS 3.3 C.7.2.1
    study_ie.write_general_study_module();

    // Write Patient Study Module - PS 3.3 C.7.2.2
    study_ie.write_patient_study_module();

    // Write General Series Module - PS 3.3 C.7.3.1
    series_ie.write_general_series_module();

    // Write General Series Module - PS 3.3 C.8.20.1
    series_ie.write_segmentation_series_module();

    // Write Frame of Reference Module - PS 3.3 C.7.4.1
    frame_of_reference_ie.write_frame_of_reference_module();

    // Write General Equipment Module - PS 3.3 C.7.5.1
    equipment_ie.write_general_equipment_module();

    // Write Enhanced General Equipment Module - PS 3.3 C.7.5.2
    equipment_ie.write_enhanced_general_equipment_module();

    // Write SOP Common Module - PS 3.3 C.12.1
    surface_ie.write_sop_common_module();

    // Write Surface Segmentation Module - PS 3.3 C.8.23.1
    // And Surface Mesh Module - PS 3.3 C.27.1
    surface_ie.write_surface_segmentation_and_surface_mesh_modules();

    // Write the file
    if((!m_cancel_requested_callback || !m_cancel_requested_callback())
       && (!m_logger || (m_logger->count(core::log::log::critical) == 0U)))
    {
        io::dicom::helper::file_writer::write(m_destination_path, writer);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::iod
