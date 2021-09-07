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

#include "io/dicom/container/sr/DicomSRSCoordNode.hpp"

#include "io/dicom/helper/DicomDataWriter.hxx"

#include <service/macros.hpp>

namespace sight::io::dicom
{

namespace container
{

namespace sr
{

//------------------------------------------------------------------------------

DicomSRSCoordNode::DicomSRSCoordNode(
    const DicomCodedAttribute& codedAttribute,
    const std::string& relationship,
    const std::string& graphicType,
    GraphicDataContainerType graphicDataContainer
) :
    io::dicom::container::sr::DicomSRNode(codedAttribute, "SCOORD", relationship),
    m_graphicType(graphicType),
    m_graphicDataContainer(graphicDataContainer)
{
    SIGHT_ASSERT(
        "Only POINT and POLYLINE are supported by SCoord node for now.",
        graphicType == "POINT" || graphicType == "POLYLINE"
    );

    SIGHT_ASSERT(
        "Graphic data doesn't match graphic type POINT.",
        m_graphicType != "POINT" || m_graphicDataContainer.size() == 2
    );

    SIGHT_ASSERT(
        "Graphic data doesn't match graphic type POLYLINE.",
        m_graphicType != "POLYLINE" || m_graphicDataContainer.size() == 4
    );
}

//------------------------------------------------------------------------------

DicomSRSCoordNode::~DicomSRSCoordNode()
{
}

//------------------------------------------------------------------------------

void DicomSRSCoordNode::write(::gdcm::DataSet& dataset) const
{
    io::dicom::container::sr::DicomSRNode::write(dataset);

    // Graphic Data - Type 1
    io::dicom::helper::DicomDataWriter::setTagValues<float, 0x0070, 0x0022>(
        &m_graphicDataContainer[0],
        m_graphicDataContainer.size(),
        dataset
    );

    // Graphic Type - Type 1
    io::dicom::helper::DicomDataWriter::setTagValue<0x0070, 0x0023>(m_graphicType, dataset);
}

//------------------------------------------------------------------------------

void DicomSRSCoordNode::print(std::ostream& os) const
{
    DicomSRNode::print(os);
    os << "\\nGraphic Type : [" << m_graphicType << "]";
}

//------------------------------------------------------------------------------

} //namespace sr

} //namespace container

} //namespace sight::io::dicom
