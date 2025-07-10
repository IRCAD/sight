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

#include "io/dicom/container/sr/dicom_sr_code_node.hpp"

#include "io/dicom/helper/dicom_data_writer.hxx"

#include <utility>

namespace sight::io::dicom::container::sr
{

//------------------------------------------------------------------------------

dicom_sr_code_node::dicom_sr_code_node(
    const dicom_coded_attribute& _coded_attribute,
    const std::string& _relationship
    ,
    dicom_coded_attribute _coded_entry
) :
    io::dicom::container::sr::dicom_sr_node(_coded_attribute, "CODE", _relationship),
    m_coded_entry(std::move(_coded_entry))
{
}

//------------------------------------------------------------------------------

dicom_sr_code_node::~dicom_sr_code_node()
= default;

//------------------------------------------------------------------------------

void dicom_sr_code_node::write(gdcm::DataSet& _dataset) const
{
    io::dicom::container::sr::dicom_sr_node::write(_dataset);

    // Concept Code Sequence - Type 1
    gdcm::SmartPointer<gdcm::SequenceOfItems> code_sequence =
        this->create_concept_name_code_sequence(m_coded_entry);
    io::dicom::helper::dicom_data_writer::set_and_merge_sequence_tag_value<0x0040, 0xa168>(code_sequence, _dataset);
}

//------------------------------------------------------------------------------

void dicom_sr_code_node::print(std::ostream& _os) const
{
    dicom_sr_node::print(_os);
    _os << "\\nCoded entry : [" << m_coded_entry << "]";
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::container::sr
