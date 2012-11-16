/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_EXCEPTION_BADCALL_HPP__
#define __FWCOM_EXCEPTION_BADCALL_HPP__

#include <fwCore/Exception.hpp>

#include "fwCom/config.hpp"

namespace fwCom
{

namespace exception
{

struct BadCall : ::fwCore::Exception
{
    FWCOM_API BadCall ( const std::string &err ) : ::fwCore::Exception(err) {} ;
};

} // namespace exception

} // namespace fwCom

#endif /* __FWCOM_EXCEPTION_BADCALL_HPP__ */

