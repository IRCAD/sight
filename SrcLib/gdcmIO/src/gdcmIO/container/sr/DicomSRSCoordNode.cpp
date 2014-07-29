/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <gdcmSequenceOfItems.h>

#include <fwCore/spyLog.hpp>

#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/container/sr/DicomSRSCoordNode.hpp"

namespace gdcmIO
{
namespace container
{
namespace sr
{

//------------------------------------------------------------------------------

DicomSRSCoordNode::DicomSRSCoordNode(const DicomCodedAttribute& codedAttribute, const std::string& relationship,
        const std::string& graphicType, GraphicDataContainerType graphicDataContainer) :
        ::gdcmIO::container::sr::DicomSRNode(codedAttribute, "SCOORD", relationship),
         m_graphicType(graphicType), m_graphicDataContainer(graphicDataContainer)
{
    SLM_ASSERT("Only POINT and POLYLINE are supported by SCoord node for now.",
            graphicType == "POINT" || graphicType == "POLYLINE");

    SLM_ASSERT("Graphic data doesn't match graphic type POINT.",
            m_graphicType != "POINT" || m_graphicDataContainer.size() == 2);

    SLM_ASSERT("Graphic data doesn't match graphic type POLYLINE.",
                m_graphicType != "POLYLINE" || m_graphicDataContainer.size() == 4);

}

//------------------------------------------------------------------------------

DicomSRSCoordNode::~DicomSRSCoordNode()
{
}

//------------------------------------------------------------------------------

void DicomSRSCoordNode::write(::gdcm::DataSet &dataset) const
{
    ::gdcmIO::container::sr::DicomSRNode::write(dataset);

    // Graphic Data - Type 1
    ::gdcmIO::helper::DicomData::setTagValues< float, 0x0070, 0x0022 >(
            &m_graphicDataContainer[0], m_graphicDataContainer.size(), dataset);

    // Graphic Type - Type 1
    ::gdcmIO::helper::DicomData::setTagValue< 0x0070, 0x0023 >(m_graphicType, dataset);
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
} //namespace gdcmIO
