/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwNetwork/exceptions/ContentNotFound.hpp"

namespace fwNetwork
{

namespace exceptions
{

ContentNotFound::ContentNotFound(const std::string& message) : ::fwNetwork::exceptions::Base(message)
{

}

ContentNotFound::~ContentNotFound() throw()
{
}

} /* namespace exceptions */
} /* namespace fwNetwork */

