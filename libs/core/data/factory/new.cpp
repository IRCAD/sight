/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "data/factory/new.hpp"

#include "data/Object.hpp"

#include <core/runtime/runtime.hpp>

#include <regex>

namespace sight::data::factory
{

//------------------------------------------------------------------------------

data::Object::sptr New(const data::registry::KeyType& _classname)
{
    SIGHT_ASSERT("A classname must be specified", !_classname.empty());

    const std::string classname = core::runtime::filterID(_classname);

    // 1. Try first to create the data
    auto data = data::registry::get()->create(classname);

    // 2. If that fails, try to load the library it belongs to
    if(data == nullptr)
    {
        std::smatch match;
        static const std::regex reg("(\\w*)::(?:(core|filter|geometry|io|navigation|ui|viz)::)?(\\w*)::.*");
        if(std::regex_match(classname, match, reg) && match.size() >= 3)
        {
            const std::string libname = match[1].str() + '_' + (match[2].length() != 0 ? (match[2].str() + "_") : "")
                                        + match[3].str();
            SIGHT_DEBUG("libname: " + libname);
            const bool loaded = core::runtime::loadLibrary(libname);
            if(!loaded)
            {
                SIGHT_THROW("Cannot load library for data '" + classname + "'");
            }
        }
        else
        {
            SIGHT_THROW("Cannot determine library name from data '" + classname + "'");
        }

        // 3. Re-try now that the library is loaded
        data = data::registry::get()->create(classname);
    }

    return data;
}

} // namespace sight::data::factory
