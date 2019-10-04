/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwGdcmIO/writer/iod/SurfaceSegmentationIOD.hpp"

#include "fwGdcmIO/exception/Failed.hpp"
#include "fwGdcmIO/helper/FileWriter.hpp"
#include "fwGdcmIO/writer/ie/Equipment.hpp"
#include "fwGdcmIO/writer/ie/FrameOfReference.hpp"
#include "fwGdcmIO/writer/ie/Patient.hpp"
#include "fwGdcmIO/writer/ie/Series.hpp"
#include "fwGdcmIO/writer/ie/Study.hpp"
#include "fwGdcmIO/writer/ie/Surface.hpp"

#include <fwCore/spyLog.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include <fwRuntime/operations.hpp>

#include <boost/make_shared.hpp>

#include <gdcmSurfaceWriter.h>

namespace fwGdcmIO
{
namespace writer
{
namespace iod
{

//------------------------------------------------------------------------------

SurfaceSegmentationIOD::SurfaceSegmentationIOD(const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                               const SPTR(::fwGdcmIO::container::DicomInstance)& imageInstance,
                                               const std::filesystem::path& destinationPath,
                                               const ::fwLog::Logger::sptr& logger,
                                               ProgressCallback progress,
                                               CancelRequestedCallback cancel) :
    ::fwGdcmIO::writer::iod::InformationObjectDefinition(instance, destinationPath, logger, progress, cancel),
    m_imageInstance(imageInstance)
{
}

//------------------------------------------------------------------------------

SurfaceSegmentationIOD::~SurfaceSegmentationIOD()
{
}

//------------------------------------------------------------------------------

void SurfaceSegmentationIOD::write(const ::fwMedData::Series::csptr& series)
{
    // Retrieve model series
    ::fwMedData::ModelSeries::csptr modelSeries = ::fwMedData::ModelSeries::dynamicCast(series);
    SLM_ASSERT("Image series should not be null.", modelSeries);

    // Create writer
    SPTR(::gdcm::SurfaceWriter) writer = std::make_shared< ::gdcm::SurfaceWriter >();

    // Create Information Entity helpers
    ::fwGdcmIO::writer::ie::Patient patientIE(writer, m_instance, series->getPatient());
    ::fwGdcmIO::writer::ie::Study studyIE(writer, m_instance, series->getStudy());
    ::fwGdcmIO::writer::ie::Series seriesIE(writer, m_instance, series);
    // Use Image as frame of reference
    ::fwGdcmIO::writer::ie::FrameOfReference frameOfReferenceIE(writer, m_imageInstance, series);
    ::fwGdcmIO::writer::ie::Equipment equipmentIE(writer, m_instance, series->getEquipment());
    ::fwGdcmIO::writer::ie::Surface surfaceIE(writer, m_instance, m_imageInstance, modelSeries, m_logger);

    // Load Segmented Property Registry
    const std::filesystem::path filepath = ::fwRuntime::getLibraryResourceFilePath(
        "fwGdcmIO-" FWGDCMIO_VER "/SegmentedPropertyRegistry.csv");

    if(!surfaceIE.loadSegmentedPropertyRegistry(filepath))
    {
        throw ::fwGdcmIO::exception::Failed("Unable to load segmented property registry: '" +
                                            filepath.string() + "'. File does not exist.");
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
    if((!m_cancelRequestedCallback || !m_cancelRequestedCallback()) &&
       (!m_logger || !m_logger->count(::fwLog::Log::CRITICAL)))
    {
        ::fwGdcmIO::helper::FileWriter::write(m_destinationPath, writer);
    }
}

//------------------------------------------------------------------------------

} // namespace iod
} // namespace writer
} // namespace fwGdcmIO
