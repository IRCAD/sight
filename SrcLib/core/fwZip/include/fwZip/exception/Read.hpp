/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWZIP_EXCEPTION_READ_HPP__
#define __FWZIP_EXCEPTION_READ_HPP__

#include <fwCore/Exception.hpp>

#include "fwZip/config.hpp"

namespace fwZip
{

namespace exception
{

/// Read exception.
struct Read : ::fwCore::Exception
{
    FWZIP_API Read ( const std::string &err );
};

} // namespace exception

} // namespace fwZip

#endif /* __FWZIP_EXCEPTION_READ_HPP__ */

