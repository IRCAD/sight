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

#include "fwGdcmIO/writer/ie/Study.hpp"

#include "fwGdcmIO/helper/DicomDataWriter.hxx"

#include <fwMedData/Study.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

//------------------------------------------------------------------------------

Study::Study(const SPTR(::gdcm::Writer)& writer,
             const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
             const ::fwMedData::Study::csptr& study,
             const ::fwLog::Logger::sptr& logger,
             ProgressCallback progress,
             CancelRequestedCallback cancel) :
    ::fwGdcmIO::writer::ie::InformationEntity< ::fwMedData::Study >(writer, instance, study,
                                                                    logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

Study::~Study()
{
}

//------------------------------------------------------------------------------

void Study::writeGeneralStudyModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Study's date - Type 2
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x0020 >(m_object->getDate(), dataset);

    // Study's time - Type 2
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x0030 >(m_object->getTime(), dataset);

    // Study 's accession number - Type 2
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x0050 >("", dataset);

    // Study's description - Type 3
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x1030 >(m_object->getDescription(), dataset);

    // Study's UID - Type 1
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0020, 0x000d >(m_object->getInstanceUID(), dataset);

    // Study's ID - Type 2
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0020, 0x0010 >("Unknown", dataset);

    // Study's referring physician name
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x0090 >(m_object->getReferringPhysicianName(), dataset);
}

//------------------------------------------------------------------------------

void Study::writePatientStudyModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // PatientAge
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0010, 0x1010 >(m_object->getPatientAge(), dataset);
}

//------------------------------------------------------------------------------

}// namespace ie
} // namespace writer
} // namespace fwGdcmIO
