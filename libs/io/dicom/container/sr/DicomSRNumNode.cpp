/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/dicom/container/sr/DicomSRNumNode.hpp"

#include "io/dicom/helper/DicomDataWriter.hxx"

namespace sight::io::dicom
{

namespace container
{

namespace sr
{

//------------------------------------------------------------------------------

DicomSRNumNode::DicomSRNumNode(
    const DicomCodedAttribute& codedAttribute,
    const std::string& relationship,
    const double numValue,
    const DicomCodedAttribute& measurementUnits
) :
    io::dicom::container::sr::DicomSRNode(codedAttribute, "NUM", relationship),
    m_numValue(numValue),
    m_measurementUnits(measurementUnits)
{
}

//------------------------------------------------------------------------------

DicomSRNumNode::~DicomSRNumNode()
{
}

//------------------------------------------------------------------------------

void DicomSRNumNode::write(::gdcm::DataSet& dataset) const
{
    io::dicom::container::sr::DicomSRNode::write(dataset);

    // Measured Value Sequence - Type 2
    this->writeMeasuredValueSequence(dataset);
}

//------------------------------------------------------------------------------

void DicomSRNumNode::writeMeasuredValueSequence(::gdcm::DataSet& dataset) const
{
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> sequence = new ::gdcm::SequenceOfItems();
    ::gdcm::Item item;
    item.SetVLToUndefined();
    ::gdcm::DataSet& itemDataset = item.GetNestedDataSet();

    // Add numerical value - Type 1
    io::dicom::helper::DicomDataWriter::setTagValues<double, 0x0040, 0xa30a>(&m_numValue, 1, itemDataset);

    // Add measured units code sequence - Type 1
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> codeSequence =
        this->createConceptNameCodeSequence(m_measurementUnits);
    io::dicom::helper::DicomDataWriter::setAndMergeSequenceTagValue<0x0040, 0x08ea>(codeSequence, itemDataset);

    sequence->AddItem(item);
    io::dicom::helper::DicomDataWriter::setSequenceTagValue<0x0040, 0xa300>(sequence, dataset);
}

//------------------------------------------------------------------------------

void DicomSRNumNode::print(std::ostream& os) const
{
    DicomSRNode::print(os);
    os << "\\nMeasurement units : [" << m_measurementUnits << "]";
    os << "\\nMeasurement value : [" << m_numValue << "]";
}

//------------------------------------------------------------------------------

} //namespace sr

} //namespace container

} //namespace sight::io::dicom
