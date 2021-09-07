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

#include "io/dicom/helper/DicomCodedAttribute.hpp"

#include <regex>

namespace sight::io::dicom
{

namespace helper
{

//------------------------------------------------------------------------------

DicomCodedAttribute::DicomCodedAttributeVectorType DicomCodedAttribute::convertEntryToCodedAttribute(
    const std::string& entry
)
{
    DicomCodedAttributeVectorType result;

    if(!entry.empty())
    {
        const std::regex codeRegex("\\(([^;]+);([^;]+);([^;)]+)\\)");
        std::smatch match;

        auto textIt = entry.begin();

        // Loop through matches
        // - match[0] = Single entry '(AAA;BBB;CCC)'
        // - match[1] = Code Value 'AAA'
        // - match[2] = Coding Scheme Designator 'BBB'
        // - match[3] = Code Meaning 'CCC'
        while(std::regex_search(textIt, entry.end(), match, codeRegex))
        {
            result.push_back(io::dicom::container::DicomCodedAttribute(match[1], match[2], match[3]));
            textIt = match[0].second;
        }
    }

    return result;
}

//------------------------------------------------------------------------------

::gdcm::Segment::BasicCodedEntryVector DicomCodedAttribute::convertEntryToGDCMCodedAttribute(const std::string& entry)
{
    ::gdcm::Segment::BasicCodedEntryVector result;

    DicomCodedAttributeVectorType codedAttributes = convertEntryToCodedAttribute(entry);
    for(const auto& attribute : codedAttributes)
    {
        result.push_back(attribute.toGDCMFormat());
    }

    return result;
}

//------------------------------------------------------------------------------

bool DicomCodedAttribute::checkAndFormatEntry(std::string& entry, bool multipleValue)
{
    const std::string input = entry;

    std::vector<std::string> attributeContainer;

    const std::regex codeRegex("(\\([^;]+;[^;]+;[^;)]+\\))");
    std::smatch match;

    auto textIt = input.begin();

    // Loop through matches
    while(std::regex_search(textIt, input.end(), match, codeRegex))
    {
        attributeContainer.push_back(match[1]);
        textIt = match[0].second;
    }

    // If the entry matches the criterion, we format the entry by concatenating
    // all the matches
    if(attributeContainer.size() == 1 || (attributeContainer.size() > 1 && multipleValue))
    {
        entry = "";
        for(const auto& attribute : attributeContainer)
        {
            entry += attribute;
        }

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------

} //namespace helper

} //namespace sight::io::dicom
