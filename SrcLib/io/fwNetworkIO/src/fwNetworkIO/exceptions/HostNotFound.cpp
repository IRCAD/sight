/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwNetworkIO/exceptions/HostNotFound.hpp"

namespace fwNetworkIO
{

namespace exceptions
{

HostNotFound::HostNotFound(const std::string& message) :
    ::fwNetworkIO::exceptions::Base(message)
{

}

HostNotFound::~HostNotFound() noexcept
{
}

} /* namespace exceptions */
} /* namespace fwNetworkIO */
