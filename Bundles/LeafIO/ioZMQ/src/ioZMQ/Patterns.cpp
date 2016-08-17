/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioZMQ/Patterns.hpp"

namespace ioZMQ
{

const Patterns::SupportedPatternMapType Patterns::s_LISTENER_PATTERNS = Patterns::initializeReaderPatterns();
const Patterns::SupportedPatternMapType Patterns::s_SENDER_PATTERNS   = Patterns::initializeWriterPatterns();

//-----------------------------------------------------------------------------

Patterns::Patterns()
{
}

//-----------------------------------------------------------------------------

Patterns::~Patterns()
{
}

//-----------------------------------------------------------------------------

Patterns::SupportedPatternMapType Patterns::initializeReaderPatterns()
{
    SupportedPatternMapType readerPatterns;

    readerPatterns["Pull"]      = ::zmqNetwork::Socket::Pull;
    readerPatterns["Subscribe"] = ::zmqNetwork::Socket::Subscribe;
    readerPatterns["Request"]   = ::zmqNetwork::Socket::Request;
    return readerPatterns;
}

//-----------------------------------------------------------------------------

Patterns::SupportedPatternMapType Patterns::initializeWriterPatterns()
{
    SupportedPatternMapType writerPatterns;

    writerPatterns["Push"]    = ::zmqNetwork::Socket::Push;
    writerPatterns["Publish"] = ::zmqNetwork::Socket::Publish;
    writerPatterns["Reply"]   = ::zmqNetwork::Socket::Reply;
    return writerPatterns;
}

//-----------------------------------------------------------------------------

const Patterns::SupportedPatternMapType& Patterns::getSupportedReaderPatterns()
{
    return Patterns::s_LISTENER_PATTERNS;
}

//-----------------------------------------------------------------------------

const Patterns::SupportedPatternMapType& Patterns::getSupportedWriterPatterns()
{
    return Patterns::s_SENDER_PATTERNS;
}

//-----------------------------------------------------------------------------

} // namespace ioZMQ
