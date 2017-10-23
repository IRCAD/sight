/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/container/DicomCodedAttribute.hpp"

namespace fwGdcmIO
{
namespace container
{

//------------------------------------------------------------------------------

DicomCodedAttribute::DicomCodedAttribute(const std::string& codeValue,
                                         const std::string& codingSchemeDesignator,
                                         const std::string& codeMeaning,
                                         const std::string& codingSchemeVersion) :
    m_codeValue(codeValue),
    m_codingSchemeDesignator(codingSchemeDesignator),
    m_codeMeaning(codeMeaning),
    m_codingSchemeVersion(codingSchemeVersion)
{
}

//------------------------------------------------------------------------------

bool DicomCodedAttribute::isEmpty() const
{
    return m_codeValue.empty() &&
           m_codingSchemeDesignator.empty() &&
           m_codeMeaning.empty() &&
           m_codingSchemeVersion.empty();
}

//------------------------------------------------------------------------------

bool DicomCodedAttribute::operator==(const DicomCodedAttribute& other) const
{
    return m_codeValue == other.m_codeValue &&
           m_codingSchemeDesignator == other.m_codingSchemeDesignator &&
           m_codeMeaning == other.m_codeMeaning &&
           m_codingSchemeVersion == other.m_codingSchemeVersion;
}

//------------------------------------------------------------------------------

::gdcm::SegmentHelper::BasicCodedEntry DicomCodedAttribute::toGDCMFormat() const
{
    return ::gdcm::SegmentHelper::BasicCodedEntry(m_codeValue.c_str(),
                                                  m_codingSchemeDesignator.c_str(),
                                                  m_codingSchemeVersion.c_str(),
                                                  m_codeMeaning.c_str());
}

//------------------------------------------------------------------------------
} //namespace container
} //namespace fwGdcmIO
