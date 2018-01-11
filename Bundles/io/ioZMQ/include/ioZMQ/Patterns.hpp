/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
