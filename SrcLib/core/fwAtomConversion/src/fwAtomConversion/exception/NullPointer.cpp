/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2004-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/Exception.hpp>

#include "fwAtomConversion/config.hpp"
#include "fwAtomConversion/exception/NullPointer.hpp"

namespace fwAtomConversion
{

namespace exception
{

NullPointer::NullPointer ( const std::string &err ) : ::fwCore::Exception(err)
{
}

} // namespace exception

} // namespace fwAtomConversion

