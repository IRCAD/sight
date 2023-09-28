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

#include "io/dicom/writer/ie/Study.hpp"

#include "io/dicom/helper/DicomDataWriter.hxx"

#include <array>
#include <utility>

namespace sight::io::dicom::writer::ie
{

//------------------------------------------------------------------------------

Study::Study(
    const SPTR(gdcm::Writer)& writer,
    const SPTR(io::dicom::container::DicomInstance)& instance,
    const data::Series::csptr& series,
    const core::log::logger::sptr& logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    io::dicom::writer::ie::InformationEntity<data::Series>(writer, instance, series,
                                                           logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

Study::~Study()
= default;

//------------------------------------------------------------------------------

void Study::writeGeneralStudyModule()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    io::dicom::helper::DicomDataWriter::setTagValue<0x0020, 0x000d>(m_object->getStudyInstanceUID(), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0020, 0x0010>(m_object->getStudyID(), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0020>(m_object->getStudyDate(), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0030>(m_object->getStudyTime(), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0090>(m_object->getReferringPhysicianName(), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x1030>(m_object->getStudyDescription(), dataset);
    // Study 's accession number - Type 2
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0050>("", dataset);
}

//------------------------------------------------------------------------------

void Study::writePatientStudyModule()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    io::dicom::helper::DicomDataWriter::setTagValue<0x0010, 0x1010>(m_object->getPatientAge(), dataset);

    if(const auto& patientSize = m_object->getPatientSize(); patientSize)
    {
        io::dicom::helper::DicomDataWriter::setTagValue<double, 0x0010, 0x1020>(*patientSize, dataset);
    }

    if(const auto& patientWeight = m_object->getPatientWeight(); patientWeight)
    {
        io::dicom::helper::DicomDataWriter::setTagValue<double, 0x0010, 0x1030>(*patientWeight, dataset);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::ie
