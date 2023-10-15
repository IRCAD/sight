/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <data/image_series.hpp>
#include <data/point_list.hpp>
#include <data/string.hpp>

#include <memory>
#include <utility>

namespace sight::io::dicom::reader::iod
{

//------------------------------------------------------------------------------

ComprehensiveSRIOD::ComprehensiveSRIOD(
    const data::dicom_series::csptr& dicomSeries,
    const SPTR(io::dicom::container::DicomInstance)& instance,
    const core::log::logger::sptr& logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    io::dicom::reader::iod::InformationObjectDefinition(dicomSeries, instance, logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

ComprehensiveSRIOD::~ComprehensiveSRIOD()
= default;

//------------------------------------------------------------------------------

void ComprehensiveSRIOD::read(data::series::sptr series)
{
    // Retrieve image series
    data::image_series::sptr imageSeries = std::dynamic_pointer_cast<data::image_series>(series);
    SIGHT_ASSERT("Image series should not be null.", imageSeries);

    // Create GDCM reader
    SPTR(gdcm::Reader) reader = std::make_shared<gdcm::Reader>();

    // Read the first file
    const auto& dicomContainer                                 = m_dicomSeries->getDicomContainer();
    const core::memory::buffer_object::sptr bufferObj          = dicomContainer.begin()->second;
    const core::memory::buffer_manager::stream_info streamInfo = bufferObj->get_stream_info();
    SPTR(std::istream) is = streamInfo.stream;
    reader->SetStream(*is);
    const bool success = reader->Read();

    SIGHT_THROW_EXCEPTION_IF(
        io::dicom::exception::Failed(
            "Unable to read the DICOM instance \""
            + bufferObj->get_stream_info().fs_file.string()
            + "\" using the GDCM Image Reader."
        ),
        !success
    );

    // Create Information Entity helpers
    io::dicom::reader::ie::Document documentIE(m_dicomSeries, reader, m_instance, imageSeries, m_logger,
                                               m_progressCallback, m_cancelRequestedCallback);

    // Read SR
    documentIE.readSR();
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::iod
