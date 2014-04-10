/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_CONTAINER_DICOMCODEDATTRIBUTE_HPP__
#define __GDCMIO_CONTAINER_DICOMCODEDATTRIBUTE_HPP__

#include <string>

#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace container
{

/**
 * @class DicomCodedAttribute
 * @brief Structure for basic coded entry. It defines the identification of a coded content.
 * @see DICOM PS 3.3 Table 8.8-1
 */
struct DicomCodedAttribute
{
    GDCMIO_API DicomCodedAttribute() :
            m_codeValue(""),
            m_codingSchemeDesignator(""),
            m_codingSchemeVersion(""),
            m_codeMeaning("")
    {
    }

    GDCMIO_API DicomCodedAttribute(const char * codeValue, const char * codingSchemeDesignator,
            const char * codeMeaning, const char * codingSchemeVersion = "") :
            m_codeValue(codeValue),
            m_codingSchemeDesignator(codingSchemeDesignator),
            m_codingSchemeVersion(codingSchemeVersion),
            m_codeMeaning(codeMeaning)
    {
    }

    GDCMIO_API ~DicomCodedAttribute()
    {
    }

    /// Code Value (see : Tag(0008,0100) )
    std::string m_codeValue;

    /// Code Scheme Designator (see : Tag(0008,0102) )
    std::string m_codingSchemeDesignator;

    ///Code Scheme Version (see : Tag(0008,0103) )
    std::string m_codingSchemeVersion;

    /// Code Meaning (see : Tag(0008,0104) )
    std::string m_codeMeaning;

};

} //namespace container
} //namespace gdcmIO

#endif /* __GDCMIO_CONTAINER_DICOMCODEDATTRIBUTE_HPP__ */
