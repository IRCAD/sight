/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <gdcmSequenceOfItems.h>
#include <gdcmUIDGenerator.h>

#include <fwCore/spyLog.hpp>

#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/container/sr/DicomSRCodeNode.hpp"

namespace gdcmIO
{
namespace container
{
namespace sr
{

//------------------------------------------------------------------------------

DicomSRCodeNode::DicomSRCodeNode(const DicomCodedAttribute& codedAttribute, const std::string& relationship
        , const DicomCodedAttribute& codedEntry) :
        ::gdcmIO::container::sr::DicomSRNode(codedAttribute, "CODE", relationship), m_codedEntry(codedEntry)
{
}

//------------------------------------------------------------------------------

DicomSRCodeNode::~DicomSRCodeNode()
{
}

//------------------------------------------------------------------------------

void DicomSRCodeNode::write(::gdcm::DataSet &dataset) const
{
    ::gdcmIO::container::sr::DicomSRNode::write(dataset);

    // Concept Code Sequence - Type 1
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > codeSequence =
            this->createConceptNameCodeSequence(dataset, m_codedEntry);
    ::gdcmIO::helper::DicomData::insertSQ< 0x0040, 0xa168 >(codeSequence, dataset);
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
} //namespace gdcmIO
