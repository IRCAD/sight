/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtoms/registry/macros.hpp"
#include "fwAtoms/Boolean.hpp"

fwAtomsRegisterMacro( ::fwAtoms::Boolean );

namespace fwAtoms
{

//------------------------------------------------------------------------------

Boolean::sptr Boolean::New(std::string value)
{
    Boolean::sptr valueSptr = Boolean::New();
    valueSptr->m_value = (value.compare("true") == 0);
    return valueSptr;
}

//------------------------------------------------------------------------------

Boolean::sptr Boolean::New(bool value)
{
    Boolean::sptr valueSptr = Boolean::New();
    valueSptr->m_value = value;
    return valueSptr;
}

//------------------------------------------------------------------------------

std::string Boolean::getString() const
{
    return m_value ? "true" : "false";
}

//------------------------------------------------------------------------------

void Boolean::setString(const std::string& value)
{
    m_value = (!value.compare("true"));
}

//------------------------------------------------------------------------------

Base::sptr Boolean::clone() const
{
    return Boolean::New(m_value);
}

}

