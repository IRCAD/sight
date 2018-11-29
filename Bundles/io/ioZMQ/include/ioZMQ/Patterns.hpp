/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#ifndef __IOZMQ_PATTERNS_HPP__
#define __IOZMQ_PATTERNS_HPP__

#include "ioZMQ/config.hpp"

#include <zmqNetwork/Socket.hpp>

#include <map>
#include <string>


namespace ioZMQ
{

/**
 *
 * @brief class only contains supported pattern for writer and reader
 */
class IOZMQ_CLASS_API Patterns
{
public:
    typedef std::map<std::string, ::zmqNetwork::Socket::PatternMode> SupportedPatternMapType;
    /**
     * @brief initialize zeromq reader pattern (ex : PULL, SUBSCRIBE, ...)
     */
    IOZMQ_API static SupportedPatternMapType initializeReaderPatterns();
    /**
     * @brief initialize zeromq writer pattern (ex : PUSH, PUBLISH, ...)
     */
    IOZMQ_API static SupportedPatternMapType initializeWriterPatterns();

    /**
     * @brief get the supported reader pattern
     *
     * @return the reader pattern pair of std::string, PatternMode
     */
    IOZMQ_API static const SupportedPatternMapType& getSupportedReaderPatterns();

    /**
     * @brief get the supported writer pattern
     *
     * @return the writer pattern pair of std::string, PatternMode
     */
    IOZMQ_API static const SupportedPatternMapType& getSupportedWriterPatterns();


private:
    Patterns();
    ~Patterns();

    /// Available listener pattern map
    static const SupportedPatternMapType s_LISTENER_PATTERNS;

    /// Available sender pattern map
    static const SupportedPatternMapType s_SENDER_PATTERNS;
};
} // namespace ioZMQ

#endif // __IOZMQ_PATTERNS_HPP__
