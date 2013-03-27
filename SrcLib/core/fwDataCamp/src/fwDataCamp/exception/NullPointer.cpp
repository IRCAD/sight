/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/Exception.hpp>

#include "fwDataCamp/config.hpp"
#include "fwDataCamp/exception/NullPointer.hpp"

namespace fwDataCamp
{

namespace exception
{

NullPointer::NullPointer ( const std::string &err ) : ::fwCore::Exception(err)
{}

} // namespace exception

} // namespace fwDataCamp

