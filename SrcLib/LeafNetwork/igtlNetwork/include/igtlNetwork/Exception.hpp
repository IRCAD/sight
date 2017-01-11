/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IGTLNETWORK_EXCEPTION_HPP__
#define __IGTLNETWORK_EXCEPTION_HPP__

#include "igtlNetwork/config.hpp"

#include <fwCore/Exception.hpp>

namespace igtlNetwork
{

/// Exception for igtlNetwork
struct IGTLNETWORK_CLASS_API Exception : ::fwCore::Exception
{
    IGTLNETWORK_API Exception( const std::string& err ) : ::fwCore::Exception(err)
    {
    }
};

} // namespace igtlProtocol

#endif // __IGTLNETWORK_EXCEPTION_HPP__

