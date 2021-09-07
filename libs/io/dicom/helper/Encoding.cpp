/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "io/dicom/helper/Encoding.hpp"

#include <core/log/Logger.hpp>
#include <core/spyLog.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/locale/encoding.hpp>

namespace sight::io::dicom
{

namespace helper
{

const Encoding::DefinedTermToCharsetMapType Encoding::s_DEFINED_TERM_TO_CHARSET = {
    // ASCII
    {"ISO_IR 6", ""},
    {"ISO 2022 IR 6", ""},

    // Latin alphabet No. 1
    {"ISO_IR 100", "ISO-8859-1"},
    {"ISO 2022 IR 100", "ISO-8859-1"},

    // Latin alphabet No. 2
    {"ISO_IR 101", "ISO-8859-2"},
    {"ISO 2022 IR 101", "ISO-8859-2"},

    // Latin alphabet No. 3
    {"ISO_IR 109", "ISO-8859-3"},
    {"ISO 2022 IR 109", "ISO-8859-3"},

    // Latin alphabet No. 4
    {"ISO_IR 110", "ISO-8859-4"},
    {"ISO 2022 IR 110", "ISO-8859-4"},

    // Cyrillic
    {"ISO_IR 144", "ISO-8859-5"},
    {"ISO 2022 IR 144", "ISO-8859-5"},

    // Arabic
    {"ISO_IR 127", "ISO-8859-6"},
    {"ISO 2022 IR 127", "ISO-8859-6"},

    // Greek
    {"ISO_IR 126", "ISO-8859-7"},
    {"ISO 2022 IR 126", "ISO-8859-7"},

    // Hebrew
    {"ISO_IR 138", "ISO-8859-8"},
    {"ISO 2022 IR 138", "ISO-8859-8"},

    // Latin alphabet No. 5
    {"ISO_IR 148", "ISO-8859-9"},
    {"ISO 2022 IR 148", "ISO-8859-9"},

    // Japanese { may require "ISO-IR-13" },
    {"ISO_IR 13", "JIS_X0201"},
    {"ISO 2022 IR 13", "JIS_X0201"},

    // Thai
    {"ISO_IR 166", "ISO-IR-166"},
    {"ISO 2022 IR 166", "ISO-IR-166"},

    // Japanese
    {"ISO 2022 IR 87", "ISO-IR-87"},
    {"ISO 2022 IR 159", "ISO-IR-159"},

    // Korean
    // - is this mapping really correct?
    {"ISO 2022 IR 149", "EUC-KR"},

    // Simplified Chinese
    // - is this mapping really correct?
    {"ISO 2022 IR 58", "GB2312"},

    // Unicode in UTF-8 { multi-byte },
    {"ISO_IR 192", "UTF-8"},

    // Chinese { multi-byte },
    {"GB18030", "GB18030"},

    // Simplified Chinese { multi-byte },
    {"GBK", "GBK"},
};

const Encoding::EscapeSequenceToCharsetMapType Encoding::s_ESCAPE_SEQUENCE_TO_CHARSET = {
    {{0x28, 0x42}, {"ISO 2022 IR 6", ""}},             // ASCII
    {{0x2d, 0x41}, {"ISO 2022 IR 100", "ISO-8859-1"}}, // Latin alphabet No. 1
    {{0x2d, 0x42}, {"ISO 2022 IR 101", "ISO-8859-2"}}, // Latin alphabet No. 2
    {{0x2d, 0x43}, {"ISO 2022 IR 109", "ISO-8859-3"}}, // Latin alphabet No. 3
    {{0x2d, 0x44}, {"ISO 2022 IR 110", "ISO-8859-4"}}, // Latin alphabet No. 4
    {{0x2d, 0x4c}, {"ISO 2022 IR 144", "ISO-8859-5"}}, // Cyrillic
    {{0x2d, 0x47}, {"ISO 2022 IR 127", "ISO-8859-6"}}, // Arabic
    {{0x2d, 0x46}, {"ISO 2022 IR 126", "ISO-8859-7"}}, // Greek
    {{0x2d, 0x48}, {"ISO 2022 IR 138", "ISO-8859-8"}}, // Hebrew
    {{0x2d, 0x4d}, {"ISO 2022 IR 148", "ISO-8859-9"}}, // Latin alphabet No. 5
    {{0x29, 0x49}, {"ISO 2022 IR 13", "JIS_X0201"}},   // Japanese { may require "ISO-IR-13"}
    {{0x28, 0x4a}, {"ISO 2022 IR 13", "ISO-IR-14"}},   // Japanese
    {{0x2d, 0x54}, {"ISO 2022 IR 166", "ISO-IR-166"}}, // Thai
    {{0x24, 0x42}, {"ISO 2022 IR 87", "ISO-IR-87"}} // Japanese { multi-byte}
};

//------------------------------------------------------------------------------

std::string Encoding::convertString(
    const std::string& source,
    const std::string& definedCharsetTerm,
    const core::log::Logger::sptr& logger
)
{
    if(source.empty())
    {
        return "";
    }

    // Retrieve DICOM Specific Character Set List
    std::vector<std::string> definedTermList;
    ::boost::split(definedTermList, definedCharsetTerm, ::boost::is_any_of("\\"));

    // Only one charset without code extension techniques is used
    if(definedCharsetTerm.empty() || definedTermList.size() == 1)
    {
        return convertStringWithoutCodeExtensions(source, definedCharsetTerm, logger);
    }
    // Several charsets with code extension techniques are used
    else
    {
        // If the attribute Specific Character Set (0008,0005) has more than one value
        // and value 1 is empty, it is assumed that value 1 is ISO 2022 IR 6.
        if(definedTermList[0].empty())
        {
            definedTermList[0] = "ISO 2022 IR 6";
        }

        // Check for characters ESC delimiter
        std::vector<std::string> sequenceList;
        ::boost::split(sequenceList, source, ::boost::is_any_of("\033"));

        std::string result;

        // Add the first part
        if(source[0] != '\033')
        {
            result += convertStringWithoutCodeExtensions(sequenceList[0], definedTermList[0], logger);
        }
        else
        {
            result += Encoding::convertSequenceWithCodeExtensions(sequenceList[0], definedTermList, logger);
        }

        // Convert remaining sequences according to specific charsets
        std::vector<std::string>::iterator it = ++sequenceList.begin();
        for( ; it != sequenceList.end() ; ++it)
        {
            result += convertSequenceWithCodeExtensions(*it, definedTermList, logger);
        }

        return result;
    }
}

//------------------------------------------------------------------------------

std::string Encoding::convertStringWithoutCodeExtensions(
    const std::string& source,
    const std::string& definedTerm,
    const core::log::Logger::sptr& logger
)
{
    std::string charset;

    if(definedTerm.empty()) // assuming ASCII (according to DICOM PS 3.5)
    {
        charset = "";
    }
    else
    {
        SIGHT_WARN_IF(
            "'ISO_IR 6' is not a defined term in DICOM, will be treated as an empty value (ASCII)",
            definedTerm == "ISO_IR 6"
        );

        // Check that the defined term is known
        if(s_DEFINED_TERM_TO_CHARSET.find(definedTerm) != s_DEFINED_TERM_TO_CHARSET.end())
        {
            charset = s_DEFINED_TERM_TO_CHARSET.at(definedTerm);
        }
        else
        {
            const std::string msg = "'" + definedTerm + "' is not a defined term in DICOM, "
                                                        "will be treated as an empty value (ASCII)";

            SIGHT_WARN_IF(msg, !logger);
            if(logger)
            {
                logger->warning(msg);
            }

            charset = "";
        }
    }

    // Empty value treated as ASCII
    if(charset.empty())
    {
        return source;
    }
    else
    {
        return ::boost::locale::conv::to_utf<char>(source, charset);
    }
}

//------------------------------------------------------------------------------

void checkDefinedTermDeclaration(
    const std::string& definedTerm,
    const std::vector<std::string>& definedTermList,
    const core::log::Logger::sptr& logger
)
{
    if(std::find(definedTermList.begin(), definedTermList.end(), definedTerm) == definedTermList.end())
    {
        const std::string msg = "Escape sequence refers to character set '" + definedTerm
                                + "' that was not declared in SpecificCharacterSet (0008,0005).";

        SIGHT_WARN_IF(msg, !logger);
        if(logger)
        {
            logger->warning(msg);
        }
    }
}

//------------------------------------------------------------------------------

std::string Encoding::convertSequenceWithCodeExtensions(
    const std::string& sequence,
    const std::vector<std::string>& definedTermList,
    const core::log::Logger::sptr& logger
)
{
    // We need at least two more characters to determine the new character set
    SIGHT_THROW_IF("Cannot convert character set: Incomplete escape sequence.", sequence.size() < 2);

    const char c1 = sequence[0];
    const char c2 = sequence[1];

    unsigned short escapeSize = 2;

    EscapeSequenceType escapeSequence                   = std::make_pair(c1, c2);
    DefinedTermAndCharsetPairType definedTermAndCharset = std::make_pair("", "");

    if(s_ESCAPE_SEQUENCE_TO_CHARSET.find(escapeSequence) != s_ESCAPE_SEQUENCE_TO_CHARSET.end())
    {
        definedTermAndCharset = s_ESCAPE_SEQUENCE_TO_CHARSET.at(escapeSequence);
    }
    else if((c1 == 0x24) && (c2 == 0x28)) // Japanese (multi-byte)
    {
        // Do we still have another character in the string?
        if(sequence.size() >= 3)
        {
            escapeSize = 3;
            if(sequence[2] == 0x44)
            {
                definedTermAndCharset = std::make_pair("ISO 2022 IR 159", "ISO-IR-159");
            }
        }
    }
    else if((c1 == 0x24) && (c2 == 0x29))
    {
        // Do we still have another character in the string?
        if(sequence.size() >= 3)
        {
            escapeSize = 3;
            if(sequence[2] == 0x43) // Korean (multi-byte)
            {
                // - is this mapping really correct?
                definedTermAndCharset = std::make_pair("ISO 2022 IR 149", "EUC-KR");
            }
            else if(sequence[2] == 0x41) // Simplified Chinese (multi-byte)
            {
                // - is this mapping really correct?
                definedTermAndCharset = std::make_pair("ISO 2022 IR 58", "GB2312");
            }
        }
    }

    // Check that a definedTerm has been found
    SIGHT_THROW_IF("Unable to retrieve character set from escape sequence.", definedTermAndCharset.first.empty());

    // Check that the defined term has been declared in SpecificCharacterSet (0008,0005)
    checkDefinedTermDeclaration(definedTermAndCharset.first, definedTermList, logger);

    // Empty value treated as ASCII
    if(definedTermAndCharset.second.empty())
    {
        return sequence.substr(escapeSize);
    }
    else
    {
        return ::boost::locale::conv::to_utf<char>(sequence.substr(escapeSize), definedTermAndCharset.second);
    }
}

//------------------------------------------------------------------------------

} //namespace helper

} //namespace sight::io::dicom
