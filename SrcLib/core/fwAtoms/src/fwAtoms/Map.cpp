/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtoms/Map.hpp"
#include "fwAtoms/registry/macros.hpp"

fwAtomsRegisterMacro( ::fwAtoms::Map );

namespace fwAtoms
{

//------------------------------------------------------------------------------

Base::sptr Map::clone() const
{
    Map::sptr cloneMap = Map::New();
    for(const ValueType &elem :  m_value)
    {
        cloneMap->m_value.insert( ValueType(elem.first, elem.second->clone() ) );
    }
    return cloneMap;
}

}

