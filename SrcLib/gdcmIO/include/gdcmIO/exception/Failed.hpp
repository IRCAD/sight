/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_EXCEPTION_FAILED_HPP__
#define __GDCMIO_EXCEPTION_FAILED_HPP__

#include <fwCore/Exception.hpp>

#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace exception
{

/**
 * @brief   Implements a failed exception class for gdcmIO
 * @class   Failed
 */
struct GDCMIO_CLASS_API Failed : ::fwCore::Exception
{
    Failed( const std::string &err ) : ::fwCore::Exception(err)
    {
    }
};

} // namespace exception
} // namespace gdcmIO

#endif // __GDCMIO_EXCEPTION_FAILED_HPP__

