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

#include "fwGdcmIO/reader/iod/ComprehensiveSRIOD.hpp"

#include "fwGdcmIO/reader/ie/Document.hpp"

#include <fwData/PointList.hpp>
#include <fwData/String.hpp>

#include <fwMedData/ImageSeries.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace iod
{

//------------------------------------------------------------------------------

ComprehensiveSRIOD::ComprehensiveSRIOD(const ::fwMedData::DicomSeries::csptr& dicomSeries,
                                       const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                       const ::fwLog::Logger::sptr& logger,
                                       ProgressCallback progress,
                                       CancelRequestedCallback cancel) :
    ::fwGdcmIO::reader::iod::InformationObjectDefinition(dicomSeries, instance, logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

ComprehensiveSRIOD::~ComprehensiveSRIOD()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void ComprehensiveSRIOD::read(::fwMedData::Series::sptr series)
{
    // Retrieve image series
    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    SLM_ASSERT("Image series should not be null.", imageSeries);

    // Create GDCM reader
    SPTR(::gdcm::Reader) reader = std::shared_ptr< ::gdcm::Reader >( new ::gdcm::Reader );

    // Read the first file
    const auto& dicomContainer                             = m_dicomSeries->getDicomContainer();
    const ::fwMemory::BufferObject::sptr bufferObj         = dicomContainer.begin()->second;
    const ::fwMemory::BufferManager::StreamInfo streamInfo = bufferObj->getStreamInfo();
    SPTR(std::istream) is = streamInfo.stream;
    reader->SetStream(*is);
    const bool success = reader->Read();

    FW_RAISE_EXCEPTION_IF(::fwGdcmIO::exception::Failed("Unable to read the DICOM instance \""+
                                                        bufferObj->getStreamInfo().fsFile.string()+
                                                        "\" using the GDCM Image Reader."), !success);

    // Create Information Entity helpers
    ::fwGdcmIO::reader::ie::Document documentIE(m_dicomSeries, reader, m_instance, imageSeries->getImage(), m_logger,
                                                m_progressCallback, m_cancelRequestedCallback);

    // Read SR
    documentIE.readSR();
}

//------------------------------------------------------------------------------

}  // namespace iod
}  // namespace reader
}  // namespace fwGdcmIO
