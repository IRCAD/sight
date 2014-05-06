/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <gdcmImageReader.h>

#include <fwMedData/ImageSeries.hpp>

#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/reader/ie/Patient.hpp"
#include "gdcmIO/reader/ie/Study.hpp"
#include "gdcmIO/reader/ie/Series.hpp"
#include "gdcmIO/reader/ie/Equipment.hpp"
#include "gdcmIO/reader/ie/Image.hpp"

#include "gdcmIO/reader/iod/CTMRImageIOD.hpp"

namespace gdcmIO
{
namespace reader
{
namespace iod
{

//------------------------------------------------------------------------------

CTMRImageIOD::CTMRImageIOD(::fwDicomData::DicomSeries::sptr dicomSeries,
        SPTR(::gdcmIO::container::DicomInstance) instance) :
        ::gdcmIO::reader::iod::InformationObjectDefinition(dicomSeries, instance)
{
}

//------------------------------------------------------------------------------

CTMRImageIOD::~CTMRImageIOD()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void CTMRImageIOD::read(::fwMedData::Series::sptr series) throw(::gdcmIO::exception::Failed)
{
    SLM_TRACE_FUNC();

    // Retrieve image series
    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    SLM_ASSERT("Image series should not be null.", imageSeries);

    // Create GDCM reader
    SPTR(::gdcm::ImageReader) reader = ::boost::shared_ptr< ::gdcm::ImageReader >( new ::gdcm::ImageReader );

    // Read the first file
    ::fwDicomData::DicomSeries::DicomPathContainerType pathContainer = m_dicomSeries->getLocalDicomPaths();
    const std::string filename = pathContainer.begin()->second.string();
    reader->SetFileName( filename.c_str() );
    bool success = reader->Read();
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Unable to read the DICOM instance using the GDCM Image Reader."),
            !success);

    // Create Information Entity helpers
    ::gdcmIO::reader::ie::Patient patientIE(m_dicomSeries, reader, m_instance, series->getPatient());
    ::gdcmIO::reader::ie::Study studyIE(m_dicomSeries, reader, m_instance, series->getStudy());
    ::gdcmIO::reader::ie::Series seriesIE(m_dicomSeries, reader, m_instance, series);
    ::gdcmIO::reader::ie::Equipment equipmentIE(m_dicomSeries, reader, m_instance, series->getEquipment());
    ::gdcmIO::reader::ie::Image imageIE(m_dicomSeries, reader, m_instance, imageSeries->getImage());

    // Read Patient Module - PS 3.3 C.7.1.1
    patientIE.readPatientModule();

    // Read General Study Module - PS 3.3 C.7.2.1
    studyIE.readGeneralStudyModule();

    // Read Patient Study Module - PS 3.3 C.7.2.2
    studyIE.readPatientStudyModule();

    // Read General Series Module - PS 3.3 C.7.3.1
    seriesIE.readGeneralSeriesModule();

    // Read Frame of Reference Module - PS 3.3 C.7.4.1
    // NOTE: Not used in FW4SPL

    // Read General Equipment Module - PS 3.3 C.7.5.1
    equipmentIE.readGeneralEquipmentModule();

    // Read General Image Module - PS 3.3 C.7.6.1
    // NOTE: Not used in FW4SPL

    // Read Image Plane Module - PS 3.3 C.7.6.2
    imageIE.readImagePlaneModule();

    // Read Image Pixel Module - PS 3.3 C.7.6.3
    imageIE.readImagePixelModule();

    // Read CT & MR Image Module - PS 3.3 C.8.2.1 - PS 3.3 C.8.3.1
    // NOTE: Not used in FW4SPL

    // Read VOI LUT Module - PS 3.3 C.11.2
    imageIE.readVOILUTModule();

    // Read SOP Common Module - PS 3.3 C.12.1
    // NOTE: Not used in FW4SPL

}

//------------------------------------------------------------------------------

}  // namespace iod
}  // namespace reader
}  // namespace gdcmIO
