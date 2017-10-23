/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_CONTAINER_DICOMCODEDATTRIBUTE_HPP__
#define __FWGDCMIO_CONTAINER_DICOMCODEDATTRIBUTE_HPP__

#include "fwGdcmIO/config.hpp"

#include <gdcmSegmentHelper.h>
#include <string>
#include <iostream>

namespace fwGdcmIO
{
namespace container
{

/**
 * @brief This class represents a DICOM basic coded entry.
 * It stores the four mains attributes : Code Value, Coding Scheme Designator,
 * Coding Scheme Version and Code Meaning.
 * See DICOM PS 3.3 - Table 8.8-1a. Basic Code Sequence Macro Attributes
 */
class FWGDCMIO_CLASS_API DicomCodedAttribute
{
public:
    /**
     * @brief Constructor
     * @param[in] codeValue Code Value (0008,0100)
     * @param[in] codingSchemeDesignator Coding Scheme Designator (0008,0102)
     * @param[in] codeMeaning Code Meaning (0008,0104)
     * @param[in] codingSchemeVersion Coding Scheme Version (0003,0103)
     */
    FWGDCMIO_API DicomCodedAttribute(const std::string& codeValue = "",
                                     const std::string& codingSchemeDesignator = "",
                                     const std::string& codeMeaning = "",
                                     const std::string& codingSchemeVersion = "");

    /// Returns true if the attribute is empty
    FWGDCMIO_API bool isEmpty() const;

    /**
     * @brief Comparison operators overloading
     * @{ */
    FWGDCMIO_API bool operator==(const DicomCodedAttribute& other) const;
    /**  @} */

    /// Dump operator
    FWGDCMIO_API friend std::ostream& operator<< (std::ostream& os, const DicomCodedAttribute& attribute)
    {
        os << "\\\"" << attribute.m_codeValue << "\\\" ";
        os << "\\\"" << attribute.m_codingSchemeDesignator << "\\\" ";
        os << "\\\"" << attribute.m_codeMeaning << "\\\" ";
        os << "\\\"" << attribute.m_codingSchemeVersion << "\\\" ";
        return os;
    }

    /// Get code value
    const std::string getCodeValue() const
    {
        return m_codeValue;
    }

    /// Set code value
    void setCodeValue(const std::string& codeValue)
    {
        m_codeValue = codeValue;
    }

    /// Get coding scheme designator
    const std::string getCodingSchemeDesignator() const
    {
        return m_codingSchemeDesignator;
    }

    /// Set coding scheme designator
    void setCodingSchemeDesignator(const std::string& codingSchemeDesignator)
    {
        m_codingSchemeDesignator = codingSchemeDesignator;
    }

    /// Get code meaning
    const std::string getCodeMeaning() const
    {
        return m_codeMeaning;
    }

    /// Set code meaning
    void setCodeMeaning(const std::string& codeMeaning)
    {
        m_codeMeaning = codeMeaning;
    }

    /// Get coding scheme version
    const std::string getCodingSchemeVersion() const
    {
        return m_codingSchemeVersion;
    }

    /// Set coding scheme version
    void setCodingSchemeVersion(const std::string& codingSchemeVersion)
    {
        m_codingSchemeVersion = codingSchemeVersion;
    }

    /// Convert entry to GDCM format
    FWGDCMIO_API ::gdcm::SegmentHelper::BasicCodedEntry toGDCMFormat() const;

private:
    /// Code Value (see : Tag(0008,0100) )
    std::string m_codeValue;

    /// Coding Scheme Designator (see : Tag(0008,0102) )
    std::string m_codingSchemeDesignator;

    /// Code Meaning (see : Tag(0008,0103) )
    std::string m_codeMeaning;

    /// Coding Scheme Version (see : Tag(0008,0104) )
    std::string m_codingSchemeVersion;

};

} //namespace container
} //namespace fwGdcmIO

#endif /* __FWGDCMIO_CONTAINER_DICOMCODEDATTRIBUTE_HPP__ */
