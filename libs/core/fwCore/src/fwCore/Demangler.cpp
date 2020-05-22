/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include <string>
#include <vector>
#include <typeinfo>

#ifndef _WIN32
#include <cxxabi.h>
#include <cstdlib>
#endif

#include "fwCore/Demangler.hpp"

namespace fwCore
{

#define COLONS std::string("::")
#define LT std::string("<")

//------------------------------------------------------------------------------

Demangler::Demangler(const std::type_info& t) :
    m_name(t.name())
{
}

//------------------------------------------------------------------------------

Demangler::Demangler(const std::string& s) :
    m_name(s)
{
}

//------------------------------------------------------------------------------

Demangler::~Demangler()
{
}

//------------------------------------------------------------------------------

std::string Demangler::getLeafClassname() const
{
    std::string demangled(this->demangle());

    const size_t lt_pos = demangled.find(LT);
    size_t colons_pos   = demangled.rfind(COLONS, lt_pos);

    colons_pos = (colons_pos == std::string::npos) ? 0 : colons_pos+COLONS.size();
    return demangled.replace(0, colons_pos, "");
}

//------------------------------------------------------------------------------

std::string Demangler::getClassname() const
{
    const std::string demangled(this->demangle());
    return COLONS + demangled;
}

//------------------------------------------------------------------------------

std::string Demangler::demangle() const
{
    const char* mangled = m_name.c_str();
#ifndef _WIN32
    char* c_demangled = abi::__cxa_demangle( mangled, 0, 0, 0);
    std::string res;
    if (c_demangled)
    {
        res = c_demangled;
        free(c_demangled);
        return res;
    }
    else
    {
        res = mangled;
    }
    return res;
#else
    static std::vector<std::string> keywords;
    typedef std::vector<std::string>::iterator keyword_iterator;
    if ( keywords.empty() )
    {
        keywords.push_back("__cdecl");
        keywords.push_back("class ");
        keywords.push_back("enum ");
        keywords.push_back("struct ");
        keywords.push_back("union ");
    }
    std::string res(mangled);
    for (keyword_iterator iter = keywords.begin(); iter != keywords.end(); ++iter )
    {
        while (res.find(*iter) != std::string::npos)
        {
            res = res.replace(res.find(*iter), iter->size(), "");
        }
        while (res.find(" *") != std::string::npos)
        {
            res = res.replace(res.find(" *"), 2, "*");
        }
        while (res.find(" &") != std::string::npos)
        {
            res = res.replace(res.find(" &"), 2, "&");
        }
    }
    return res;
#endif
}

//------------------------------------------------------------------------------

#undef COLONS
#undef LT

} //namespace fwCore
