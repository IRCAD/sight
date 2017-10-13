/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/exception/Failed.hpp"
#include "fwGdcmIO/helper/FileWriter.hpp"
#include "fwGdcmIO/writer/ie/Equipment.hpp"
#include "fwGdcmIO/writer/ie/FrameOfReference.hpp"
#include "fwGdcmIO/writer/ie/Patient.hpp"
#include "fwGdcmIO/writer/ie/Series.hpp"
#include "fwGdcmIO/writer/ie/Study.hpp"
#include "fwGdcmIO/writer/ie/Surface.hpp"
#include "fwGdcmIO/writer/iod/SurfaceSegmentationIOD.hpp"

#include <fwCore/spyLog.hpp>

#include <fwRuntime/operations.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include <gdcmSurfaceWriter.h>

#include <boost/make_shared.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace iod
{

//------------------------------------------------------------------------------

SurfaceSegmentationIOD::SurfaceSegmentationIOD(const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                               const SPTR(::fwGdcmIO::container::DicomInstance)& imageInstance,
                                               const ::boost::filesystem::path& destinationPath,
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

void SurfaceSegmentationIOD::write(const ::fwMedData::Series::sptr& series)
{
    // Retrieve model series
    ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::dynamicCast(series);
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
    const ::boost::filesystem::path filepath = ::fwRuntime::getLibraryResourceFilePath("fwGdcmIO-" FWGDCMIO_VER "/SegmentedPropertyRegistry.csv");

    if(!surfaceIE.loadSegmentedPropertyRegistry(filepath))
    {
        throw  ::fwGdcmIO::exception::Failed("Unable to load segmented property registry: '" +
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
