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

#include "fwGdcmIO/reader/iod/CTMRImageIOD.hpp"

#include "fwGdcmIO/helper/DicomDataReader.hxx"
#include "fwGdcmIO/reader/ie/Equipment.hpp"
#include "fwGdcmIO/reader/ie/Image.hpp"
#include "fwGdcmIO/reader/ie/Patient.hpp"
#include "fwGdcmIO/reader/ie/Series.hpp"
#include "fwGdcmIO/reader/ie/Study.hpp"

#include <fwMedData/ImageSeries.hpp>

#include <gdcmImageReader.h>

namespace fwGdcmIO
{
namespace reader
{
namespace iod
{

//------------------------------------------------------------------------------

CTMRImageIOD::CTMRImageIOD(const ::fwMedData::DicomSeries::csptr& dicomSeries,
                           const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                           const ::fwLog::Logger::sptr& logger,
                           ProgressCallback progress,
                           CancelRequestedCallback cancel) :
    ::fwGdcmIO::reader::iod::InformationObjectDefinition(dicomSeries, instance, logger, progress, cancel),
    m_enableBufferRotation(true)
{
}

//------------------------------------------------------------------------------

CTMRImageIOD::~CTMRImageIOD()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void CTMRImageIOD::read(::fwMedData::Series::sptr series)
{
    // Retrieve image series
    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    SLM_ASSERT("Image series should not be null.", imageSeries);

    // Create GDCM reader
    SPTR(::gdcm::ImageReader) reader = std::shared_ptr< ::gdcm::ImageReader >( new ::gdcm::ImageReader );

    // Read the first file
    const ::fwMemory::BufferObject::sptr bufferObj         = m_dicomSeries->getDicomContainer().begin()->second;
    const ::fwMemory::BufferManager::StreamInfo streamInfo = bufferObj->getStreamInfo();
    SPTR(std::istream) is = streamInfo.stream;
    reader->SetStream(*is);

    const bool success = reader->Read();
    FW_RAISE_EXCEPTION_IF(::fwGdcmIO::exception::Failed("Unable to read the DICOM instance \""+
                                                        bufferObj->getStreamInfo().fsFile.string()+
                                                        "\" using the GDCM Reader."), !success);

    // Create Information Entity helpers
    ::fwGdcmIO::reader::ie::Patient patientIE(m_dicomSeries, reader, m_instance, series->getPatient(), m_logger,
                                              m_progressCallback, m_cancelRequestedCallback);
    ::fwGdcmIO::reader::ie::Study studyIE(m_dicomSeries, reader, m_instance, series->getStudy(), m_logger,
                                          m_progressCallback, m_cancelRequestedCallback);
    ::fwGdcmIO::reader::ie::Series seriesIE(m_dicomSeries, reader, m_instance, series, m_logger,
                                            m_progressCallback, m_cancelRequestedCallback);
    ::fwGdcmIO::reader::ie::Equipment equipmentIE(m_dicomSeries, reader, m_instance, series->getEquipment(), m_logger,
                                                  m_progressCallback, m_cancelRequestedCallback);
    ::fwGdcmIO::reader::ie::Image imageIE(m_dicomSeries, reader, m_instance, imageSeries->getImage(), m_logger,
                                          m_progressCallback, m_cancelRequestedCallback);
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
    // NOTE: Not used in Sight

    // Read General Equipment Module - PS 3.3 C.7.5.1
    equipmentIE.readGeneralEquipmentModule();
    progress += 3;
    m_progressCallback(progress);

    // Read General Image Module - PS 3.3 C.7.6.1
    // NOTE: Not used in Sight

    // Read Image Plane Module - PS 3.3 C.7.6.2
    imageIE.readImagePlaneModule();
    progress += 3;
    m_progressCallback(progress);

    // Read Image Pixel Module - PS 3.3 C.7.6.3
    imageIE.readImagePixelModule();
    progress = 98;
    m_progressCallback(progress);

    // Read CT & MR Image Module - PS 3.3 C.8.2.1 - PS 3.3 C.8.3.1
    // NOTE: Not used in Sight

    // Read VOI LUT Module - PS 3.3 C.11.2
    imageIE.readVOILUTModule();
    progress = 100;
    m_progressCallback(progress);

    // Read SOP Common Module - PS 3.3 C.12.1
    // NOTE: Not used in Sight

}

//------------------------------------------------------------------------------

}  // namespace iod
}  // namespace reader
}  // namespace fwGdcmIO
