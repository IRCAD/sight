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

ComprehensiveSRIOD::ComprehensiveSRIOD(
    const SPTR(io::dicom::container::DicomInstance)& instance,
    const std::filesystem::path& destinationPath,
    bool use3DSR,
    const core::log::logger::sptr& logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    io::dicom::writer::iod::InformationObjectDefinition(instance, destinationPath, logger, progress, cancel),
    m_use3DSR(use3DSR)
{
}

//------------------------------------------------------------------------------

ComprehensiveSRIOD::~ComprehensiveSRIOD()
= default;

//------------------------------------------------------------------------------

void ComprehensiveSRIOD::write(const data::series::csptr& series)
{
    // Retrieve image series
    data::image_series::csptr imageSeries = std::dynamic_pointer_cast<const data::image_series>(series);
    SIGHT_ASSERT("Image series should not be null.", imageSeries);

    // Create writer
    SPTR(gdcm::Writer) writer = std::make_shared<gdcm::Writer>();

    // Create Information Entity helpers
    io::dicom::writer::ie::Patient patientIE(writer, m_instance, series);
    io::dicom::writer::ie::Study studyIE(writer, m_instance, series);
    io::dicom::writer::ie::series seriesIE(writer, m_instance, series);
    io::dicom::writer::ie::Equipment equipmentIE(writer, m_instance, series);
    io::dicom::writer::ie::Document documentIE(writer, m_instance, imageSeries, m_use3DSR);

    // Write Patient Module - PS 3.3 C.7.1.1
    patientIE.writePatientModule();

    // Write General Study Module - PS 3.3 C.7.2.1
    studyIE.writeGeneralStudyModule();

    // Write Patient Study Module - PS 3.3 C.7.2.2
    studyIE.writePatientStudyModule();

    // Write SR Document Series Module - PS 3.3 C.17.1
    seriesIE.writeSRDocumentSeriesModule();

    // Write General Equipment Module - PS 3.3 C.7.5.1
    equipmentIE.writeGeneralEquipmentModule();

    // Write SR Document General Module - PS 3.3 C.17.2
    documentIE.writeSRDocumentGeneralModule();

    // Write SR Document Content Module - PS 3.3 C.17.3
    documentIE.writeSRDocumentContentModule();

    // Write SOP Common module - PS 3.3 C.12.1
    documentIE.writeSOPCommonModule();

    // Write document
    io::dicom::helper::FileWriter::write(m_destinationPath, writer);
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::iod
