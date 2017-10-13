/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_HELPER_CSVIO_HPP__
#define __FWGDCMIO_HELPER_CSVIO_HPP__

#include "fwGdcmIO/config.hpp"

#include <iostream>
#include <string>
#include <vector>


namespace fwGdcmIO
{

namespace helper
{

/**
 * @brief Read CSV file and returns parsed tokens.
 * The input file is supposed to use comma separator, but another separator can be used when reading file.
 */
FWGDCMIO_CLASS_API class CsvIO
{

public:

    /**
     * @brief Constructor
     *
     * @param csvStream the stream to read
     */
    FWGDCMIO_API CsvIO(std::istream& csvStream);

    /// Desctructor
    FWGDCMIO_API virtual ~CsvIO();

    /// Containers to store parsed tokens.
    typedef std::vector< std::string > TokenContainerType;

    /**
     * @brief Returns tokens on next line to read, using comma separator
     *
     * Other sperators can be used
     * @param separator torken separator
     *
     * @return tokens found on next line separated with given characters
     */
    FWGDCMIO_API TokenContainerType getLine(const std::string& separator = ",");

protected:

    /// Associated stream
    std::istream& m_stream;
};

} // namespace helper
} // namespace fwGdcmIO


#endif // __FWGDCMIO_HELPER_CSVIO_HPP__

