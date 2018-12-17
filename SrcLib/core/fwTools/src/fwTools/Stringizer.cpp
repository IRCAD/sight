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

#ifndef WIN32
#include <cxxabi.h>
#include <stdlib.h>
#endif

#include "fwTools/Stringizer.hpp"


namespace fwTools
{

//------------------------------------------------------------------------------

template<>
std::string getString(const std::string &aString)
{
    return aString;
}

//------------------------------------------------------------------------------

template<>
std::string getString(const std::type_info &ti)
{
#ifndef WIN32
    char *demangled = abi::__cxa_demangle(ti.name(),0,0,0);
    if (demangled)
    {
        std::string res(demangled);
        free(demangled);
        return res;
    }
    else
    {
        return ti.name();
    }
#else
    return ti.name();
#endif
}

//------------------------------------------------------------------------------

template<>
std::string getString(const TypeInfo &ti)
{
    return getString(ti.type_info());
}

//------------------------------------------------------------------------------

template<>
std::string getString(const signed char &c)
{
    return boost::lexical_cast<std::string>((short)c);
}

//------------------------------------------------------------------------------

template<>
std::string getString(const char &c)
{
    return boost::lexical_cast<std::string>((short)c);
}

//------------------------------------------------------------------------------

template<>
std::string getString(const unsigned char &c)
{
    return boost::lexical_cast<std::string>((short)c);
}

//------------------------------------------------------------------------------

}
