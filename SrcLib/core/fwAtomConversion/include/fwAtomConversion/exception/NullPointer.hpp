/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2004-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMCONVERSION_EXCEPTION_NULLPOINTER_HPP__
#define __FWATOMCONVERSION_EXCEPTION_NULLPOINTER_HPP__

#include <fwCore/Exception.hpp>

#include "fwAtomConversion/config.hpp"

namespace fwAtomConversion
{

namespace exception
{

/// Null pointer exception.
struct NullPointer : ::fwCore::Exception
{
    FWATOMCONVERSION_API NullPointer ( const std::string &err );
};

} // namespace exception

} // namespace fwAtomConversion

#endif /* __FWATOMCONVERSION_EXCEPTION_NULLPOINTER_HPP__ */

