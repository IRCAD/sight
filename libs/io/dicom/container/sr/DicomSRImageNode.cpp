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

#include "io/dicom/container/sr/DicomSRImageNode.hpp"

#include "io/dicom/helper/DicomDataWriter.hxx"

#include <utility>

namespace sight::io::dicom::container::sr
{

//------------------------------------------------------------------------------

DicomSRImageNode::DicomSRImageNode(
    const DicomCodedAttribute& _coded_attribute,
    const std::string& _relationship,
    std::string _sop_class_uid,
    std::string _sop_instance_uid,
    int _frame_number
) :
    io::dicom::container::sr::DicomSRNode(_coded_attribute, "IMAGE", _relationship),
    m_sopClassUID(std::move(_sop_class_uid)),
    m_sopInstanceUID(std::move(_sop_instance_uid)),
    m_frameNumber(_frame_number)
{
}

//------------------------------------------------------------------------------

DicomSRImageNode::~DicomSRImageNode()
= default;

//------------------------------------------------------------------------------

void DicomSRImageNode::write(gdcm::DataSet& _dataset) const
{
    io::dicom::container::sr::DicomSRNode::write(_dataset);

    // Referenced SOP Sequence
    this->writeReferencedSOPSequence(_dataset);
}

//------------------------------------------------------------------------------

void DicomSRImageNode::writeReferencedSOPSequence(gdcm::DataSet& _dataset) const
{
    gdcm::SmartPointer<gdcm::SequenceOfItems> sequence = new gdcm::SequenceOfItems();
    gdcm::Item item;
    item.SetVLToUndefined();
    gdcm::DataSet& item_dataset = item.GetNestedDataSet();

    // Referenced SOP Class UID - Type 1
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x1150>(m_sopClassUID, item_dataset);

    // Referenced SOP Instance UID  - Type 1
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x1155>(m_sopInstanceUID, item_dataset);

    // Referenced Frame Number - Type 1C
    io::dicom::helper::DicomDataWriter::setTagValues<int, 0x0008, 0x1160>(&m_frameNumber, 1, item_dataset);

    sequence->AddItem(item);
    io::dicom::helper::DicomDataWriter::setSequenceTagValue<0x0008, 0x1199>(sequence, _dataset);
}

//------------------------------------------------------------------------------

void DicomSRImageNode::print(std::ostream& _os) const
{
    DicomSRNode::print(_os);
    _os << "\\nSOP Class UID : [" << m_sopClassUID << "]";
    _os << "\\nSOP Instance UID : [" << m_sopInstanceUID << "]";
    _os << "\\nFrame number : [" << m_frameNumber << "]";
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::container::sr
