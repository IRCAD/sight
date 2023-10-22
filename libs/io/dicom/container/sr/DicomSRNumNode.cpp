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

#include "io/dicom/container/sr/DicomSRNumNode.hpp"

#include "io/dicom/helper/DicomDataWriter.hxx"

#include <utility>

namespace sight::io::dicom::container::sr
{

//------------------------------------------------------------------------------

DicomSRNumNode::DicomSRNumNode(
    const DicomCodedAttribute& _coded_attribute,
    const std::string& _relationship,
    const double _num_value,
    DicomCodedAttribute _measurement_units
) :
    io::dicom::container::sr::DicomSRNode(_coded_attribute, "NUM", _relationship),
    m_numValue(_num_value),
    m_measurementUnits(std::move(_measurement_units))
{
}

//------------------------------------------------------------------------------

DicomSRNumNode::~DicomSRNumNode()
= default;

//------------------------------------------------------------------------------

void DicomSRNumNode::write(gdcm::DataSet& _dataset) const
{
    io::dicom::container::sr::DicomSRNode::write(_dataset);

    // Measured Value Sequence - Type 2
    this->writeMeasuredValueSequence(_dataset);
}

//------------------------------------------------------------------------------

void DicomSRNumNode::writeMeasuredValueSequence(gdcm::DataSet& _dataset) const
{
    gdcm::SmartPointer<gdcm::SequenceOfItems> sequence = new gdcm::SequenceOfItems();
    gdcm::Item item;
    item.SetVLToUndefined();
    gdcm::DataSet& item_dataset = item.GetNestedDataSet();

    // Add numerical value - Type 1
    io::dicom::helper::DicomDataWriter::setTagValues<double, 0x0040, 0xa30a>(&m_numValue, 1, item_dataset);

    // Add measured units code sequence - Type 1
    gdcm::SmartPointer<gdcm::SequenceOfItems> code_sequence =
        this->createConceptNameCodeSequence(m_measurementUnits);
    io::dicom::helper::DicomDataWriter::setAndMergeSequenceTagValue<0x0040, 0x08ea>(code_sequence, item_dataset);

    sequence->AddItem(item);
    io::dicom::helper::DicomDataWriter::setSequenceTagValue<0x0040, 0xa300>(sequence, _dataset);
}

//------------------------------------------------------------------------------

void DicomSRNumNode::print(std::ostream& _os) const
{
    DicomSRNode::print(_os);
    _os << "\\nMeasurement units : [" << m_measurementUnits << "]";
    _os << "\\nMeasurement value : [" << m_numValue << "]";
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::container::sr
