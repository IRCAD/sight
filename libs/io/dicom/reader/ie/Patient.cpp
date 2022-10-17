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

#include "io/dicom/reader/ie/Patient.hpp"

#include "io/dicom/helper/DicomDataReader.hxx"

#include <data/DicomSeries.hpp>

namespace sight::io::dicom::reader::ie
{

//------------------------------------------------------------------------------

Patient::Patient(
    const data::DicomSeries::csptr& dicomSeries,
    const SPTR(gdcm::Reader)& reader,
    const io::dicom::container::DicomInstance::sptr& instance,
    const data::Series::sptr& series,
    const core::log::Logger::sptr& logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    io::dicom::reader::ie::InformationEntity<data::Series>(dicomSeries, reader, instance, series, logger,
                                                           progress, cancel)
{
}

//------------------------------------------------------------------------------

Patient::~Patient()
= default;

//------------------------------------------------------------------------------

void Patient::readPatientModule()
{
    // Retrieve dataset
    const gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    // Patient's name - Type 2
    const std::string& patientName =
        io::dicom::helper::DicomDataReader::getTagValue<0x0010, 0x0010>(dataset);
    m_object->setPatientName(patientName);

    // Patient's ID - Type 2
    const std::string& patientId =
        io::dicom::helper::DicomDataReader::getTagValue<0x0010, 0x0020>(dataset);
    m_object->setPatientID(patientId);

    // Patient's birth date - Type 2
    const std::string& patientBirthDate =
        io::dicom::helper::DicomDataReader::getTagValue<0x0010, 0x0030>(dataset);
    m_object->setPatientBirthDate(patientBirthDate);

    // Patient's sex - Type 2
    const std::string& patientSex =
        io::dicom::helper::DicomDataReader::getTagValue<0x0010, 0x0040>(dataset);
    m_object->setPatientSex(patientSex);
}

} // namespace sight::io::dicom::reader::ie
