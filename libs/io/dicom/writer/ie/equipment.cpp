/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "io/dicom/writer/ie/equipment.hpp"

#include "io/dicom/helper/dicom_data_writer.hxx"

#include <core/runtime/profile/profile.hpp>

namespace sight::io::dicom::writer::ie
{

//------------------------------------------------------------------------------

equipment::equipment(
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

equipment::~equipment()
= default;

//------------------------------------------------------------------------------

void equipment::write_general_equipment_module()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Manufacturer - Type 2 (Type 1 for EnhancedGeneralEquipmentModule)
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0070>("Sight", dataset);

    // Institution Name - Type 3
    const std::string& institut_name = m_object->get_institution_name();
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0080>(institut_name, dataset);

    // Software Versions - Type 3 (Type 1 for EnhancedGeneralEquipmentModule)
    const auto profile                 = core::runtime::get_current_profile();
    const std::string software_version = profile ? profile->name() + " " + profile->version() : "Unknown";

    // We do not use the DicomDataWriter helper as VM might be more than one
    gdcm::Attribute<0x0018, 0x1020> attribute;
    attribute.SetNumberOfValues(1);
    attribute.SetValue(software_version);
    dataset.Insert(attribute.GetAsDataElement());
}

//------------------------------------------------------------------------------

void equipment::write_enhanced_general_equipment_module()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Manufacturer's Model Name - Type 1
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x1090>("Sight_FWGDCMIO", dataset);

    // Device Serial Number - Type 1
    std::string device_serial_number = std::string("0.1");
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0018, 0x1000>(device_serial_number, dataset);
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::ie
