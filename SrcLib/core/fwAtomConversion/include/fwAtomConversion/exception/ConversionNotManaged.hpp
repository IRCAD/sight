/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMCONVERSION_EXCEPTION_CONVERSIONNOTMANAGED_HPP__
#define __FWATOMCONVERSION_EXCEPTION_CONVERSIONNOTMANAGED_HPP__

#include <fwCore/Exception.hpp>

#include "fwAtomConversion/config.hpp"

namespace fwAtomConversion
{
namespace exception
{

/// Throw this exception when a conversion between data and atom is not managed (error message explains the reasons)
struct FWATOMCONVERSION_CLASS_API ConversionNotManaged : ::fwCore::Exception
{
    FWATOMCONVERSION_API ConversionNotManaged( const std::string &err ) : ::fwCore::Exception(err) {}
};

} // namespace exception
} // namespace fwAtomConversion

#endif // __FWATOMCONVERSION_EXCEPTION_CONVERSIONNOTMANAGED_HPP__

