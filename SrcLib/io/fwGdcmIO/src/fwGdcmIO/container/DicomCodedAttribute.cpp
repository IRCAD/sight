/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/container/DicomCodedAttribute.hpp"

#include <fwCore/spyLog.hpp>

namespace fwGdcmIO
{
namespace container
{

//------------------------------------------------------------------------------

DicomCodedAttribute::DicomCodedAttribute(const std::string& codeValue, const std::string& codingSchemeDesignator,
                                         const std::string& codeMeaning, const std::string& codingSchemeVersion) :
    m_codeValue(codeValue),
    m_codingSchemeDesignator(codingSchemeDesignator),
    m_codeMeaning(codeMeaning),
    m_codingSchemeVersion(codingSchemeVersion)
{
}

//------------------------------------------------------------------------------

bool DicomCodedAttribute::isEmpty() const
{
    return m_codeValue == "" && m_codingSchemeDesignator == "" && m_codeMeaning == "" && m_codingSchemeVersion == "";
}

//------------------------------------------------------------------------------

bool DicomCodedAttribute::operator==(const DicomCodedAttribute& other) const
{
    return m_codeValue == other.m_codeValue && m_codingSchemeDesignator == other.m_codingSchemeDesignator
           && m_codeMeaning == other.m_codeMeaning && m_codingSchemeVersion == other.m_codingSchemeVersion;
}

//------------------------------------------------------------------------------

} //namespace container
} //namespace fwGdcmIO
