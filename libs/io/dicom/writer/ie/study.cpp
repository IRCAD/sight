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

#include "io/dicom/writer/ie/study.hpp"

#include "io/dicom/helper/dicom_data_writer.hxx"

#include <array>
#include <utility>

namespace sight::io::dicom::writer::ie
{

//------------------------------------------------------------------------------

study::study(
    const SPTR(gdcm::Writer)& _writer,
    const SPTR(io::dicom::container::dicom_instance)& _instance,
    const data::series::csptr& _series,
    const core::log::logger::sptr& _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    io::dicom::writer::ie::information_entity<data::series>(_writer, _instance, _series,
                                                            _logger, _progress, _cancel)
{
}

//------------------------------------------------------------------------------

study::~study()
= default;

//------------------------------------------------------------------------------

void study::write_general_study_module()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    io::dicom::helper::dicom_data_writer::set_tag_value<0x0020, 0x000d>(m_object->get_study_instance_uid(), dataset);
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0020, 0x0010>(m_object->get_study_id(), dataset);
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0020>(m_object->get_study_date(), dataset);
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0030>(m_object->get_study_time(), dataset);
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0090>(
        m_object->get_referring_physician_name(),
        dataset
    );
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x1030>(m_object->get_study_description(), dataset);
    // Study 's accession number - Type 2
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0050>("", dataset);
}

//------------------------------------------------------------------------------

void study::write_patient_study_module()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    io::dicom::helper::dicom_data_writer::set_tag_value<0x0010, 0x1010>(m_object->get_patient_age(), dataset);

    if(const auto& patient_size = m_object->get_patient_size(); patient_size)
    {
        io::dicom::helper::dicom_data_writer::set_tag_value<double, 0x0010, 0x1020>(*patient_size, dataset);
    }

    if(const auto& patient_weight = m_object->get_patient_weight(); patient_weight)
    {
        io::dicom::helper::dicom_data_writer::set_tag_value<double, 0x0010, 0x1030>(*patient_weight, dataset);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::ie
