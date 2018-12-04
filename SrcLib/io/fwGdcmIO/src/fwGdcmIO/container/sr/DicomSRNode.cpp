/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
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

#include "fwGdcmIO/container/sr/DicomSRNode.hpp"

#include "fwGdcmIO/helper/DicomDataWriter.hxx"

namespace fwGdcmIO
{
namespace container
{
namespace sr
{

//------------------------------------------------------------------------------

DicomSRNode::DicomSRNode(const DicomCodedAttribute& codedAttribute,
                         const std::string& type,
                         const std::string& relationship) :
    m_codedAttribute(codedAttribute),
    m_type(type),
    m_relationship(relationship)
{
}

//------------------------------------------------------------------------------

DicomSRNode::~DicomSRNode()
{
}

//------------------------------------------------------------------------------

void DicomSRNode::addSubNode(const SPTR(DicomSRNode)& node)
{
    m_subNodeContainer.push_back(node);
}

//------------------------------------------------------------------------------

void DicomSRNode::write(::gdcm::DataSet& dataset) const
{
    // Value Type - Type 1
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0040, 0xa040 >(m_type, dataset);

    // Relationship Value - Type 1 (Shouldn't be there for root node)
    if(!m_relationship.empty())
    {
        ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0040, 0xa010 >(m_relationship, dataset);
    }

    // Concept Name Code Sequence - Type 1C
    if(!m_codedAttribute.getCodeValue().empty() && !m_codedAttribute.getCodingSchemeDesignator().empty())
    {
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > codeSequence =
            this->createConceptNameCodeSequence(m_codedAttribute);
        ::fwGdcmIO::helper::DicomDataWriter::setAndMergeSequenceTagValue<0x0040, 0xa043>(codeSequence, dataset);

    }

    // Content sequence - Type 1C
    if(!m_subNodeContainer.empty())
    {
        this->writeContentSequence(dataset);
    }
}

//------------------------------------------------------------------------------

::gdcm::SmartPointer< ::gdcm::SequenceOfItems > DicomSRNode::createConceptNameCodeSequence(
    const DicomCodedAttribute& codedAttribute) const
{
    // Write code sequence
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > codeSequence = new ::gdcm::SequenceOfItems();
    codeSequence->SetLengthToUndefined();

    // Create item (shall be one)
    ::gdcm::Item item;
    item.SetVLToUndefined();
    ::gdcm::DataSet& itemDataset = item.GetNestedDataSet();

    // Code value - Type 1
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue<0x0008, 0x0100>(codedAttribute.getCodeValue(), itemDataset);

    // Coding Scheme Designator - Type 1
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue<0x0008, 0x0102>(
        codedAttribute.getCodingSchemeDesignator(), itemDataset);

    // Coding Scheme Version - Type 1C
    if (!m_codedAttribute.getCodingSchemeVersion().empty())
    {
        ::fwGdcmIO::helper::DicomDataWriter::setTagValue<0x0008, 0x0103>(
            codedAttribute.getCodingSchemeVersion(), itemDataset);
    }

    // Code Meaning - Type 1
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue<0x0008, 0x0104>(codedAttribute.getCodeMeaning(), itemDataset);

    // Insert in a sequence
    codeSequence->AddItem(item);

    return codeSequence;
}

//------------------------------------------------------------------------------

void DicomSRNode::writeContentSequence(::gdcm::DataSet& dataset) const
{
    // Create the content sequence
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence = new ::gdcm::SequenceOfItems();

    // Write every node
    for(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& child : m_subNodeContainer)
    {
        ::gdcm::Item item;
        item.SetVLToUndefined();
        ::gdcm::DataSet& itemDataset = item.GetNestedDataSet();
        child->write(itemDataset);
        sequence->AddItem(item);
    }

    ::fwGdcmIO::helper::DicomDataWriter::setSequenceTagValue< 0x0040, 0xa730 >(sequence, dataset);
}

//------------------------------------------------------------------------------

void DicomSRNode::print(std::ostream& os) const
{
    os << m_type;
    if(!m_codedAttribute.isEmpty())
    {
        os << "\\n[" << m_codedAttribute << "]";
    }
}

//------------------------------------------------------------------------------

} //namespace sr
} //namespace container
} //namespace fwGdcmIO
