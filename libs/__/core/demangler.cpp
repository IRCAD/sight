/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <memory>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

#ifndef _WIN32
#include <cxxabi.h>
#include <cstdlib>
#endif

#include "core/demangler.hpp"

namespace sight::core
{

static const std::string COLONS = "::";
static const std::string LT     = "<";

//------------------------------------------------------------------------------

demangler::demangler(const std::type_info& t) :
    m_name(t.name())
{
}

//------------------------------------------------------------------------------

demangler::demangler(std::string s) :
    m_name(std::move(s))
{
}

//------------------------------------------------------------------------------

demangler::~demangler()
= default;

//------------------------------------------------------------------------------

std::string demangler::get_leaf_classname() const
{
    std::string demangled(this->demangle());

    const std::size_t lt_pos = demangled.find(LT);
    std::size_t colons_pos   = demangled.rfind(COLONS, lt_pos);

    colons_pos = (colons_pos == std::string::npos) ? 0 : colons_pos + COLONS.size();
    return demangled.replace(0, colons_pos, "");
}

//------------------------------------------------------------------------------

std::string demangler::get_classname() const
{
    std::string demangled(this->demangle());
    return demangled;
}

//------------------------------------------------------------------------------

std::string demangler::demangle() const
{
    const char* mangled = m_name.c_str();
#ifndef _WIN32
    const auto c_demangled =
        std::unique_ptr<char, decltype(std::free) *>(
            abi::__cxa_demangle(mangled, nullptr, nullptr, nullptr),
            std::free
        );
    std::string res;
    if(c_demangled != nullptr)
    {
        res = c_demangled.get();
        return res;
    }

    res = mangled;

    return res;
#else
    static std::vector<std::string> keywords;
    typedef std::vector<std::string>::iterator keyword_iterator;
    if(keywords.empty())
    {
        keywords.push_back("__cdecl");
        keywords.push_back("class ");
        keywords.push_back("enum ");
        keywords.push_back("struct ");
        keywords.push_back("union ");
    }

    std::string res(mangled);
    for(keyword_iterator iter = keywords.begin() ; iter != keywords.end() ; ++iter)
    {
        while(res.find(*iter) != std::string::npos)
        {
            res = res.replace(res.find(*iter), iter->size(), "");
        }

        while(res.find(" *") != std::string::npos)
        {
            res = res.replace(res.find(" *"), 2, "*");
        }

        while(res.find(" &") != std::string::npos)
        {
            res = res.replace(res.find(" &"), 2, "&");
        }
    }
    return res;
#endif
}

//------------------------------------------------------------------------------

} //namespace sight::core
