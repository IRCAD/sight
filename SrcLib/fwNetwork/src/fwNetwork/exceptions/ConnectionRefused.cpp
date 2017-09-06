/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwNetwork/exceptions/ConnectionRefused.hpp"

namespace fwNetwork
{

namespace exceptions
{

ConnectionRefused::ConnectionRefused(const std::string& message) : ::fwNetwork::exceptions::Base(message)
{
}

ConnectionRefused::~ConnectionRefused() noexcept
{
}

} /* namespace exceptions */
} /* namespace fwNetwork */

