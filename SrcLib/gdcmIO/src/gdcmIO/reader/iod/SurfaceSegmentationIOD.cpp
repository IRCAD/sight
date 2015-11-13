/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <gdcmSegmentReader.h>
#include <gdcmSurfaceReader.h>
#include <gdcmSurfaceHelper.h>

#include <fwData/Boolean.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwMedData/ModelSeries.hpp>

#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/reader/ie/Patient.hpp"
#include "gdcmIO/reader/ie/Study.hpp"
#include "gdcmIO/reader/ie/Series.hpp"
#include "gdcmIO/reader/ie/Equipment.hpp"
#include "gdcmIO/reader/ie/Surface.hpp"

#include "gdcmIO/reader/iod/SurfaceSegmentationIOD.hpp"

namespace gdcmIO
{
namespace reader
{
namespace iod
{

//------------------------------------------------------------------------------

SurfaceSegmentationIOD::SurfaceSegmentationIOD(::fwDicomData::DicomSeries::sptr dicomSeries,
                                               SPTR(::gdcmIO::container::DicomInstance)instance) :
    ::gdcmIO::reader::iod::InformationObjectDefinition(dicomSeries, instance)
{
}

//------------------------------------------------------------------------------

SurfaceSegmentationIOD::~SurfaceSegmentationIOD()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SurfaceSegmentationIOD::read(::fwMedData::Series::sptr series) throw(::gdcmIO::exception::Failed)
{
    ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::dynamicCast(series);
    SLM_ASSERT("ModelSeries should not be null.", modelSeries);

    // Create GDCM Reader
    SPTR(::gdcm::SurfaceReader) reader = std::shared_ptr< ::gdcm::SurfaceReader >( new ::gdcm::SurfaceReader );

    // Path container
    ::fwDicomData::DicomSeries::DicomPathContainerType pathContainer = m_dicomSeries->getLocalDicomPaths();
    OSLM_WARN_IF("More than one surface segmentation storage was found in series. Only the first one will be read.",
                 pathContainer.size() > 1);

    // Read first file
    const std::string filename = pathContainer.begin()->second.string();
    reader->SetFileName( filename.c_str() );
    bool success = reader->Read();
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Unable to read the DICOM instance using the GDCM Image Reader."),
                          !success);

    // Create Information Entity helpers
    ::gdcmIO::reader::ie::Patient patientIE(m_dicomSeries, reader, m_instance, series->getPatient());
    ::gdcmIO::reader::ie::Study studyIE(m_dicomSeries, reader, m_instance, series->getStudy());
    ::gdcmIO::reader::ie::Series seriesIE(m_dicomSeries, reader, m_instance, series);
    // Use Image as frame of reference
    ::gdcmIO::reader::ie::Equipment equipmentIE(m_dicomSeries, reader, m_instance, series->getEquipment());
    ::gdcmIO::reader::ie::Surface surfaceIE(m_dicomSeries, reader, m_instance, modelSeries);

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

    // Skipped segmentation count
    unsigned int skippedSegmentationCount = 0;

    // Read each surface segmentation
    const ::gdcm::SegmentReader::SegmentVector &segmentContainer = reader->GetSegments();
    for(::gdcm::SmartPointer< ::gdcm::Segment > segment :  segmentContainer)
    {
        try
        {
            // Read Surface Segmentation & Surface Mesh Module - PS 3.3 C.8.23.1 & C.27.1
            surfaceIE.readSurfaceSegmentationModule(segment);
        }
        catch (::gdcmIO::exception::Failed & e)
        {
            ++skippedSegmentationCount;
            SLM_ERROR("Surface mesh reading error : " + std::string(e.what()));
        }
    }

    if (skippedSegmentationCount == modelSeries->getReconstructionDB().size())
    {
        throw ::gdcmIO::exception::Failed("All 3D reconstructions have been rejected.");
    }
    else if (skippedSegmentationCount > 0)
    {
        OSLM_WARN(skippedSegmentationCount<<" 3D reconstruction(s) have been rejected.");
    }

    OSLM_TRACE("Number of reconstructions : " << modelSeries->getReconstructionDB().size());

    // Display reconstructions
    series->setField("ShowReconstructions", ::fwData::Boolean::New(true));
}

//------------------------------------------------------------------------------


}  // namespace iod
}  // namespace reader
}  // namespace gdcmIO
