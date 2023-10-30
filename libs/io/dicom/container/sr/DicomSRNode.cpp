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

#include "io/dicom/container/sr/DicomSRNode.hpp"

#include "io/dicom/helper/DicomDataWriter.hxx"

#include <utility>

namespace sight::io::dicom::container::sr
{

//------------------------------------------------------------------------------

dicom_sr_node::dicom_sr_node(
    dicom_coded_attribute _coded_attribute,
    std::string _type,
    std::string _relationship
) :
    m_coded_attribute(std::move(_coded_attribute)),
    m_type(std::move(_type)),
    m_relationship(std::move(_relationship))
{
}

//------------------------------------------------------------------------------

dicom_sr_node::~dicom_sr_node()
= default;

//------------------------------------------------------------------------------

void dicom_sr_node::add_sub_node(const SPTR(dicom_sr_node)& _node)
{
    m_sub_node_container.push_back(_node);
}

//------------------------------------------------------------------------------

void dicom_sr_node::write(gdcm::DataSet& _dataset) const
{
    // Value Type - Type 1
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0040, 0xa040>(m_type, _dataset);

    // Relationship Value - Type 1 (Shouldn't be there for root node)
    if(!m_relationship.empty())
    {
        io::dicom::helper::dicom_data_writer::set_tag_value<0x0040, 0xa010>(m_relationship, _dataset);
    }

    // Concept Name Code Sequence - Type 1C
    if(!m_coded_attribute.get_code_value().empty() && !m_coded_attribute.get_coding_scheme_designator().empty())
    {
        gdcm::SmartPointer<gdcm::SequenceOfItems> code_sequence =
            this->create_concept_name_code_sequence(m_coded_attribute);
        io::dicom::helper::dicom_data_writer::set_and_merge_sequence_tag_value<0x0040, 0xa043>(code_sequence, _dataset);
    }

    // Content sequence - Type 1C
    if(!m_sub_node_container.empty())
    {
        this->write_content_sequence(_dataset);
    }
}

//------------------------------------------------------------------------------

gdcm::SmartPointer<gdcm::SequenceOfItems> dicom_sr_node::create_concept_name_code_sequence(
    const dicom_coded_attribute& _coded_attribute
) const
{
    // Write code sequence
    gdcm::SmartPointer<gdcm::SequenceOfItems> code_sequence = new gdcm::SequenceOfItems();
    code_sequence->SetLengthToUndefined();

    // Create item (shall be one)
    gdcm::Item item;
    item.SetVLToUndefined();
    gdcm::DataSet& item_dataset = item.GetNestedDataSet();

    // Code value - Type 1
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0100>(
        _coded_attribute.get_code_value(),
        item_dataset
    );

    // Coding Scheme Designator - Type 1
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0102>(
        _coded_attribute.get_coding_scheme_designator(),
        item_dataset
    );

    // Coding Scheme Version - Type 1C
    if(!m_coded_attribute.get_coding_scheme_version().empty())
    {
        io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0103>(
            _coded_attribute.get_coding_scheme_version(),
            item_dataset
        );
    }

    // Code Meaning - Type 1
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0104>(
        _coded_attribute.get_code_meaning(),
        item_dataset
    );

    // Insert in a sequence
    code_sequence->AddItem(item);

    return code_sequence;
}

//------------------------------------------------------------------------------

void dicom_sr_node::write_content_sequence(gdcm::DataSet& _dataset) const
{
    // Create the content sequence
    gdcm::SmartPointer<gdcm::SequenceOfItems> sequence = new gdcm::SequenceOfItems();

    // Write every node
    for(const SPTR(io::dicom::container::sr::dicom_sr_node) & child : m_sub_node_container)
    {
        gdcm::Item item;
        item.SetVLToUndefined();
        gdcm::DataSet& item_dataset = item.GetNestedDataSet();
        child->write(item_dataset);
        sequence->AddItem(item);
    }

    io::dicom::helper::dicom_data_writer::set_sequence_tag_value<0x0040, 0xa730>(sequence, _dataset);
}

//------------------------------------------------------------------------------

void dicom_sr_node::print(std::ostream& _os) const
{
    _os << m_type;
    if(!m_coded_attribute.is_empty())
    {
        _os << "\\n[" << m_coded_attribute << "]";
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::container::sr
