/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWNETWORK_EXCEPTIONS_CONNECTIONREFUSED_HPP__
#define __FWNETWORK_EXCEPTIONS_CONNECTIONREFUSED_HPP__

#include "fwNetwork/exceptions/Base.hpp"

namespace fwNetwork
{

namespace exceptions
{

class ConnectionRefused: public fwNetwork::exceptions::Base
{
public:
    ConnectionRefused(const std::string& message);
    virtual ~ConnectionRefused() throw();
};

} /* namespace exceptions */
} /* namespace fwNetwork */

#endif /* __FWNETWORK_EXCEPTIONS_CONNECTIONREFUSED_HPP__ */

