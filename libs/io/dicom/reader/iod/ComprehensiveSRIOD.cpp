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

#include "io/dicom/reader/iod/ComprehensiveSRIOD.hpp"

#include "io/dicom/reader/ie/Document.hpp"

#include <data/ImageSeries.hpp>
#include <data/PointList.hpp>
#include <data/String.hpp>

namespace sight::io::dicom
{

namespace reader
{

namespace iod
{

//------------------------------------------------------------------------------

ComprehensiveSRIOD::ComprehensiveSRIOD(
    const data::DicomSeries::csptr& dicomSeries,
    const SPTR(io::dicom::container::DicomInstance)& instance,
    const core::log::Logger::sptr& logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    io::dicom::reader::iod::InformationObjectDefinition(dicomSeries, instance, logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

ComprehensiveSRIOD::~ComprehensiveSRIOD()
{
}

//------------------------------------------------------------------------------

void ComprehensiveSRIOD::read(data::Series::sptr series)
{
    // Retrieve image series
    data::ImageSeries::sptr imageSeries = data::ImageSeries::dynamicCast(series);
    SIGHT_ASSERT("Image series should not be null.", imageSeries);

    // Create GDCM reader
    SPTR(::gdcm::Reader) reader = std::shared_ptr< ::gdcm::Reader>(new ::gdcm::Reader);

    // Read the first file
    const auto& dicomContainer                               = m_dicomSeries->getDicomContainer();
    const core::memory::BufferObject::sptr bufferObj         = dicomContainer.begin()->second;
    const core::memory::BufferManager::StreamInfo streamInfo = bufferObj->getStreamInfo();
    SPTR(std::istream) is = streamInfo.stream;
    reader->SetStream(*is);
    const bool success = reader->Read();

    SIGHT_THROW_EXCEPTION_IF(
        io::dicom::exception::Failed(
            "Unable to read the DICOM instance \""
            + bufferObj->getStreamInfo().fsFile.string()
            + "\" using the GDCM Image Reader."
        ),
        !success
    );

    // Create Information Entity helpers
    io::dicom::reader::ie::Document documentIE(m_dicomSeries, reader, m_instance, imageSeries->getImage(), m_logger,
                                               m_progressCallback, m_cancelRequestedCallback);

    // Read SR
    documentIE.readSR();
}

//------------------------------------------------------------------------------

} // namespace iod

} // namespace reader

} // namespace sight::io::dicom
