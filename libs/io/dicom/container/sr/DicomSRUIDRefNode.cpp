/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "io/dicom/container/sr/DicomSRUIDRefNode.hpp"

#include "io/dicom/helper/DicomDataWriter.hxx"

namespace sight::io::dicom
{

namespace container
{

namespace sr
{

//------------------------------------------------------------------------------

DicomSRUIDRefNode::DicomSRUIDRefNode(
    const DicomCodedAttribute& codedAttribute,
    const std::string& relationship,
    const std::string uidValue
) :
    io::dicom::container::sr::DicomSRNode(codedAttribute, "UIDREF", relationship),
    m_uidValue(uidValue)
{
}

//------------------------------------------------------------------------------

DicomSRUIDRefNode::~DicomSRUIDRefNode()
{
}

//------------------------------------------------------------------------------

void DicomSRUIDRefNode::write(gdcm::DataSet& dataset) const
{
    io::dicom::container::sr::DicomSRNode::write(dataset);

    // UID Value - Type 1C
    io::dicom::helper::DicomDataWriter::setTagValue<0x0040, 0xa124>(m_uidValue, dataset);
}

//------------------------------------------------------------------------------

void DicomSRUIDRefNode::print(std::ostream& os) const
{
    DicomSRNode::print(os);
    os << "\\nUID value : [" << m_uidValue << "]";
}

//------------------------------------------------------------------------------

} //namespace sr

} //namespace container

} //namespace sight::io::dicom
