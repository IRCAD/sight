/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "io/dicom/container/DicomCodedAttribute.hpp"

#include <utility>

namespace sight::io::dicom::container
{

//------------------------------------------------------------------------------

DicomCodedAttribute::DicomCodedAttribute(
    std::string codeValue,
    std::string codingSchemeDesignator,
    std::string codeMeaning,
    std::string codingSchemeVersion
) :
    m_codeValue(std::move(codeValue)),
    m_codingSchemeDesignator(std::move(codingSchemeDesignator)),
    m_codeMeaning(std::move(codeMeaning)),
    m_codingSchemeVersion(std::move(codingSchemeVersion))
{
}

//------------------------------------------------------------------------------

bool DicomCodedAttribute::isEmpty() const
{
    return m_codeValue.empty()
           && m_codingSchemeDesignator.empty()
           && m_codeMeaning.empty()
           && m_codingSchemeVersion.empty();
}

//------------------------------------------------------------------------------

bool DicomCodedAttribute::operator==(const DicomCodedAttribute& other) const
{
    return m_codeValue == other.m_codeValue
           && m_codingSchemeDesignator == other.m_codingSchemeDesignator
           && m_codeMeaning == other.m_codeMeaning
           && m_codingSchemeVersion == other.m_codingSchemeVersion;
}

//------------------------------------------------------------------------------

gdcm::SegmentHelper::BasicCodedEntry DicomCodedAttribute::toGDCMFormat() const
{
    return {
        m_codeValue.c_str(),
        m_codingSchemeDesignator.c_str(),
        m_codingSchemeVersion.c_str(),
        m_codeMeaning.c_str()
    };
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::container
