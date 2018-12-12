/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#ifndef __FWGDCMIO_HELPER_ENCODING_HPP__
#define __FWGDCMIO_HELPER_ENCODING_HPP__

#include "fwGdcmIO/config.hpp"

#include <fwCore/Exception.hpp>
#include <fwCore/macros.hpp>

#include <boost/locale/encoding_errors.hpp>

#include <map>
#include <string>
#include <vector>

namespace fwLog
{
class Logger;
}

namespace fwGdcmIO
{
namespace helper
{

/**
 * @brief Helper class for encoding management.
 */
class FWGDCMIO_CLASS_API Encoding
{
public:

    /**
     * @brief Convert a DICOM string from the specified charset to utf-8.
     * @param[in] source Source buffer
     * @param[in] definedCharsetTerm DICOM source charset
     * @param[in] logger Logger used for conversion errors
     * @throw ::fwCore::Exception
     * @throw ::boost::locale::conv::invalid_charset_error
     * @return converted string in utf-8 format
     */
    FWGDCMIO_API static std::string convertString(const std::string& source,
                                                  const std::string& definedCharsetTerm,
                                                  const SPTR(::fwLog::Logger)& logger = nullptr)
    throw(::fwCore::Exception, ::boost::locale::conv::invalid_charset_error);

private:

    /**
     * @brief Convert string charset without code extension from DICOM format to boost format.
     * @param[in] source Source buffer
     * @param[in] definedTerm DICOM charset
     * @param[in] logger Logger used for conversion errors
     * @return converted charset
     */
    static std::string convertStringWithoutCodeExtensions(const std::string& source,
                                                          const std::string& definedTerm,
                                                          const SPTR(::fwLog::Logger)& logger);
    /**
     * @brief Convert string sequence with a code extension.
     * @param[in] sequence Sequence buffer
     * @param[in] definedTermList DICOM charset list
     * @param[in] logger Logger used for conversion errors
     * @throw ::fwCore::Exception
     * @throw ::boost::locale::conv::invalid_charset_error
     * @return converted sequence in utf-8 format
     */
    static std::string convertSequenceWithCodeExtensions(const std::string& sequence,
                                                         const std::vector<std::string>& definedTermList,
                                                         const SPTR(::fwLog::Logger)& logger)
    throw(::fwCore::Exception, ::boost::locale::conv::invalid_charset_error);

    /**
     * @brief Map used to convert defined term to charset
     * @{ */
    typedef std::map< std::string, std::string> DefinedTermToCharsetMapType;
    static const DefinedTermToCharsetMapType s_DEFINED_TERM_TO_CHARSET;
    /**  @} */

    /// Escape sequence representation
    typedef std::pair< char, char> EscapeSequenceType;

    /// Pair containing a defined term and the associated charset
    typedef std::pair< std::string, std::string > DefinedTermAndCharsetPairType;

    /**
     * @brief Map used to convert escape sequence to charset
     * @{ */
    typedef std::map< EscapeSequenceType, DefinedTermAndCharsetPairType>  EscapeSequenceToCharsetMapType;
    static const EscapeSequenceToCharsetMapType s_ESCAPE_SEQUENCE_TO_CHARSET;
    /**  @} */

};

} // namespace helper
} // namespace fwGdcmIO

#endif //__FWGDCMIO_HELPER_ENCODING_HPP__

