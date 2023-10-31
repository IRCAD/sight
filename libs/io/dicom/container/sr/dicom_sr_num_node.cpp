/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "io/dicom/container/sr/dicom_sr_num_node.hpp"

#include "io/dicom/helper/dicom_data_writer.hxx"

#include <utility>

namespace sight::io::dicom::container::sr
{

//------------------------------------------------------------------------------

dicom_sr_num_node::dicom_sr_num_node(
    const dicom_coded_attribute& _coded_attribute,
    const std::string& _relationship,
    const double _num_value,
    dicom_coded_attribute _measurement_units
) :
    io::dicom::container::sr::dicom_sr_node(_coded_attribute, "NUM", _relationship),
    m_num_value(_num_value),
    m_measurement_units(std::move(_measurement_units))
{
}

//------------------------------------------------------------------------------

dicom_sr_num_node::~dicom_sr_num_node()
= default;

//------------------------------------------------------------------------------

void dicom_sr_num_node::write(gdcm::DataSet& _dataset) const
{
    io::dicom::container::sr::dicom_sr_node::write(_dataset);

    // Measured Value Sequence - Type 2
    this->write_measured_value_sequence(_dataset);
}

//------------------------------------------------------------------------------

void dicom_sr_num_node::write_measured_value_sequence(gdcm::DataSet& _dataset) const
{
    gdcm::SmartPointer<gdcm::SequenceOfItems> sequence = new gdcm::SequenceOfItems();
    gdcm::Item item;
    item.SetVLToUndefined();
    gdcm::DataSet& item_dataset = item.GetNestedDataSet();

    // Add numerical value - Type 1
    io::dicom::helper::dicom_data_writer::set_tag_values<double, 0x0040, 0xa30a>(&m_num_value, 1, item_dataset);

    // Add measured units code sequence - Type 1
    gdcm::SmartPointer<gdcm::SequenceOfItems> code_sequence =
        this->create_concept_name_code_sequence(m_measurement_units);
    io::dicom::helper::dicom_data_writer::set_and_merge_sequence_tag_value<0x0040, 0x08ea>(code_sequence, item_dataset);

    sequence->AddItem(item);
    io::dicom::helper::dicom_data_writer::set_sequence_tag_value<0x0040, 0xa300>(sequence, _dataset);
}

//------------------------------------------------------------------------------

void dicom_sr_num_node::print(std::ostream& _os) const
{
    dicom_sr_node::print(_os);
    _os << "\\nMeasurement units : [" << m_measurement_units << "]";
    _os << "\\nMeasurement value : [" << m_num_value << "]";
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::container::sr
