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

study::study(
    const data::dicom_series::csptr& _dicom_series,
    const SPTR(gdcm::Reader)& _reader,
    const io::dicom::container::dicom_instance::sptr& _instance,
    const data::series::sptr& _series,
    const core::log::logger::sptr& _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    io::dicom::reader::ie::information_entity<data::series>(_dicom_series, _reader, _instance, _series, _logger,
                                                            _progress, _cancel)
{
}

//------------------------------------------------------------------------------

study::~study()
= default;

//------------------------------------------------------------------------------

void study::read_general_study_module()
{
    // Retrieve dataset
    const gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    const std::string& study_uid = io::dicom::helper::dicom_data_reader::get_tag_value<0x0020, 0x000d>(dataset);
    m_object->set_study_instance_uid(study_uid);

    const std::string& study_id = io::dicom::helper::dicom_data_reader::get_tag_value<0x0020, 0x0010>(dataset);
    m_object->set_study_id(study_id);

    const std::string& study_date = io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x0020>(dataset);
    m_object->set_study_date(study_date);

    const std::string& study_time = io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x0030>(dataset);
    m_object->set_study_time(study_time);

    const std::string& study_referring_physician_name =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x0090>(dataset);
    m_object->set_referring_physician_name(study_referring_physician_name);

    const std::string& study_description = io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x1030>(dataset);
    m_object->set_study_description(study_description);
}

//------------------------------------------------------------------------------

void study::read_patient_study_module()
{
    // Retrieve dataset
    const gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    const std::string& study_patient_age = io::dicom::helper::dicom_data_reader::get_tag_value<0x0010, 0x1010>(dataset);
    m_object->set_patient_age(study_patient_age);

    const std::string& study_patient_size =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0010, 0x1020>(dataset);

    if(!study_patient_size.empty())
    {
        m_object->set_patient_size(std::stod(study_patient_size));
    }

    const std::string& study_patient_weight = io::dicom::helper::dicom_data_reader::get_tag_value<0x0010, 0x1030>(
        dataset
    );

    if(!study_patient_weight.empty())
    {
        m_object->set_patient_weight(std::stod(study_patient_weight));
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::ie
