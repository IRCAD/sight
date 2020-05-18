/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
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

#include "fwGdcmIO/reader/ie/Patient.hpp"

#include "fwGdcmIO/helper/DicomDataReader.hxx"

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/Patient.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

Patient::Patient(const ::fwMedData::DicomSeries::csptr& dicomSeries,
                 const SPTR(::gdcm::Reader)& reader,
                 const ::fwGdcmIO::container::DicomInstance::sptr& instance,
                 const ::fwMedData::Patient::sptr& patient,
                 const ::fwLog::Logger::sptr& logger,
                 ProgressCallback progress,
                 CancelRequestedCallback cancel) :
    ::fwGdcmIO::reader::ie::InformationEntity< ::fwMedData::Patient >(dicomSeries, reader, instance, patient, logger,
                                                                      progress, cancel)
{
}

//------------------------------------------------------------------------------

Patient::~Patient()
{
}

//------------------------------------------------------------------------------

void Patient::readPatientModule()
{
    // Retrieve dataset
    const ::gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    // Patient's name - Type 2
    const std::string& patientName =
        ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0010, 0x0010 >(dataset);
    m_object->setName(patientName);

    // Patient's ID - Type 2
    const std::string& patientId =
        ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0010, 0x0020 >(dataset);
    m_object->setPatientId(patientId);

    // Patient's birth date - Type 2
    const std::string& birthDate =
        ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0010, 0x0030 >(dataset);
    m_object->setBirthdate(birthDate);

    // Patient's sex - Type 2
    const std::string& sex =
        ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0010, 0x0040 >(dataset);
    m_object->setSex(sex);
}

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO
