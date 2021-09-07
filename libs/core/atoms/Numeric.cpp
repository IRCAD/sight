/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "atoms/Numeric.hpp"

#include "atoms/Exception.hpp"
#include "atoms/Numeric.hxx"
#include "atoms/registry/macros.hpp"

#include <core/spyLog.hpp>

#include <boost/lexical_cast.hpp>

fwAtomsRegisterMacro(sight::atoms::Numeric);

namespace sight::atoms
{

//------------------------------------------------------------------------------

Base::sptr Numeric::clone() const
{
    Numeric::sptr clone = Numeric::New();
    clone->m_value = m_value;
    return clone;
}

//------------------------------------------------------------------------------

std::string Numeric::getString() const
{
    SIGHT_THROW_EXCEPTION_IF(atoms::Exception("Empty numeric atom"), m_value.which() == 0);
    return ::boost::lexical_cast<std::string>(m_value);
}

//------------------------------------------------------------------------------

template<typename T>
bool lexicalCast(Numeric::ValueType& v, const std::string& s)
{
    bool ok = false;

    try
    {
        T val = ::boost::lexical_cast<T>(s);
        v  = val;
        ok = true;
    }
    catch(const ::boost::bad_lexical_cast&)
    {
    }
    return ok;
}

//------------------------------------------------------------------------------

Numeric::ValueType Numeric::valueFromString(const std::string& s, Numeric::NumericType type)
{
    Numeric::ValueType res;
    SIGHT_ASSERT("Invalid variant type requested", EMPTY <= type && type <= DOUBLE);

    if(type == EMPTY)
    {
        SIGHT_THROW_EXCEPTION_IF(
            atoms::Exception(std::string("Unable to get numeric from '") + s + "'"),
            !(lexicalCast<std::int64_t>(res, s)
              || lexicalCast<std::uint64_t>(res, s)
              || lexicalCast<double>(res, s))
        );
    }
    else if(type == INT)
    {
        SIGHT_THROW_EXCEPTION_IF(
            atoms::Exception(std::string("Unable to get int64 numeric from '") + s + "'"),
            !lexicalCast<std::int64_t>(res, s)
        );
    }
    else if(type == UINT)
    {
        SIGHT_THROW_EXCEPTION_IF(
            atoms::Exception(std::string("Unable to get uint64 numeric from '") + s + "'"),
            !lexicalCast<std::uint64_t>(res, s)
        );
    }
    else if(type == DOUBLE)
    {
        SIGHT_THROW_EXCEPTION_IF(
            atoms::Exception(std::string("Unable to get double numeric from '") + s + "'"),
            !lexicalCast<double>(res, s)
        );
    }

    return res;
}

//------------------------------------------------------------------------------

void Numeric::setFromString(const std::string& s, Numeric::NumericType type)
{
    m_value = Numeric::valueFromString(s, type);
}

} // namespace sight::atoms
