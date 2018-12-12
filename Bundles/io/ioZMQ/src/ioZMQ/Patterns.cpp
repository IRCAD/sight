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
