/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

namespace sight::io::dicom::container
{

/**
 * @brief This class represents a DICOM basic coded entry.
 * It stores the four mains attributes : Code Value, Coding Scheme Designator,
 * Coding Scheme Version and Code Meaning.
 * See DICOM PS 3.3 - Table 8.8-1a. Basic Code Sequence Macro Attributes
 */
class IO_DICOM_CLASS_API dicom_coded_attribute
{
public:

    /**
     * @brief Constructor
     * @param[in] codeValue Code Value (0008,0100)
     * @param[in] codingSchemeDesignator Coding Scheme Designator (0008,0102)
     * @param[in] codeMeaning Code Meaning (0008,0104)
     * @param[in] codingSchemeVersion Coding Scheme Version (0003,0103)
     */
    IO_DICOM_API dicom_coded_attribute(
        std::string _code_value               = "",
        std::string _coding_scheme_designator = "",
        std::string _code_meaning             = "",
        std::string _coding_scheme_version    = ""
    );

    /// Returns true if the attribute is empty
    [[nodiscard]] IO_DICOM_API bool is_empty() const;

    /**
     * @brief Comparison operators overloading
     * @{ */
    IO_DICOM_API bool operator==(const dicom_coded_attribute& _other) const;
    /**  @} */

    /// Dump operator
    IO_DICOM_API friend std::ostream& operator<<(std::ostream& _os, const dicom_coded_attribute& _attribute)
    {
        _os << "\\\"" << _attribute.m_code_value << "\\\" ";
        _os << "\\\"" << _attribute.m_coding_scheme_designator << "\\\" ";
        _os << "\\\"" << _attribute.m_code_meaning << "\\\" ";
        _os << "\\\"" << _attribute.m_coding_scheme_version << "\\\" ";
        return _os;
    }

    /// Get code value
    [[nodiscard]] std::string get_code_value() const
    {
        return m_code_value;
    }

    /// Set code value
    void set_code_value(const std::string& _code_value)
    {
        m_code_value = _code_value;
    }

    /// Get coding scheme designator
    [[nodiscard]] std::string get_coding_scheme_designator() const
    {
        return m_coding_scheme_designator;
    }

    /// Set coding scheme designator
    void set_coding_scheme_designator(const std::string& _coding_scheme_designator)
    {
        m_coding_scheme_designator = _coding_scheme_designator;
    }

    /// Get code meaning
    [[nodiscard]] std::string get_code_meaning() const
    {
        return m_code_meaning;
    }

    /// Set code meaning
    void set_code_meaning(const std::string& _code_meaning)
    {
        m_code_meaning = _code_meaning;
    }

    /// Get coding scheme version
    [[nodiscard]] std::string get_coding_scheme_version() const
    {
        return m_coding_scheme_version;
    }

    /// Set coding scheme version
    void set_coding_scheme_version(const std::string& _coding_scheme_version)
    {
        m_coding_scheme_version = _coding_scheme_version;
    }

    /// Convert entry to GDCM format
    [[nodiscard]] IO_DICOM_API gdcm::SegmentHelper::BasicCodedEntry to_gdcm_format() const;

private:

    /// Code Value (see : Tag(0008,0100) )
    std::string m_code_value;

    /// Coding Scheme Designator (see : Tag(0008,0102) )
    std::string m_coding_scheme_designator;

    /// Code Meaning (see : Tag(0008,0103) )
    std::string m_code_meaning;

    /// Coding Scheme Version (see : Tag(0008,0104) )
    std::string m_coding_scheme_version;
};

} // namespace sight::io::dicom::container
