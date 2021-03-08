/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/dicom/reader/iod/SurfaceSegmentationIOD.hpp"

#include "io/dicom/reader/ie/Equipment.hpp"
#include "io/dicom/reader/ie/Patient.hpp"
#include "io/dicom/reader/ie/Series.hpp"
#include "io/dicom/reader/ie/Study.hpp"
#include "io/dicom/reader/ie/Surface.hpp"

#include <core/runtime/operations.hpp>

#include <data/Boolean.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>

#include <gdcmSegmentReader.h>
#include <gdcmSurfaceHelper.h>
#include <gdcmSurfaceReader.h>

namespace sight::io::dicom
{
namespace reader
{
namespace iod
{

//------------------------------------------------------------------------------

SurfaceSegmentationIOD::SurfaceSegmentationIOD(const data::DicomSeries::csptr& dicomSeries,
                                               const SPTR(io::dicom::container::DicomInstance)& instance,
                                               const core::log::Logger::sptr& logger,
                                               ProgressCallback progress,
                                               CancelRequestedCallback cancel) :
    io::dicom::reader::iod::InformationObjectDefinition(dicomSeries, instance, logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

SurfaceSegmentationIOD::~SurfaceSegmentationIOD()
{
}

//------------------------------------------------------------------------------

void SurfaceSegmentationIOD::read(data::Series::sptr series)
{
    data::ModelSeries::sptr modelSeries = data::ModelSeries::dynamicCast(series);
    SIGHT_ASSERT("ModelSeries should not be null.", modelSeries);

    // Create GDCM Reader
    SPTR(::gdcm::SurfaceReader) reader = std::shared_ptr< ::gdcm::SurfaceReader >( new ::gdcm::SurfaceReader );

    // Dicom container
    data::DicomSeries::DicomContainerType dicomContainer = m_dicomSeries->getDicomContainer();
    if(dicomContainer.size() > 1)
    {
        m_logger->warning("More than one surface segmentation storage have been found in the series. "
                          "Only the first one will be read.");
    }

    // Read first file
    const core::memory::BufferObject::sptr bufferObj         = dicomContainer.begin()->second;
    const core::memory::BufferManager::StreamInfo streamInfo = bufferObj->getStreamInfo();
    SPTR(std::istream) is = streamInfo.stream;
    reader->SetStream(*is);

    const bool success = reader->Read();
    SIGHT_THROW_EXCEPTION_IF(io::dicom::exception::Failed("Unable to read the DICOM instance \""+
                                                          bufferObj->getStreamInfo().fsFile.string()+
                                                          "\" using the GDCM Reader."), !success);

    // Create Information Entity helpers
    io::dicom::reader::ie::Patient patientIE(m_dicomSeries, reader, m_instance, series->getPatient(), m_logger,
                                             m_progressCallback, m_cancelRequestedCallback);
    io::dicom::reader::ie::Study studyIE(m_dicomSeries, reader, m_instance, series->getStudy(), m_logger,
                                         m_progressCallback, m_cancelRequestedCallback);
    io::dicom::reader::ie::Series seriesIE(m_dicomSeries, reader, m_instance, series, m_logger,
                                           m_progressCallback, m_cancelRequestedCallback);
    // Use Image as frame of reference
    io::dicom::reader::ie::Equipment equipmentIE(m_dicomSeries, reader, m_instance, series->getEquipment(), m_logger,
                                                 m_progressCallback, m_cancelRequestedCallback);
    io::dicom::reader::ie::Surface surfaceIE(m_dicomSeries, reader, m_instance, modelSeries, m_logger,
                                             m_progressCallback, m_cancelRequestedCallback);

    // Load Segmented Property Registry
    const std::filesystem::path filepath = core::runtime::getLibraryResourceFilePath(
        "io_dicom/SegmentedPropertyRegistry.csv");
    if(!surfaceIE.loadSegmentedPropertyRegistry(filepath))
    {
        throw io::dicom::exception::Failed("Unable to load segmented property registry: '" +
                                           filepath.string() + "'. File does not exist.");
    }

    // Read Patient Module - PS 3.3 C.7.1.1
    patientIE.readPatientModule();

    // Read General Study Module - PS 3.3 C.7.2.1
    studyIE.readGeneralStudyModule();

    // Read Patient Study Module - PS 3.3 C.7.2.2
    studyIE.readPatientStudyModule();

    // Read General Series Module - PS 3.3 C.7.3.1
    seriesIE.readGeneralSeriesModule();

    // Read General Series Module - PS 3.3 C.8.20.1
    // NOTE: Not used in Sight

    // Read Frame of Reference Module - PS 3.3 C.7.4.1
    // NOTE: Not used in Sight

    // Read General Equipment Module - PS 3.3 C.7.5.1
    equipmentIE.readGeneralEquipmentModule();

    // Read SOP Common Module - PS 3.3 C.12.1
    // NOTE: Not used in Sight

    // Read Surface Segmentation Module - PS 3.3 C.8.23.1
    // And Surface Mesh Module - PS 3.3 C.27.1
    surfaceIE.readSurfaceSegmentationAndSurfaceMeshModules();

    // Display reconstructions
    series->setField("ShowReconstructions", data::Boolean::New(true));

}

//------------------------------------------------------------------------------

}  // namespace iod
}  // namespace reader
}  // namespace sight::io::dicom
