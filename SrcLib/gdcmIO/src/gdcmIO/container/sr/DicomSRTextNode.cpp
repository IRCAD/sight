/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <gdcmSequenceOfItems.h>

#include <fwCore/spyLog.hpp>

#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/container/sr/DicomSRTextNode.hpp"

namespace gdcmIO
{
namespace container
{
namespace sr
{

//------------------------------------------------------------------------------

DicomSRTextNode::DicomSRTextNode(const DicomCodedAttribute& codedAttribute,
        const std::string& relationship, const std::string textValue) :
        ::gdcmIO::container::sr::DicomSRNode(codedAttribute, "TEXT", relationship), m_textValue(textValue)
{
}

//------------------------------------------------------------------------------

DicomSRTextNode::~DicomSRTextNode()
{
}

//------------------------------------------------------------------------------

void DicomSRTextNode::write(::gdcm::DataSet &dataset) const
{
    ::gdcmIO::container::sr::DicomSRNode::write(dataset);

    // Text Value - Type 1C
    ::gdcmIO::helper::DicomData::setTagValue< 0x0040, 0xa160 >(m_textValue, dataset);
}

//------------------------------------------------------------------------------

void DicomSRTextNode::print(std::ostream& os) const
{
    DicomSRNode::print(os);
    os << "\\nText value : [" << m_textValue << "]";
}

//------------------------------------------------------------------------------

} //namespace sr
} //namespace container
} //namespace gdcmIO
