/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/dicom/helper/encoding.hpp"

#include <core/log/logger.hpp>
#include <core/spy_log.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/locale/encoding.hpp>

namespace sight::io::dicom::helper
{

const encoding::defined_term_to_charset_map_t encoding::DEFINED_TERM_TO_CHARSET = {
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

const encoding::escape_sequence_to_charset_map_t encoding::ESCAPE_SEQUENCE_TO_CHARSET = {
    {{char(0x28), char(0x42)}, {"ISO 2022 IR 6", ""}},             // ASCII
    {{char(0x2d), char(0x41)}, {"ISO 2022 IR 100", "ISO-8859-1"}}, // Latin alphabet No. 1
    {{char(0x2d), char(0x42)}, {"ISO 2022 IR 101", "ISO-8859-2"}}, // Latin alphabet No. 2
    {{char(0x2d), char(0x43)}, {"ISO 2022 IR 109", "ISO-8859-3"}}, // Latin alphabet No. 3
    {{char(0x2d), char(0x44)}, {"ISO 2022 IR 110", "ISO-8859-4"}}, // Latin alphabet No. 4
    {{char(0x2d), char(0x4c)}, {"ISO 2022 IR 144", "ISO-8859-5"}}, // Cyrillic
    {{char(0x2d), char(0x47)}, {"ISO 2022 IR 127", "ISO-8859-6"}}, // Arabic
    {{char(0x2d), char(0x46)}, {"ISO 2022 IR 126", "ISO-8859-7"}}, // Greek
    {{char(0x2d), char(0x48)}, {"ISO 2022 IR 138", "ISO-8859-8"}}, // Hebrew
    {{char(0x2d), char(0x4d)}, {"ISO 2022 IR 148", "ISO-8859-9"}}, // Latin alphabet No. 5
    {{char(0x29), char(0x49)}, {"ISO 2022 IR 13", "JIS_X0201"}},   // Japanese { may require "ISO-IR-13"}
    {{char(0x28), char(0x4a)}, {"ISO 2022 IR 13", "ISO-IR-14"}},   // Japanese
    {{char(0x2d), char(0x54)}, {"ISO 2022 IR 166", "ISO-IR-166"}}, // Thai
    {{char(0x24), char(0x42)}, {"ISO 2022 IR 87", "ISO-IR-87"}} // Japanese { multi-byte}
};

//------------------------------------------------------------------------------

std::string encoding::convert_string(
    const std::string& _source,
    const std::string& _defined_charset_term,
    const core::log::logger::sptr& _logger
)
{
    if(_source.empty())
    {
        return "";
    }

    // Retrieve DICOM Specific Character Set List
    std::vector<std::string> defined_term_list;
    boost::split(defined_term_list, _defined_charset_term, boost::is_any_of("\\"));

    // Only one charset without code extension techniques is used
    if(_defined_charset_term.empty() || defined_term_list.size() == 1)
    {
        return convert_string_without_code_extensions(_source, _defined_charset_term, _logger);
    }

    // Several charsets with code extension techniques are used

    // If the attribute Specific Character Set (0008,0005) has more than one value
    // and value 1 is empty, it is assumed that value 1 is ISO 2022 IR 6.
    if(defined_term_list[0].empty())
    {
        defined_term_list[0] = "ISO 2022 IR 6";
    }

    // Check for characters ESC delimiter
    std::vector<std::string> sequence_list;
    boost::split(sequence_list, _source, boost::is_any_of("\033"));

    std::string result;

    // Add the first part
    if(_source[0] != '\033')
    {
        result += convert_string_without_code_extensions(sequence_list[0], defined_term_list[0], _logger);
    }
    else
    {
        result += encoding::convert_sequence_with_code_extensions(sequence_list[0], defined_term_list, _logger);
    }

    // Convert remaining sequences according to specific charsets
    auto it = ++sequence_list.begin();
    for( ; it != sequence_list.end() ; ++it)
    {
        result += convert_sequence_with_code_extensions(*it, defined_term_list, _logger);
    }

    return result;
}

//------------------------------------------------------------------------------

std::string encoding::convert_string_without_code_extensions(
    const std::string& _source,
    const std::string& _defined_term,
    const core::log::logger::sptr& _logger
)
{
    std::string charset;

    if(_defined_term.empty()) // assuming ASCII (according to DICOM PS 3.5)
    {
        charset = "";
    }
    else
    {
        SIGHT_WARN_IF(
            "'ISO_IR 6' is not a defined term in DICOM, will be treated as an empty value (ASCII)",
            _defined_term == "ISO_IR 6"
        );

        // Check that the defined term is known
        if(DEFINED_TERM_TO_CHARSET.find(_defined_term) != DEFINED_TERM_TO_CHARSET.end())
        {
            charset = DEFINED_TERM_TO_CHARSET.at(_defined_term);
        }
        else
        {
            const std::string msg = "'" + _defined_term + "' is not a defined term in DICOM, "
                                                          "will be treated as an empty value (ASCII)";

            SIGHT_WARN_IF(msg, !_logger);
            if(_logger)
            {
                _logger->warning(msg);
            }

            charset = "";
        }
    }

    // Empty value treated as ASCII
    if(charset.empty())
    {
        return _source;
    }

    return boost::locale::conv::to_utf<char>(_source, charset);
}

//------------------------------------------------------------------------------

void check_defined_term_declaration(
    const std::string& _defined_term,
    const std::vector<std::string>& _defined_term_list,
    const core::log::logger::sptr& _logger
)
{
    if(std::find(_defined_term_list.begin(), _defined_term_list.end(), _defined_term) == _defined_term_list.end())
    {
        const std::string msg = "Escape sequence refers to character set '" + _defined_term
                                + "' that was not declared in SpecificCharacterSet (0008,0005).";

        SIGHT_WARN_IF(msg, !_logger);
        if(_logger)
        {
            _logger->warning(msg);
        }
    }
}

//------------------------------------------------------------------------------

std::string encoding::convert_sequence_with_code_extensions(
    const std::string& _sequence,
    const std::vector<std::string>& _defined_term_list,
    const core::log::logger::sptr& _logger
)
{
    // We need at least two more characters to determine the new character set
    SIGHT_THROW_IF("Cannot convert character set: Incomplete escape sequence.", _sequence.size() < 2);

    const char c1 = _sequence[0];
    const char c2 = _sequence[1];

    std::uint16_t escape_size = 2;

    escape_sequence_t escape_sequence                        = std::make_pair(c1, c2);
    defined_term_and_charset_pair_t defined_term_and_charset = std::make_pair("", "");

    if(auto it = ESCAPE_SEQUENCE_TO_CHARSET.find(escape_sequence); it != ESCAPE_SEQUENCE_TO_CHARSET.end())
    {
        defined_term_and_charset = it->second;
    }
    else if((c1 == 0x24) && (c2 == 0x28)) // Japanese (multi-byte)
    {
        // Do we still have another character in the string?
        if(_sequence.size() >= 3)
        {
            escape_size = 3;
            if(_sequence[2] == 0x44)
            {
                defined_term_and_charset = std::make_pair("ISO 2022 IR 159", "ISO-IR-159");
            }
        }
    }
    else if((c1 == 0x24) && (c2 == 0x29))
    {
        // Do we still have another character in the string?
        if(_sequence.size() >= 3)
        {
            escape_size = 3;
            if(_sequence[2] == 0x43) // Korean (multi-byte)
            {
                // - is this mapping really correct?
                defined_term_and_charset = std::make_pair("ISO 2022 IR 149", "EUC-KR");
            }
            else if(_sequence[2] == 0x41) // Simplified Chinese (multi-byte)
            {
                // - is this mapping really correct?
                defined_term_and_charset = std::make_pair("ISO 2022 IR 58", "GB2312");
            }
        }
    }

    // Check that a definedTerm has been found
    SIGHT_THROW_IF("Unable to retrieve character set from escape sequence.", defined_term_and_charset.first.empty());

    // Check that the defined term has been declared in SpecificCharacterSet (0008,0005)
    check_defined_term_declaration(defined_term_and_charset.first, _defined_term_list, _logger);

    // Empty value treated as ASCII
    if(defined_term_and_charset.second.empty())
    {
        return _sequence.substr(escape_size);
    }

    return boost::locale::conv::to_utf<char>(_sequence.substr(escape_size), defined_term_and_charset.second);
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
