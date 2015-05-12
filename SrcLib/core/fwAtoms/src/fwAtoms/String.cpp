/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtoms/registry/macros.hpp"
#include "fwAtoms/String.hpp"

fwAtomsRegisterMacro( ::fwAtoms::String );

namespace fwAtoms
{

//------------------------------------------------------------------------------

String::sptr String::New(std::string value)
{
    String::sptr std = String::New();
    std->m_value = value;
    return std;
}

//------------------------------------------------------------------------------

Base::sptr String::clone() const
{
    return String::New(m_value);
}

}

