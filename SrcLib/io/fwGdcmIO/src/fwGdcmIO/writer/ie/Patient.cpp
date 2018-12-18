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

#include "fwGdcmIO/writer/ie/Patient.hpp"

#include "fwGdcmIO/helper/DicomDataWriter.hxx"

#include <fwMedData/Patient.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

//------------------------------------------------------------------------------

Patient::Patient(const SPTR(::gdcm::Writer)& writer,
                 const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                 const ::fwMedData::Patient::csptr& patient,
                 const ::fwLog::Logger::sptr& logger,
                 ProgressCallback progress,
                 CancelRequestedCallback cancel) :
    ::fwGdcmIO::writer::ie::InformationEntity< ::fwMedData::Patient >(writer, instance, patient,
                                                                      logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

Patient::~Patient()
{
}

//------------------------------------------------------------------------------

void Patient::writePatientModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Patient's name - Type 2
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0010, 0x0010 >(m_object->getName(), dataset);

    // Patient's ID - Type 2
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0010, 0x0020 >(m_object->getPatientId(), dataset);

    // Patient's birth date - Type 2
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0010, 0x0030 >(m_object->getBirthdate(), dataset);

    // Patient's sex - Type 2
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0010, 0x0040 >(m_object->getSex(), dataset);
}

//------------------------------------------------------------------------------

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO
