/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "io/dicom/config.hpp"

#include <gdcmSegmentHelper.h>

#include <iostream>
#include <string>

namespace sight::io::dicom
{

namespace container
{

/**
 * @brief This class represents a DICOM basic coded entry.
 * It stores the four mains attributes : Code Value, Coding Scheme Designator,
 * Coding Scheme Version and Code Meaning.
 * See DICOM PS 3.3 - Table 8.8-1a. Basic Code Sequence Macro Attributes
 */
class IO_DICOM_CLASS_API DicomCodedAttribute
{
public:

    /**
     * @brief Constructor
     * @param[in] codeValue Code Value (0008,0100)
     * @param[in] codingSchemeDesignator Coding Scheme Designator (0008,0102)
     * @param[in] codeMeaning Code Meaning (0008,0104)
     * @param[in] codingSchemeVersion Coding Scheme Version (0003,0103)
     */
    IO_DICOM_API DicomCodedAttribute(
        const std::string& codeValue              = "",
        const std::string& codingSchemeDesignator = "",
        const std::string& codeMeaning            = "",
        const std::string& codingSchemeVersion    = ""
    );

    /// Returns true if the attribute is empty
    IO_DICOM_API bool isEmpty() const;

    /**
     * @brief Comparison operators overloading
     * @{ */
    IO_DICOM_API bool operator==(const DicomCodedAttribute& other) const;
    /**  @} */

    /// Dump operator
    IO_DICOM_API friend std::ostream& operator<<(std::ostream& os, const DicomCodedAttribute& attribute)
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
    IO_DICOM_API ::gdcm::SegmentHelper::BasicCodedEntry toGDCMFormat() const;

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

} //namespace sight::io::dicom
