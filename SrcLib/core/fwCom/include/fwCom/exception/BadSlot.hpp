/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_EXCEPTION_BADSLOT_HPP__
#define __FWCOM_EXCEPTION_BADSLOT_HPP__

#include <fwCore/Exception.hpp>

#include "fwCom/config.hpp"

namespace fwCom
{

namespace exception
{

/// Bad slot exception.
struct BadSlot : ::fwCore::Exception
{
    FWCOM_API BadSlot ( const std::string &err );
};

} // namespace exception

} // namespace fwCom

#endif /* __FWCOM_EXCEPTION_BADSLOT_HPP__ */

