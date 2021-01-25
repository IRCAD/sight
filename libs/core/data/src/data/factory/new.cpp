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

#include "data/factory/new.hpp"

#include "data/Object.hpp"

#include <core/runtime/operations.hpp>

#include <regex>

namespace sight::data
{

namespace factory
{

//------------------------------------------------------------------------------

data::Object::sptr New( const data::registry::KeyType& classname )
{
    if(classname.empty())
    {
        return nullptr;
    }

    std::smatch match;
    static const std::regex reg("::(\\w*)::(\\w*)::.*");
    if( std::regex_match(classname, match, reg ) && match.size() == 3)
    {
        const bool loaded = core::runtime::loadLibrary(match[2].str(), match[1].str());
        if(!loaded)
        {
            return nullptr;
        }
    }
    else
    {
        FW_RAISE("Cannot determine library name from data '" + classname + "'");
    }

    return data::registry::get()->create(classname);

}

} // namespace factory

} // namespace sight::data
