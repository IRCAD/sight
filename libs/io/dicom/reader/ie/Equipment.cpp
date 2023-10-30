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

#include "io/dicom/reader/ie/Equipment.hpp"

#include "io/dicom/helper/DicomDataReader.hxx"

#include <data/dicom_series.hpp>

namespace sight::io::dicom::reader::ie
{

//------------------------------------------------------------------------------

equipment::equipment(
    const data::dicom_series::csptr& _dicom_series,
    const SPTR(gdcm::Reader)& _reader,
    const io::dicom::container::dicom_instance::sptr& _instance,
    const data::series::sptr& _series,
    const core::log::logger::sptr& _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    io::dicom::reader::ie::information_entity<data::series>(_dicom_series, _reader, _instance, _series,
                                                            _logger, _progress, _cancel)
{
}

//------------------------------------------------------------------------------

equipment::~equipment()
= default;

//------------------------------------------------------------------------------

void equipment::read_general_equipment_module()
{
    // Retrieve dataset
    const gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    // Institution Name - Type 3
    const std::string& institut_name =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x0080>(dataset);
    m_object->set_institution_name(institut_name);

    // Manufacturer - Type 2
    // NOTE: Not used in Sight

    // Manufacturer's Model Name - Type 3 (Type 1 for EnhancedGeneralEquipmentModule)
    // NOTE: Not used in Sight

    // Device Serial Number - Type 3 (Type 1 for EnhancedGeneralEquipmentModule)
    // NOTE: Not used in Sight

    // Software Versions - Type 3 (Type 1 for EnhancedGeneralEquipmentModule)
    // NOTE: Not used in Sight
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::ie
