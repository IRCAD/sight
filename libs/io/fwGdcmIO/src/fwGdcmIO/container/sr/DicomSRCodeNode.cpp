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

#include "fwGdcmIO/container/sr/DicomSRCodeNode.hpp"

#include "fwGdcmIO/helper/DicomDataWriter.hxx"

namespace fwGdcmIO
{
namespace container
{
namespace sr
{

//------------------------------------------------------------------------------

DicomSRCodeNode::DicomSRCodeNode(const DicomCodedAttribute& codedAttribute, const std::string& relationship
                                 , const DicomCodedAttribute& codedEntry) :
    ::fwGdcmIO::container::sr::DicomSRNode(codedAttribute, "CODE", relationship),
    m_codedEntry(codedEntry)
{
}

//------------------------------------------------------------------------------

DicomSRCodeNode::~DicomSRCodeNode()
{
}

//------------------------------------------------------------------------------

void DicomSRCodeNode::write(::gdcm::DataSet& dataset) const
{
    ::fwGdcmIO::container::sr::DicomSRNode::write(dataset);

    // Concept Code Sequence - Type 1
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > codeSequence =
        this->createConceptNameCodeSequence(m_codedEntry);
    ::fwGdcmIO::helper::DicomDataWriter::setAndMergeSequenceTagValue< 0x0040, 0xa168 >(codeSequence, dataset);
}

//------------------------------------------------------------------------------

void DicomSRCodeNode::print(std::ostream& os) const
{
    DicomSRNode::print(os);
    os << "\\nCoded entry : [" << m_codedEntry << "]";
}

//------------------------------------------------------------------------------

} //namespace sr
} //namespace container
} //namespace fwGdcmIO
