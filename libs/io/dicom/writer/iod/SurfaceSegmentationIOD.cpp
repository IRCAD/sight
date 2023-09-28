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
#include "io/dicom/writer/ie/Series.hpp"
#include "io/dicom/writer/ie/Study.hpp"
#include "io/dicom/writer/ie/Surface.hpp"

#include <core/runtime/path.hpp>
#include <core/spy_log.hpp>

#include <data/ModelSeries.hpp>

#include <gdcmSurfaceWriter.h>

namespace sight::io::dicom::writer::iod
{

//------------------------------------------------------------------------------

SurfaceSegmentationIOD::SurfaceSegmentationIOD(
    const SPTR(io::dicom::container::DicomInstance)& instance,
    SPTR(io::dicom::container::DicomInstance)imageInstance,
    const std::filesystem::path& destinationPath,
    const core::log::logger::sptr& logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    io::dicom::writer::iod::InformationObjectDefinition(instance,
                                                        destinationPath,
                                                        logger,
                                                        std::move(progress),
                                                        std::move(cancel)),
    m_imageInstance(std::move(imageInstance))
{
}

//------------------------------------------------------------------------------

SurfaceSegmentationIOD::~SurfaceSegmentationIOD()
= default;

//------------------------------------------------------------------------------

void SurfaceSegmentationIOD::write(const data::Series::csptr& series)
{
    // Retrieve model series
    data::ModelSeries::csptr modelSeries = std::dynamic_pointer_cast<const data::ModelSeries>(series);
    SIGHT_ASSERT("Image series should not be null.", modelSeries);

    // Create writer
    SPTR(gdcm::SurfaceWriter) writer = std::make_shared<gdcm::SurfaceWriter>();

    // Create Information Entity helpers
    io::dicom::writer::ie::Patient patientIE(writer, m_instance, series);
    io::dicom::writer::ie::Study studyIE(writer, m_instance, series);
    io::dicom::writer::ie::Series seriesIE(writer, m_instance, series);
    // Use Image as frame of reference
    io::dicom::writer::ie::FrameOfReference frameOfReferenceIE(writer, m_imageInstance, series);
    io::dicom::writer::ie::Equipment equipmentIE(writer, m_instance, series);
    io::dicom::writer::ie::Surface surfaceIE(writer, m_instance, m_imageInstance, modelSeries, m_logger);

    // Load Segmented Property Registry
    const std::filesystem::path filepath = core::runtime::get_library_resource_file_path(
        "io_dicom/SegmentedPropertyRegistry.csv"
    );

    if(!surfaceIE.loadSegmentedPropertyRegistry(filepath))
    {
        throw io::dicom::exception::Failed(
                  "Unable to load segmented property registry: '"
                  + filepath.string() + "'. File does not exist."
        );
    }

    // Write Patient Module - PS 3.3 C.7.1.1
    patientIE.writePatientModule();

    // Write General Study Module - PS 3.3 C.7.2.1
    studyIE.writeGeneralStudyModule();

    // Write Patient Study Module - PS 3.3 C.7.2.2
    studyIE.writePatientStudyModule();

    // Write General Series Module - PS 3.3 C.7.3.1
    seriesIE.writeGeneralSeriesModule();

    // Write General Series Module - PS 3.3 C.8.20.1
    seriesIE.writeSegmentationSeriesModule();

    // Write Frame of Reference Module - PS 3.3 C.7.4.1
    frameOfReferenceIE.writeFrameOfReferenceModule();

    // Write General Equipment Module - PS 3.3 C.7.5.1
    equipmentIE.writeGeneralEquipmentModule();

    // Write Enhanced General Equipment Module - PS 3.3 C.7.5.2
    equipmentIE.writeEnhancedGeneralEquipmentModule();

    // Write SOP Common Module - PS 3.3 C.12.1
    surfaceIE.writeSOPCommonModule();

    // Write Surface Segmentation Module - PS 3.3 C.8.23.1
    // And Surface Mesh Module - PS 3.3 C.27.1
    surfaceIE.writeSurfaceSegmentationAndSurfaceMeshModules();

    // Write the file
    if((!m_cancelRequestedCallback || !m_cancelRequestedCallback())
       && (!m_logger || (m_logger->count(core::log::log::CRITICAL) == 0U)))
    {
        io::dicom::helper::FileWriter::write(m_destinationPath, writer);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::iod
