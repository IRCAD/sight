/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwNetworkIO/exceptions/ConnectionRefused.hpp"

namespace fwNetworkIO
{

namespace exceptions
{

ConnectionRefused::ConnectionRefused(const std::string& message) :
    ::fwNetworkIO::exceptions::Base(message)
{
}

ConnectionRefused::~ConnectionRefused() noexcept
{
}

} /* namespace exceptions */
} /* namespace fwNetworkIO */
