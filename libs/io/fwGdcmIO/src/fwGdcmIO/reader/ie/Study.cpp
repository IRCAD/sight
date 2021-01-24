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

#include "fwGdcmIO/reader/ie/Study.hpp"

#include "fwGdcmIO/helper/DicomDataReader.hxx"

#include <data/DicomSeries.hpp>
#include <data/Study.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

Study::Study(const data::DicomSeries::csptr& dicomSeries,
             const SPTR(::gdcm::Reader)& reader,
             const ::fwGdcmIO::container::DicomInstance::sptr& instance,
             const data::Study::sptr& study,
             const ::fwLog::Logger::sptr& logger,
             ProgressCallback progress,
             CancelRequestedCallback cancel) :
    ::fwGdcmIO::reader::ie::InformationEntity< data::Study >(dicomSeries, reader, instance, study, logger,
                                                             progress, cancel)
{
}

//------------------------------------------------------------------------------

Study::~Study()
{
}

//------------------------------------------------------------------------------

void Study::readGeneralStudyModule()
{
    // Retrieve dataset
    const ::gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    const std::string& studyUID = ::fwGdcmIO::helper::DicomDataReader::getTagValue<0x0020, 0x000d>(dataset);
    m_object->setInstanceUID(studyUID);

    const std::string& studyID = ::fwGdcmIO::helper::DicomDataReader::getTagValue<0x0020, 0x0010>(dataset);
    m_object->setStudyID(studyID);

    const std::string& studyDate = ::fwGdcmIO::helper::DicomDataReader::getTagValue<0x0008, 0x0020>(dataset);
    m_object->setDate(studyDate);

    const std::string& studyTime = ::fwGdcmIO::helper::DicomDataReader::getTagValue<0x0008, 0x0030>(dataset);
    m_object->setTime(studyTime);

    const std::string& studyReferringPhysicianName
        = ::fwGdcmIO::helper::DicomDataReader::getTagValue<0x0008, 0x0090>(dataset);
    m_object->setReferringPhysicianName(studyReferringPhysicianName);

    const std::string& studyConsultingPhysicianName
        = ::fwGdcmIO::helper::DicomDataReader::getTagValue<0x0008, 0x009C>(dataset);
    m_object->setConsultingPhysicianName(studyConsultingPhysicianName);

    const std::string& studyDescription = ::fwGdcmIO::helper::DicomDataReader::getTagValue<0x0008, 0x1030>(dataset);
    m_object->setDescription(studyDescription);
}

//------------------------------------------------------------------------------

void Study::readPatientStudyModule()
{
    // Retrieve dataset
    const ::gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    const std::string& studyPatientAge = ::fwGdcmIO::helper::DicomDataReader::getTagValue<0x0010, 0x1010>(dataset);
    m_object->setPatientAge(studyPatientAge);

    const std::string& studyPatientSize = ::fwGdcmIO::helper::DicomDataReader::getTagValue<0x0010, 0x1020>(dataset);
    m_object->setPatientSize(studyPatientSize);

    const std::string& studyPatientWeight = ::fwGdcmIO::helper::DicomDataReader::getTagValue<0x0010, 0x1030>(dataset);
    m_object->setPatientWeight(studyPatientWeight);

    const std::string& studyPatientBodyMassIndex
        = ::fwGdcmIO::helper::DicomDataReader::getTagValue<0x0010, 0x1022>(dataset);
    m_object->setPatientBodyMassIndex(studyPatientBodyMassIndex);
}

//------------------------------------------------------------------------------

}// namespace ie
} // namespace reader
} // namespace fwGdcmIO
