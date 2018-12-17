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

#include "fwGdcmIO/container/sr/DicomSRImageNode.hpp"

#include "fwGdcmIO/helper/DicomDataWriter.hxx"

namespace fwGdcmIO
{
namespace container
{
namespace sr
{

//------------------------------------------------------------------------------

DicomSRImageNode::DicomSRImageNode(const DicomCodedAttribute& codedAttribute,
                                   const std::string& relationship,
                                   const std::string& sopClassUID,
                                   const std::string& sopInstanceUID,
                                   int frameNumber) :
    ::fwGdcmIO::container::sr::DicomSRNode(codedAttribute, "IMAGE", relationship),
    m_sopClassUID(sopClassUID),
    m_sopInstanceUID(sopInstanceUID),
    m_frameNumber(frameNumber)
{
}

//------------------------------------------------------------------------------

DicomSRImageNode::~DicomSRImageNode()
{
}

//------------------------------------------------------------------------------

void DicomSRImageNode::write(::gdcm::DataSet& dataset) const
{
    ::fwGdcmIO::container::sr::DicomSRNode::write(dataset);

    // Referenced SOP Sequence
    this->writeReferencedSOPSequence(dataset);
}

//------------------------------------------------------------------------------

void DicomSRImageNode::writeReferencedSOPSequence(::gdcm::DataSet& dataset) const
{
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence = new ::gdcm::SequenceOfItems();
    ::gdcm::Item item;
    item.SetVLToUndefined();
    ::gdcm::DataSet& itemDataset = item.GetNestedDataSet();

    // Referenced SOP Class UID - Type 1
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x1150 >(m_sopClassUID, itemDataset);

    // Referenced SOP Instance UID  - Type 1
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x1155 >(m_sopInstanceUID, itemDataset);

    // Referenced Frame Number - Type 1C
    ::fwGdcmIO::helper::DicomDataWriter::setTagValues< int, 0x0008, 0x1160 >(&m_frameNumber, 1, itemDataset);

    sequence->AddItem(item);
    ::fwGdcmIO::helper::DicomDataWriter::setSequenceTagValue< 0x0008, 0x1199 >(sequence, dataset);
}

//------------------------------------------------------------------------------

void DicomSRImageNode::print(std::ostream& os) const
{
    DicomSRNode::print(os);
    os << "\\nSOP Class UID : [" << m_sopClassUID << "]";
    os << "\\nSOP Instance UID : [" << m_sopInstanceUID << "]";
    os << "\\nFrame number : [" << m_frameNumber << "]";
}

//------------------------------------------------------------------------------

} //namespace sr
} //namespace container
} //namespace fwGdcmIO
