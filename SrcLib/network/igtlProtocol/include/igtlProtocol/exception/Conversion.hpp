/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IGTLPROTOCOL_EXCEPTION_CONVERSION_HPP__
#define __IGTLPROTOCOL_EXCEPTION_CONVERSION_HPP__

#include "igtlProtocol/config.hpp"

#include <fwCore/Exception.hpp>

namespace igtlProtocol
{
namespace exception
{

/// Throw this exception when a conversion between data and igtl message is not possible (error message explains the
/// reasons)
struct IGTLPROTOCOL_CLASS_API Conversion : ::fwCore::Exception
{
    IGTLPROTOCOL_API Conversion( const std::string& err ) : ::fwCore::Exception(err)
    {
    }
};

} // namespace exception
} // namespace igtlProtocol

#endif // __IGTLPROTOCOL_EXCEPTION_CONVERSION_HPP__

