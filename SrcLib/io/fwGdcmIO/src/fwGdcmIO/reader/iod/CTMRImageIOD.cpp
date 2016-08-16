/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomData.hpp"
#include "fwGdcmIO/reader/ie/Equipment.hpp"
#include "fwGdcmIO/reader/ie/Image.hpp"
#include "fwGdcmIO/reader/ie/Patient.hpp"
#include "fwGdcmIO/reader/ie/Series.hpp"
#include "fwGdcmIO/reader/ie/Study.hpp"
#include "fwGdcmIO/reader/iod/CTMRImageIOD.hpp"

#include <fwCore/macros.hpp>

#include <fwMedData/ImageSeries.hpp>

#include <gdcmImageReader.h>

namespace fwGdcmIO
{
namespace reader
{
namespace iod
{

//------------------------------------------------------------------------------

CTMRImageIOD::CTMRImageIOD(::fwMedData::DicomSeries::sptr dicomSeries,
                           SPTR(::fwGdcmIO::container::DicomInstance)instance, ::fwLog::Logger::sptr logger,
                           std::function< void(unsigned int&) >& callback, bool& cancelled) :
    ::fwGdcmIO::reader::iod::InformationObjectDefinition(dicomSeries, instance, logger, callback, cancelled),
    m_enableBufferRotation(true)
{
}

//------------------------------------------------------------------------------

CTMRImageIOD::~CTMRImageIOD()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void CTMRImageIOD::read(::fwMedData::Series::sptr series) throw(::fwGdcmIO::exception::Failed)
{
    // Retrieve image series
    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    SLM_ASSERT("Image series should not be null.", imageSeries);

    // Create GDCM reader
    SPTR(::gdcm::ImageReader) reader = std::shared_ptr< ::gdcm::ImageReader >( new ::gdcm::ImageReader );

    // Read the first file
    ::fwMedData::DicomSeries::DicomPathContainerType pathContainer = m_dicomSeries->getLocalDicomPaths();
    const std::string filename = pathContainer.begin()->second.string();
    reader->SetFileName( filename.c_str() );
    bool success = reader->Read();
    FW_RAISE_EXCEPTION_IF(::fwGdcmIO::exception::Failed("Unable to read the DICOM instance \""+
                                                        filename+"\" using the GDCM Image Reader."),
                          !success);

    // Create Information Entity helpers
    ::fwGdcmIO::reader::ie::Patient patientIE(m_dicomSeries, reader, m_instance, series->getPatient(), m_logger,
                                              m_progressCallback, m_cancelled);
    ::fwGdcmIO::reader::ie::Study studyIE(m_dicomSeries, reader, m_instance, series->getStudy(), m_logger,
                                          m_progressCallback, m_cancelled);
    ::fwGdcmIO::reader::ie::Series seriesIE(m_dicomSeries, reader, m_instance, series, m_logger,
                                            m_progressCallback, m_cancelled);
    ::fwGdcmIO::reader::ie::Equipment equipmentIE(m_dicomSeries, reader, m_instance, series->getEquipment(), m_logger,
                                                  m_progressCallback, m_cancelled);
    ::fwGdcmIO::reader::ie::Image imageIE(m_dicomSeries, reader, m_instance, imageSeries->getImage(), m_logger,
                                          m_progressCallback, m_cancelled);
    imageIE.setBufferRotationEnabled(m_enableBufferRotation);

    // Read Patient Module - PS 3.3 C.7.1.1
    patientIE.readPatientModule();
    unsigned int progress = 3;
    m_progressCallback(progress);

    // Read General Study Module - PS 3.3 C.7.2.1
    studyIE.readGeneralStudyModule();
    progress += 3;
    m_progressCallback(progress);

    // Read Patient Study Module - PS 3.3 C.7.2.2
    studyIE.readPatientStudyModule();
    progress += 3;
    m_progressCallback(progress);

    // Read General Series Module - PS 3.3 C.7.3.1
    seriesIE.readGeneralSeriesModule();
    progress += 3;
    m_progressCallback(progress);

    // Read Frame of Reference Module - PS 3.3 C.7.4.1
    // NOTE: Not used in FW4SPL

    // Read General Equipment Module - PS 3.3 C.7.5.1
    equipmentIE.readGeneralEquipmentModule();
    progress += 3;
    m_progressCallback(progress);

    // Read General Image Module - PS 3.3 C.7.6.1
    // NOTE: Not used in FW4SPL

    // Read Image Plane Module - PS 3.3 C.7.6.2
    imageIE.readImagePlaneModule();
    progress += 3;
    m_progressCallback(progress);

    // Read Image Pixel Module - PS 3.3 C.7.6.3
    imageIE.readImagePixelModule();
    progress = 98;
    m_progressCallback(progress);

    // Read CT & MR Image Module - PS 3.3 C.8.2.1 - PS 3.3 C.8.3.1
    // NOTE: Not used in FW4SPL

    // Read VOI LUT Module - PS 3.3 C.11.2
    imageIE.readVOILUTModule();
    progress = 100;
    m_progressCallback(progress);

    // Read SOP Common Module - PS 3.3 C.12.1
    // NOTE: Not used in FW4SPL

}

//------------------------------------------------------------------------------

}  // namespace iod
}  // namespace reader
}  // namespace fwGdcmIO
