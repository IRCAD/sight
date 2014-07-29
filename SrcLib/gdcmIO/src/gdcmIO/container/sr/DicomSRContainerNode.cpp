/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <gdcmSequenceOfItems.h>

#include <fwCore/spyLog.hpp>

#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/container/sr/DicomSRContainerNode.hpp"

namespace gdcmIO
{
namespace container
{
namespace sr
{

//------------------------------------------------------------------------------

DicomSRContainerNode::DicomSRContainerNode(const DicomCodedAttribute& codedAttribute, const std::string& relationship) :
        ::gdcmIO::container::sr::DicomSRNode(codedAttribute, "CONTAINER", relationship)
{
}

//------------------------------------------------------------------------------

DicomSRContainerNode::~DicomSRContainerNode()
{
}

//------------------------------------------------------------------------------

void DicomSRContainerNode::write(::gdcm::DataSet &dataset) const
{
    ::gdcmIO::container::sr::DicomSRNode::write(dataset);

    // Continuity of content - Type 1 - See PS 3.3 C.18.8 (Only for type CONTAINER)
    // NOTE : Continuity is fixed to SEPARATE because it provides a better compatibility.
    ::gdcmIO::helper::DicomData::setTagValue< 0x0040, 0xa050 >("SEPARATE", dataset);
}

//------------------------------------------------------------------------------

} //namespace sr
} //namespace container
} //namespace gdcmIO
