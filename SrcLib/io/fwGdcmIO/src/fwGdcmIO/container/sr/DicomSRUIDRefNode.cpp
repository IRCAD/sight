/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/container/sr/DicomSRUIDRefNode.hpp"
#include "fwGdcmIO/helper/DicomDataWriter.hxx"

namespace fwGdcmIO
{
namespace container
{
namespace sr
{

//------------------------------------------------------------------------------

DicomSRUIDRefNode::DicomSRUIDRefNode(const DicomCodedAttribute& codedAttribute,
                                     const std::string& relationship,
                                     const std::string uidValue) :
    ::fwGdcmIO::container::sr::DicomSRNode(codedAttribute, "UIDREF", relationship),
    m_uidValue(uidValue)
{
}

//------------------------------------------------------------------------------

DicomSRUIDRefNode::~DicomSRUIDRefNode()
{
}

//------------------------------------------------------------------------------

void DicomSRUIDRefNode::write(::gdcm::DataSet &dataset) const
{
    ::fwGdcmIO::container::sr::DicomSRNode::write(dataset);

    // UID Value - Type 1C
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0040, 0xa124 >(m_uidValue, dataset);
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
} //namespace fwGdcmIO
