/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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

