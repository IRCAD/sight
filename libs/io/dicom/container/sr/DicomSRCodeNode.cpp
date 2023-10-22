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

#include "io/dicom/container/sr/DicomSRCodeNode.hpp"

#include "io/dicom/helper/DicomDataWriter.hxx"

#include <utility>

namespace sight::io::dicom::container::sr
{

//------------------------------------------------------------------------------

DicomSRCodeNode::DicomSRCodeNode(
    const DicomCodedAttribute& _coded_attribute,
    const std::string& _relationship
    ,
    DicomCodedAttribute _coded_entry
) :
    io::dicom::container::sr::DicomSRNode(_coded_attribute, "CODE", _relationship),
    m_codedEntry(std::move(_coded_entry))
{
}

//------------------------------------------------------------------------------

DicomSRCodeNode::~DicomSRCodeNode()
= default;

//------------------------------------------------------------------------------

void DicomSRCodeNode::write(gdcm::DataSet& _dataset) const
{
    io::dicom::container::sr::DicomSRNode::write(_dataset);

    // Concept Code Sequence - Type 1
    gdcm::SmartPointer<gdcm::SequenceOfItems> code_sequence =
        this->createConceptNameCodeSequence(m_codedEntry);
    io::dicom::helper::DicomDataWriter::setAndMergeSequenceTagValue<0x0040, 0xa168>(code_sequence, _dataset);
}

//------------------------------------------------------------------------------

void DicomSRCodeNode::print(std::ostream& _os) const
{
    DicomSRNode::print(_os);
    _os << "\\nCoded entry : [" << m_codedEntry << "]";
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::container::sr
