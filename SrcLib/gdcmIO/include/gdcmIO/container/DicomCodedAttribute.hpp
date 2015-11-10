/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_CONTAINER_DICOMCODEDATTRIBUTE_HPP__
#define __GDCMIO_CONTAINER_DICOMCODEDATTRIBUTE_HPP__

#include "gdcmIO/config.hpp"

#include <string>
#include <vector>
#include <ostream>


namespace gdcmIO
{
namespace container
{

/**
 * @class DicomCodedAttribute
 * @brief This class defines a coded entry of a SR
 */
class GDCMIO_CLASS_API DicomCodedAttribute
{
public:
    /// Constructor
    GDCMIO_API DicomCodedAttribute(const std::string& codeValue = "", const std::string& codingSchemeDesignator = "",
                                   const std::string& codeMeaning = "", const std::string& codingSchemeVersion = "");

    /// Return true if the attribute is empty
    GDCMIO_API bool isEmpty() const;

    /// Return true if the DicomCodedAttribute are the same
    GDCMIO_API bool operator==(const DicomCodedAttribute& other) const;

    /// Dump operator
    friend std::ostream& operator<< (std::ostream& os, const DicomCodedAttribute& attribute)
    {
        os << "\\\"" << attribute.m_codeValue << "\\\" ";
        os << "\\\"" << attribute.m_codingSchemeDesignator << "\\\" ";
        os << "\\\"" << attribute.m_codeMeaning << "\\\" ";
        os << "\\\"" << attribute.m_codingSchemeVersion << "\\\" ";
        return os;
    }

    /**
     * @brief  Retrieve code meaning
     * @{ */
    const std::string& getCodeMeaning() const
    {
        return m_codeMeaning;
    }
    void setCodeMeaning(const std::string& codeMeaning)
    {
        m_codeMeaning = codeMeaning;
    }
    /**  @} */

    /**
     * @brief Retrieve code value
     * @{ */
    const std::string& getCodeValue() const
    {
        return m_codeValue;
    }
    void setCodeValue(const std::string& codeValue)
    {
        m_codeValue = codeValue;
    }
    /**  @} */

    /**
     * @brief Retrieve coding scheme designator
     * @{ */
    const std::string& getCodingSchemeDesignator() const
    {
        return m_codingSchemeDesignator;
    }
    void setCodingSchemeDesignator(const std::string& codingSchemeDesignator)
    {
        m_codingSchemeDesignator = codingSchemeDesignator;
    }
    /**  @} */

    /**
     * @brief Retrieve coding scheme version
     * @{ */
    const std::string& getCodingSchemeVersion() const
    {
        return m_codingSchemeVersion;
    }
    void setCodingSchemeVersion(const std::string& codingSchemeVersion)
    {
        m_codingSchemeVersion = codingSchemeVersion;
    }
    /**  @} */

protected:
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
} //namespace gdcmIO

#endif /* __GDCMIO_CONTAINER_DICOMCODEDATTRIBUTE_HPP__ */
