/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATACAMP_EXCEPTION_NULLPOINTER_HPP__
#define __FWDATACAMP_EXCEPTION_NULLPOINTER_HPP__

#include <fwCore/Exception.hpp>

#include "fwDataCamp/config.hpp"

namespace fwDataCamp
{

namespace exception
{

/// Null pointer exception.
struct NullPointer : ::fwCore::Exception
{
    FWDATACAMP_API NullPointer ( const std::string &err ) ;
};

} // namespace exception

} // namespace fwDataCamp

#endif /* __FWDATACAMP_EXCEPTION_NULLPOINTER_HPP__ */

