/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <data/dicom_series.hpp>

namespace sight::io::dicom::reader::ie
{

//------------------------------------------------------------------------------

patient::patient(
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

patient::~patient()
= default;

//------------------------------------------------------------------------------

void patient::read_patient_module()
{
    // Retrieve dataset
    const gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    // Patient's name - Type 2
    const std::string& patient_name =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0010, 0x0010>(dataset);
    m_object->set_patient_name(patient_name);

    // Patient's ID - Type 2
    const std::string& patient_id =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0010, 0x0020>(dataset);
    m_object->set_patient_id(patient_id);

    // Patient's birth date - Type 2
    const std::string& patient_birth_date =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0010, 0x0030>(dataset);
    m_object->set_patient_birth_date(patient_birth_date);

    // Patient's sex - Type 2
    const std::string& patient_sex =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0010, 0x0040>(dataset);
    m_object->set_patient_sex(patient_sex);
}

} // namespace sight::io::dicom::reader::ie
