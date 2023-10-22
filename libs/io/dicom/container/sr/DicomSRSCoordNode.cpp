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

#include "io/dicom/container/sr/DicomSRSCoordNode.hpp"

#include "io/dicom/helper/DicomDataWriter.hxx"

#include <service/macros.hpp>

namespace sight::io::dicom::container::sr
{

//------------------------------------------------------------------------------

DicomSRSCoordNode::DicomSRSCoordNode(
    const DicomCodedAttribute& _coded_attribute,
    const std::string& _relationship,
    std::string _graphic_type,
    graphic_data_container_t _graphic_data_container
) :
    io::dicom::container::sr::DicomSRNode(_coded_attribute, "SCOORD", _relationship),
    m_graphicType(std::move(_graphic_type)),
    m_graphicDataContainer(std::move(_graphic_data_container))
{
    SIGHT_ASSERT(
        "Only POINT and POLYLINE are supported by SCoord node for now.",
        m_graphicType == "POINT" || m_graphicType == "POLYLINE"
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
= default;

//------------------------------------------------------------------------------

void DicomSRSCoordNode::write(gdcm::DataSet& _dataset) const
{
    io::dicom::container::sr::DicomSRNode::write(_dataset);

    // Graphic Data - Type 1
    io::dicom::helper::DicomDataWriter::setTagValues<float, 0x0070, 0x0022>(
        (m_graphicDataContainer).data(),
        m_graphicDataContainer.size(),
        _dataset
    );

    // Graphic Type - Type 1
    io::dicom::helper::DicomDataWriter::setTagValue<0x0070, 0x0023>(m_graphicType, _dataset);
}

//------------------------------------------------------------------------------

void DicomSRSCoordNode::print(std::ostream& _os) const
{
    DicomSRNode::print(_os);
    _os << "\\nGraphic Type : [" << m_graphicType << "]";
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::container::sr
