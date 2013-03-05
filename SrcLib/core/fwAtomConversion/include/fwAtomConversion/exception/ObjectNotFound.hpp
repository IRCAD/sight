/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMCONVERSION_EXCEPTION_OBJECTNOTFOUND_HPP__
#define __FWATOMCONVERSION_EXCEPTION_OBJECTNOTFOUND_HPP__

#include <fwCore/Exception.hpp>

#include "fwAtomConversion/config.hpp"

namespace fwAtomConversion
{

namespace exception
{

/// Object not found exception.
struct ObjectNotFound : ::fwCore::Exception
{
    FWATOMCONVERSION_API ObjectNotFound ( const std::string &err ) ;
};

} // namespace exception

} // namespace fwAtomConversion

#endif /* __FWATOMCONVERSION_EXCEPTION_OBJECTNOTFOUND_HPP__ */

