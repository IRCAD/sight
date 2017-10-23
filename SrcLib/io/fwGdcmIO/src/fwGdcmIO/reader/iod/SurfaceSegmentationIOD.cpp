/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/reader/iod/SurfaceSegmentationIOD.hpp"

#include "fwGdcmIO/reader/ie/Equipment.hpp"
#include "fwGdcmIO/reader/ie/Patient.hpp"
#include "fwGdcmIO/reader/ie/Series.hpp"
#include "fwGdcmIO/reader/ie/Study.hpp"
#include "fwGdcmIO/reader/ie/Surface.hpp"

#include <fwData/Boolean.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwRuntime/operations.hpp>

#include <gdcmSegmentReader.h>
#include <gdcmSurfaceHelper.h>
#include <gdcmSurfaceReader.h>

namespace fwGdcmIO
{
namespace reader
{
namespace iod
{

//------------------------------------------------------------------------------

SurfaceSegmentationIOD::SurfaceSegmentationIOD(const ::fwMedData::DicomSeries::sptr& dicomSeries,
                                               const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                               const ::fwLog::Logger::sptr& logger,
                                               ProgressCallback progress,
                                               CancelRequestedCallback cancel) :
    ::fwGdcmIO::reader::iod::InformationObjectDefinition(dicomSeries, instance, logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

SurfaceSegmentationIOD::~SurfaceSegmentationIOD()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SurfaceSegmentationIOD::read(::fwMedData::Series::sptr series) throw(::fwGdcmIO::exception::Failed)
{
    ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::dynamicCast(series);
    SLM_ASSERT("ModelSeries should not be null.", modelSeries);

    // Create GDCM Reader
    SPTR(::gdcm::SurfaceReader) reader = std::shared_ptr< ::gdcm::SurfaceReader >( new ::gdcm::SurfaceReader );

    // Path container
    ::fwMedData::DicomSeries::DicomPathContainerType pathContainer = m_dicomSeries->getLocalDicomPaths();
    if(pathContainer.size() > 1)
    {
        m_logger->warning("More than one surface segmentation storage have been found in the series. "
                          "Only the first one will be read.");
    }

    // Read first file
    const std::string filename = pathContainer.begin()->second.string();
    reader->SetFileName( filename.c_str() );
    bool success = reader->Read();
    FW_RAISE_EXCEPTION_IF(::fwGdcmIO::exception::Failed("Unable to read the DICOM instance \""+
                                                        filename+"\" using the GDCM Image Reader."), !success);

    // Create Information Entity helpers
    ::fwGdcmIO::reader::ie::Patient patientIE(m_dicomSeries, reader, m_instance, series->getPatient(), m_logger,
                                              m_progressCallback, m_cancelRequestedCallback);
    ::fwGdcmIO::reader::ie::Study studyIE(m_dicomSeries, reader, m_instance, series->getStudy(), m_logger,
                                          m_progressCallback, m_cancelRequestedCallback);
    ::fwGdcmIO::reader::ie::Series seriesIE(m_dicomSeries, reader, m_instance, series, m_logger,
                                            m_progressCallback, m_cancelRequestedCallback);
    // Use Image as frame of reference
    ::fwGdcmIO::reader::ie::Equipment equipmentIE(m_dicomSeries, reader, m_instance, series->getEquipment(), m_logger,
                                                  m_progressCallback, m_cancelRequestedCallback);
    ::fwGdcmIO::reader::ie::Surface surfaceIE(m_dicomSeries, reader, m_instance, modelSeries, m_logger,
                                              m_progressCallback, m_cancelRequestedCallback);

    // Load Segmented Property Registry
    const ::boost::filesystem::path filepath = ::fwRuntime::getLibraryResourceFilePath(
        "fwGdcmIO-" FWGDCMIO_VER "/SegmentedPropertyRegistry.csv");
    if(!surfaceIE.loadSegmentedPropertyRegistry(filepath))
    {
        throw  ::fwGdcmIO::exception::Failed("Unable to load segmented property registry: '" +
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
    // NOTE: Not used in FW4SPL

    // Read Frame of Reference Module - PS 3.3 C.7.4.1
    // NOTE: Not used in FW4SPL

    // Read General Equipment Module - PS 3.3 C.7.5.1
    equipmentIE.readGeneralEquipmentModule();

    // Read SOP Common Module - PS 3.3 C.12.1
    // NOTE: Not used in FW4SPL

    // Read Surface Segmentation Module - PS 3.3 C.8.23.1
    // And Surface Mesh Module - PS 3.3 C.27.1
    surfaceIE.readSurfaceSegmentationAndSurfaceMeshModules();

#if 0
// Skipped segmentation count
    unsigned int skippedSegmentationCount = 0;

    // Read each surface segmentation
    const ::gdcm::SegmentReader::SegmentVector& segmentContainer = reader->GetSegments();
    for(::gdcm::SmartPointer< ::gdcm::Segment > segment: segmentContainer)
    {
        try
        {
            // Read Surface Segmentation & Surface Mesh Module - PS 3.3 C.8.23.1 & C.27.1
            surfaceIE.readSurfaceSegmentationModule(segment);
        }
        catch (::fwGdcmIO::exception::Failed& e)
        {
            ++skippedSegmentationCount;
            m_logger->warning("Surface mesh reading error : " + std::string(e.what()));
        }
    }

    if (skippedSegmentationCount == modelSeries->getReconstructionDB().size())
    {
        m_logger->critical("All 3D reconstructions have been rejected.");
    }
    else if (skippedSegmentationCount > 0)
    {
        std::stringstream ss;
        ss << skippedSegmentationCount << " 3D reconstruction(s) have been rejected.";
        m_logger->critical(ss.str());
    }

    OSLM_TRACE("Number of reconstructions : " << modelSeries->getReconstructionDB().size());
#endif

    // Display reconstructions
    series->setField("ShowReconstructions", ::fwData::Boolean::New(true));

}

//------------------------------------------------------------------------------

}  // namespace iod
}  // namespace reader
}  // namespace fwGdcmIO
