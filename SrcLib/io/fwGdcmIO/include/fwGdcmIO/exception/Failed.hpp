/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_EXCEPTION_FAILED_HPP__
#define __FWGDCMIO_EXCEPTION_FAILED_HPP__

#include "fwGdcmIO/config.hpp"

#include <fwCore/Exception.hpp>

namespace fwGdcmIO
{
namespace exception
{

/**
 * @brief   Implements a failed exception class for fwGdcmIO
 */
struct FWGDCMIO_CLASS_API Failed : ::fwCore::Exception
{
    Failed( const std::string& err ) : ::fwCore::Exception(err)
    {
    }
};

} // namespace exception
} // namespace fwGdcmIO

#endif // __FWGDCMIO_EXCEPTION_FAILED_HPP__

