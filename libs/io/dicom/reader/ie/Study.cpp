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

#include "io/dicom/reader/ie/Study.hpp"

#include "io/dicom/helper/DicomDataReader.hxx"

#include <data/dicom_series.hpp>

namespace sight::io::dicom::reader::ie
{

//------------------------------------------------------------------------------

Study::Study(
    const data::dicom_series::csptr& _dicom_series,
    const SPTR(gdcm::Reader)& _reader,
    const io::dicom::container::DicomInstance::sptr& _instance,
    const data::series::sptr& _series,
    const core::log::logger::sptr& _logger,
    ProgressCallback _progress,
    CancelRequestedCallback _cancel
) :
    io::dicom::reader::ie::InformationEntity<data::series>(_dicom_series, _reader, _instance, _series, _logger,
                                                           _progress, _cancel)
{
}

//------------------------------------------------------------------------------

Study::~Study()
= default;

//------------------------------------------------------------------------------

void Study::readGeneralStudyModule()
{
    // Retrieve dataset
    const gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    const std::string& study_uid = io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x000d>(dataset);
    m_object->setStudyInstanceUID(study_uid);

    const std::string& study_id = io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x0010>(dataset);
    m_object->setStudyID(study_id);

    const std::string& study_date = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0020>(dataset);
    m_object->setStudyDate(study_date);

    const std::string& study_time = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0030>(dataset);
    m_object->setStudyTime(study_time);

    const std::string& study_referring_physician_name =
        io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0090>(dataset);
    m_object->setReferringPhysicianName(study_referring_physician_name);

    const std::string& study_description = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x1030>(dataset);
    m_object->setStudyDescription(study_description);
}

//------------------------------------------------------------------------------

void Study::readPatientStudyModule()
{
    // Retrieve dataset
    const gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    const std::string& study_patient_age = io::dicom::helper::DicomDataReader::getTagValue<0x0010, 0x1010>(dataset);
    m_object->setPatientAge(study_patient_age);

    const std::string& study_patient_size = io::dicom::helper::DicomDataReader::getTagValue<0x0010, 0x1020>(dataset);

    if(!study_patient_size.empty())
    {
        m_object->setPatientSize(std::stod(study_patient_size));
    }

    const std::string& study_patient_weight = io::dicom::helper::DicomDataReader::getTagValue<0x0010, 0x1030>(dataset);

    if(!study_patient_weight.empty())
    {
        m_object->setPatientWeight(std::stod(study_patient_weight));
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::ie
