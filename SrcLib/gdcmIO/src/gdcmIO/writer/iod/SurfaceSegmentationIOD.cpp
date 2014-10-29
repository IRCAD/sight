/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/make_shared.hpp>

#include <gdcmSurfaceWriter.h>

#include <fwCore/spyLog.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Study.hpp>

#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/helper/FileWriter.hpp"
#include "gdcmIO/writer/ie/Patient.hpp"
#include "gdcmIO/writer/ie/Study.hpp"
#include "gdcmIO/writer/ie/Equipment.hpp"
#include "gdcmIO/writer/ie/Series.hpp"
#include "gdcmIO/writer/ie/Surface.hpp"
#include "gdcmIO/writer/ie/FrameOfReference.hpp"


#include "gdcmIO/writer/iod/SurfaceSegmentationIOD.hpp"

namespace gdcmIO
{
namespace writer
{
namespace iod
{

//------------------------------------------------------------------------------

SurfaceSegmentationIOD::SurfaceSegmentationIOD(SPTR(::gdcmIO::container::DicomInstance) instance,
        SPTR(::gdcmIO::container::DicomInstance) imageInstance, ::boost::filesystem::path folderPath):
        ::gdcmIO::writer::iod::InformationObjectDefinition(instance, folderPath), m_imageInstance(imageInstance)
{
}

//------------------------------------------------------------------------------

SurfaceSegmentationIOD::~SurfaceSegmentationIOD()
{
}

//------------------------------------------------------------------------------

void SurfaceSegmentationIOD::write(::fwMedData::Series::sptr series)
{
    // Retrieve model series
    ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::dynamicCast(series);
    SLM_ASSERT("Image series should not be null.", modelSeries);

    // Create writer
    SPTR(::gdcm::SurfaceWriter) writer = ::boost::make_shared< ::gdcm::SurfaceWriter >();


    // Create Information Entity helpers
    ::gdcmIO::writer::ie::Patient patientIE(writer, m_instance, series->getPatient());
    ::gdcmIO::writer::ie::Study studyIE(writer, m_instance, series->getStudy());
    ::gdcmIO::writer::ie::Series seriesIE(writer, m_instance, series);
    // Use Image as frame of reference
    ::gdcmIO::writer::ie::FrameOfReference frameOfReferenceIE(writer, m_imageInstance, series);
    ::gdcmIO::writer::ie::Equipment equipmentIE(writer, m_instance, series->getEquipment());
    ::gdcmIO::writer::ie::Surface surfaceIE(writer, m_instance, m_imageInstance, modelSeries);

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

    // Copy dataset to avoid writing conflict with GDCM
    const ::gdcm::DataSet datasetCopy = writer->GetFile().GetDataSet();

    // Write SOP Common Module - PS 3.3 C.12.1
    surfaceIE.writeSOPCommonModule();

    // Skipped segmentation count
    unsigned int skippedSegmentationCount = 0;

    // Write surface segmentations
    for(unsigned int index = 0; index < modelSeries->getReconstructionDB().size(); ++index)
    {
        try
        {
            // Write Surface Segmentation Module - PS 3.3 C.8.23.1
            surfaceIE.writeSurfaceSegmentationModule(index);

            // Write Surface Mesh Module - PS 3.3 C.27.1
            surfaceIE.writeSurfaceMeshModule(index);

        }
        catch (::gdcmIO::exception::Failed & e)
        {
            ++skippedSegmentationCount;
            SLM_ERROR(e.what());
        }
    }

    if (skippedSegmentationCount == modelSeries->getReconstructionDB().size())
    {
        throw ::gdcmIO::exception::Failed("All 3D reconstructions have been rejected");
    }
    else if (skippedSegmentationCount > 0)
    {
        OSLM_WARN(skippedSegmentationCount<<" 3D reconstruction(s) have been rejected");
    }


    // Number of Surfaces Tag(0x0066,0x0001) - Type 1
    writer->SetNumberOfSurfaces(modelSeries->getReconstructionDB().size() - skippedSegmentationCount);
    OSLM_TRACE("Number of Surfaces : " << writer->GetNumberOfSurfaces());

    // Write the file
    ::gdcmIO::helper::FileWriter::write(m_folderPath.string() + "/imRT", writer);


}


} // namespace iod
} // namespace writer
} // namespace gdcmIO
