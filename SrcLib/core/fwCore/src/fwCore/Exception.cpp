/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwCore/Exception.hpp"

namespace fwCore
{

Exception::Exception ( const std::string& err ) :
    std::runtime_error( err )
{
}

} //namespace fwCore

