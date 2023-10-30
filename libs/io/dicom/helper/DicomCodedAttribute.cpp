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

#include "io/dicom/helper/DicomCodedAttribute.hpp"

#include <regex>

namespace sight::io::dicom::helper
{

//------------------------------------------------------------------------------

dicom_coded_attribute::DicomCodedAttributeVectorType dicom_coded_attribute::convert_entry_to_coded_attribute(
    const std::string& _entry
)
{
    DicomCodedAttributeVectorType result;

    if(!_entry.empty())
    {
        const std::regex code_regex("\\(([^;]+);([^;]+);([^;)]+)\\)");
        std::smatch match;

        auto text_it = _entry.begin();

        // Loop through matches
        // - match[0] = Single entry '(AAA;BBB;CCC)'
        // - match[1] = Code Value 'AAA'
        // - match[2] = Coding Scheme Designator 'BBB'
        // - match[3] = Code Meaning 'CCC'
        while(std::regex_search(text_it, _entry.end(), match, code_regex))
        {
            result.emplace_back(match[1], match[2], match[3]);
            text_it = match[0].second;
        }
    }

    return result;
}

//------------------------------------------------------------------------------

gdcm::Segment::BasicCodedEntryVector dicom_coded_attribute::convert_entry_to_gdcm_coded_attribute(
    const std::string& _entry
)
{
    gdcm::Segment::BasicCodedEntryVector result;

    DicomCodedAttributeVectorType coded_attributes = convert_entry_to_coded_attribute(_entry);
    for(const auto& attribute : coded_attributes)
    {
        result.push_back(attribute.to_gdcm_format());
    }

    return result;
}

//------------------------------------------------------------------------------

bool dicom_coded_attribute::check_and_format_entry(std::string& _entry, bool _multiple_value)
{
    const std::string input = _entry;

    std::vector<std::string> attribute_container;

    const std::regex code_regex("(\\([^;]+;[^;]+;[^;)]+\\))");
    std::smatch match;

    auto text_it = input.begin();

    // Loop through matches
    while(std::regex_search(text_it, input.end(), match, code_regex))
    {
        attribute_container.push_back(match[1]);
        text_it = match[0].second;
    }

    // If the entry matches the criterion, we format the entry by concatenating
    // all the matches
    if(attribute_container.size() == 1 || (attribute_container.size() > 1 && _multiple_value))
    {
        _entry = "";
        for(const auto& attribute : attribute_container)
        {
            _entry += attribute;
        }

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
