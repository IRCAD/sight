/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ZMQNETWORK_EXCEPTION_HPP__
#define __ZMQNETWORK_EXCEPTION_HPP__

#include "zmqNetwork/config.hpp"

#include <fwCore/Exception.hpp>

namespace zmqNetwork
{

/// Exception for zmqNetwork
struct ZMQNETWORK_CLASS_API Exception : ::fwCore::Exception
{
    ZMQNETWORK_API Exception( const std::string& err ) : ::fwCore::Exception(err)
    {
    }
};

} // namespace zmqNetwork

#endif // __ZMQNETWORK_EXCEPTION_HPP__

