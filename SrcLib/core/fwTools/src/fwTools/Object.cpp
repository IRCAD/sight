/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwTools/Object.hpp"

#include "fwTools/UUID.hpp"

namespace fwTools
{

//------------------------------------------------------------------------------

Object::Object() :
    m_uuid( ::fwTools::UUID::New() )
{
}

//-----------------------------------------------------------------------------

} //namespace fwTools

