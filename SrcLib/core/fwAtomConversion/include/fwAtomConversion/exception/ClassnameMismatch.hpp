/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMCONVERSION_EXCEPTION_CLASSNAMEMISMATCH_HPP__
#define __FWATOMCONVERSION_EXCEPTION_CLASSNAMEMISMATCH_HPP__

#include <fwCore/Exception.hpp>

#include "fwAtomConversion/config.hpp"

namespace fwAtomConversion
{
namespace exception
{

/// Throw this exception when a new data is created with a specific uuid, but this data already exists in system
struct FWATOMCONVERSION_CLASS_API ClassnameMismatch : ::fwCore::Exception
{
    FWATOMCONVERSION_API ClassnameMismatch( const std::string &err ) : ::fwCore::Exception(err)
    {
    }
};

} // namespace exception
} // namespace fwAtomConversion

#endif // __FWATOMCONVERSION_EXCEPTION_CLASSNAMEMISMATCH_HPP__

