/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include <core/exception.hpp>
#include <core/macros.hpp>

#include <boost/locale/encoding_errors.hpp>

#include <map>
#include <string>
#include <vector>

namespace sight::core::log
{

class logger;

} // namespace sight::core::log

namespace sight::io::dicom::helper
{

/**
 * @brief Helper class for encoding management.
 */
class IO_DICOM_CLASS_API encoding
{
public:

    /**
     * @brief Convert a DICOM string from the specified charset to utf-8.
     * @param[in] source Source buffer
     * @param[in] definedCharsetTerm DICOM source charset
     * @param[in] logger Logger used for conversion errors
     * @throw core::exception
     * @throw boost::locale::conv::invalid_charset_error
     * @return converted string in utf-8 format
     */
    IO_DICOM_API static std::string convert_string(
        const std::string& _source,
        const std::string& _defined_charset_term,
        const SPTR(core::log::logger)& _logger = nullptr
    );

private:

    /**
     * @brief Convert string charset without code extension from DICOM format to boost format.
     * @param[in] source Source buffer
     * @param[in] definedTerm DICOM charset
     * @param[in] logger Logger used for conversion errors
     * @return converted charset
     */
    static std::string convert_string_without_code_extensions(
        const std::string& _source,
        const std::string& _defined_term,
        const SPTR(core::log::logger)& _logger
    );
    /**
     * @brief Convert string sequence with a code extension.
     * @param[in] sequence Sequence buffer
     * @param[in] definedTermList DICOM charset list
     * @param[in] logger Logger used for conversion errors
     * @throw core::exception
     * @throw boost::locale::conv::invalid_charset_error
     * @return converted sequence in utf-8 format
     */
    static std::string convert_sequence_with_code_extensions(
        const std::string& _sequence,
        const std::vector<std::string>& _defined_term_list,
        const SPTR(core::log::logger)& _logger
    );

    /**
     * @brief Map used to convert defined term to charset
     * @{ */
    using defined_term_to_charset_map_t = std::map<std::string, std::string>;
    static const defined_term_to_charset_map_t DEFINED_TERM_TO_CHARSET;
    /**  @} */

    /// Escape sequence representation
    using escape_sequence_t = std::pair<char, char>;

    /// Pair containing a defined term and the associated charset
    using defined_term_and_charset_pair_t = std::pair<std::string, std::string>;

    /**
     * @brief Map used to convert escape sequence to charset
     * @{ */
    using escape_sequence_to_charset_map_t = std::map<escape_sequence_t, defined_term_and_charset_pair_t>;
    static const escape_sequence_to_charset_map_t ESCAPE_SEQUENCE_TO_CHARSET;
    /**  @} */
};

} // namespace sight::io::dicom::helper
