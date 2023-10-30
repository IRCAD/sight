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

#include "io/dicom/writer/iod/ComprehensiveSRIOD.hpp"

#include "io/dicom/helper/FileWriter.hpp"
#include "io/dicom/writer/ie/Document.hpp"
#include "io/dicom/writer/ie/Equipment.hpp"
#include "io/dicom/writer/ie/image.hpp"
#include "io/dicom/writer/ie/Patient.hpp"
#include "io/dicom/writer/ie/series.hpp"
#include "io/dicom/writer/ie/Study.hpp"

#include <core/spy_log.hpp>

#include <data/image.hpp>
#include <data/image_series.hpp>

#include <gdcmWriter.h>

namespace sight::io::dicom::writer::iod
{

//------------------------------------------------------------------------------

comprehensive_sriod::comprehensive_sriod(
    const SPTR(io::dicom::container::dicom_instance)& _instance,
    const std::filesystem::path& _destination_path,
    bool _use_3d_sr,
    const core::log::logger::sptr& _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    io::dicom::writer::iod::information_object_definition(_instance, _destination_path, _logger, _progress, _cancel),
    m_use_3d_sr(_use_3d_sr)
{
}

//------------------------------------------------------------------------------

comprehensive_sriod::~comprehensive_sriod()
= default;

//------------------------------------------------------------------------------

void comprehensive_sriod::write(const data::series::csptr& _series)
{
    // Retrieve image series
    data::image_series::csptr image_series = std::dynamic_pointer_cast<const data::image_series>(_series);
    SIGHT_ASSERT("Image series should not be null.", image_series);

    // Create writer
    SPTR(gdcm::Writer) writer = std::make_shared<gdcm::Writer>();

    // Create Information Entity helpers
    io::dicom::writer::ie::patient patient_ie(writer, m_instance, _series);
    io::dicom::writer::ie::study study_ie(writer, m_instance, _series);
    io::dicom::writer::ie::series series_ie(writer, m_instance, _series);
    io::dicom::writer::ie::equipment equipment_ie(writer, m_instance, _series);
    io::dicom::writer::ie::document document_ie(writer, m_instance, image_series, m_use_3d_sr);

    // Write Patient Module - PS 3.3 C.7.1.1
    patient_ie.write_patient_module();

    // Write General Study Module - PS 3.3 C.7.2.1
    study_ie.write_general_study_module();

    // Write Patient Study Module - PS 3.3 C.7.2.2
    study_ie.write_patient_study_module();

    // Write SR Document Series Module - PS 3.3 C.17.1
    series_ie.write_sr_document_series_module();

    // Write General Equipment Module - PS 3.3 C.7.5.1
    equipment_ie.write_general_equipment_module();

    // Write SR Document General Module - PS 3.3 C.17.2
    document_ie.write_sr_document_general_module();

    // Write SR Document Content Module - PS 3.3 C.17.3
    document_ie.write_sr_document_content_module();

    // Write SOP Common module - PS 3.3 C.12.1
    document_ie.write_sop_common_module();

    // Write document
    io::dicom::helper::file_writer::write(m_destination_path, writer);
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::iod
