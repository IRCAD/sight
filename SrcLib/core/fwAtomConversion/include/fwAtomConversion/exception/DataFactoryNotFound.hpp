/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMCONVERSION_EXCEPTION_DATAFACTORYNOTFOUND_HPP__
#define __FWATOMCONVERSION_EXCEPTION_DATAFACTORYNOTFOUND_HPP__

#include <fwCore/Exception.hpp>

#include "fwAtomConversion/config.hpp"

namespace fwAtomConversion
{
namespace exception
{

/// Throw this exception when an object classname is not found exception in ::fwData::factory.
struct FWATOMCONVERSION_CLASS_API DataFactoryNotFound : ::fwCore::Exception
{
    FWATOMCONVERSION_API DataFactoryNotFound( const std::string &err ) : ::fwCore::Exception(err)
    {
    }
};

} // namespace exception
} // namespace fwAtomConversion

#endif // __FWATOMCONVERSION_EXCEPTION_DATAFACTORYNOTFOUND_HPP__

