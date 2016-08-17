/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwNetwork/exceptions/HostNotFound.hpp"

namespace fwNetwork
{

namespace exceptions
{

HostNotFound::HostNotFound(const std::string& message) : ::fwNetwork::exceptions::Base(message)
{

}

HostNotFound::~HostNotFound() throw()
{
}

} /* namespace exceptions */
} /* namespace fwNetwork */

