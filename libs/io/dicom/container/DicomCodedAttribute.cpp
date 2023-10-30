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

#include "io/dicom/container/DicomCodedAttribute.hpp"

#include <utility>

namespace sight::io::dicom::container
{

//------------------------------------------------------------------------------

dicom_coded_attribute::dicom_coded_attribute(
    std::string _code_value,
    std::string _coding_scheme_designator,
    std::string _code_meaning,
    std::string _coding_scheme_version
) :
    m_code_value(std::move(_code_value)),
    m_coding_scheme_designator(std::move(_coding_scheme_designator)),
    m_code_meaning(std::move(_code_meaning)),
    m_coding_scheme_version(std::move(_coding_scheme_version))
{
}

//------------------------------------------------------------------------------

bool dicom_coded_attribute::is_empty() const
{
    return m_code_value.empty()
           && m_coding_scheme_designator.empty()
           && m_code_meaning.empty()
           && m_coding_scheme_version.empty();
}

//------------------------------------------------------------------------------

bool dicom_coded_attribute::operator==(const dicom_coded_attribute& _other) const
{
    return m_code_value == _other.m_code_value
           && m_coding_scheme_designator == _other.m_coding_scheme_designator
           && m_code_meaning == _other.m_code_meaning
           && m_coding_scheme_version == _other.m_coding_scheme_version;
}

//------------------------------------------------------------------------------

gdcm::SegmentHelper::BasicCodedEntry dicom_coded_attribute::to_gdcm_format() const
{
    return {
        m_code_value.c_str(),
        m_coding_scheme_designator.c_str(),
        m_coding_scheme_version.c_str(),
        m_code_meaning.c_str()
    };
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::container
