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

#include "io/dicom/writer/ie/Equipment.hpp"

#include "io/dicom/helper/DicomDataWriter.hxx"

#include <core/runtime/profile/profile.hpp>

namespace sight::io::dicom::writer::ie
{

//------------------------------------------------------------------------------

Equipment::Equipment(
    const SPTR(gdcm::Writer)& _writer,
    const SPTR(io::dicom::container::DicomInstance)& _instance,
    const data::series::csptr& _series,
    const core::log::logger::sptr& _logger,
    ProgressCallback _progress,
    CancelRequestedCallback _cancel
) :
    io::dicom::writer::ie::InformationEntity<data::series>(_writer, _instance, _series,
                                                           _logger, _progress, _cancel)
{
}

//------------------------------------------------------------------------------

Equipment::~Equipment()
= default;

//------------------------------------------------------------------------------

void Equipment::writeGeneralEquipmentModule()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Manufacturer - Type 2 (Type 1 for EnhancedGeneralEquipmentModule)
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0070>("Sight", dataset);

    // Institution Name - Type 3
    const std::string& institut_name = m_object->getInstitutionName();
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0080>(institut_name, dataset);

    // Software Versions - Type 3 (Type 1 for EnhancedGeneralEquipmentModule)
    const auto profile                 = core::runtime::get_current_profile();
    const std::string software_version = profile ? profile->name() + " " + profile->get_version() : "Unknown";

    // We do not use the DicomDataWriter helper as VM might be more than one
    gdcm::Attribute<0x0018, 0x1020> attribute;
    attribute.SetNumberOfValues(1);
    attribute.SetValue(software_version);
    dataset.Insert(attribute.GetAsDataElement());
}

//------------------------------------------------------------------------------

void Equipment::writeEnhancedGeneralEquipmentModule()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Manufacturer's Model Name - Type 1
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x1090>("Sight_FWGDCMIO", dataset);

    // Device Serial Number - Type 1
    std::string device_serial_number = std::string("0.1");
    io::dicom::helper::DicomDataWriter::setTagValue<0x0018, 0x1000>(device_serial_number, dataset);
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::ie
