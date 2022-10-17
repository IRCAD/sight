/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "io/dicom/writer/ie/FrameOfReference.hpp"

#include "io/dicom/helper/DicomDataWriter.hxx"

#include <data/Series.hpp>

namespace sight::io::dicom::writer::ie
{

//------------------------------------------------------------------------------

FrameOfReference::FrameOfReference(
    const SPTR(gdcm::Writer)& writer,
    const SPTR(io::dicom::container::DicomInstance)& instance,
    const data::Series::csptr& series,
    const core::log::Logger::sptr& logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    io::dicom::writer::ie::InformationEntity<data::Series>(writer, instance, series,
                                                           logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

FrameOfReference::~FrameOfReference()
= default;

//------------------------------------------------------------------------------

void FrameOfReference::writeFrameOfReferenceModule()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Frame of Reference UID
    const std::string frameOfReferenceUID = m_instance->getFrameOfReferenceUID();
    io::dicom::helper::DicomDataWriter::setTagValue<0x0020, 0x0052>(frameOfReferenceUID, dataset);

    // Position Reference Indicator - Type 2
    io::dicom::helper::DicomDataWriter::setEmptyTagValue<0x0020, 0x1040>(dataset);
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::ie
