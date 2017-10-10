/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/container/sr/DicomSRImageNode.hpp"
#include "fwGdcmIO/helper/DicomDataWriter.hxx"

#include <fwCore/spyLog.hpp>

#include <gdcmSequenceOfItems.h>

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

void DicomSRImageNode::write(::gdcm::DataSet &dataset) const
{
    ::fwGdcmIO::container::sr::DicomSRNode::write(dataset);

    // Referenced SOP Sequence
    this->writeReferencedSOPSequence(dataset);
}

//------------------------------------------------------------------------------

void DicomSRImageNode::writeReferencedSOPSequence(::gdcm::DataSet &dataset) const
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
