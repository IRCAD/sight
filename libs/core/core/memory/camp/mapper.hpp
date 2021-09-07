/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#pragma once

#include <core/tools/Type.hpp>

#include <camp/valuemapper.hpp>

namespace camp_ext
{

template<>
struct ValueMapper<sight::core::tools::Type>
{
    typedef sight::core::tools::Type ReturnType;
    static const int type = camp::stringType;
    //------------------------------------------------------------------------------

    static const std::string to(const ReturnType& source)
    {
        return source.string();
    }

    //------------------------------------------------------------------------------

    static ReturnType from(bool)
    {
        CAMP_ERROR(camp::BadType(camp::boolType, camp::mapType<ReturnType>()));
    }

    //------------------------------------------------------------------------------

    static ReturnType from(long)
    {
        CAMP_ERROR(camp::BadType(camp::intType, camp::mapType<ReturnType>()));
    }

    //------------------------------------------------------------------------------

    static ReturnType from(double)
    {
        CAMP_ERROR(camp::BadType(camp::realType, camp::mapType<ReturnType>()));
    }

    //------------------------------------------------------------------------------

    static ReturnType from(const std::string& source)
    {
        return sight::core::tools::Type::create(source);
    }

    //------------------------------------------------------------------------------

    static ReturnType from(const camp::EnumObject&)
    {
        CAMP_ERROR(camp::BadType(camp::enumType, camp::mapType<ReturnType>()));
    }

    //------------------------------------------------------------------------------

    static ReturnType from(const camp::UserObject&)
    {
        CAMP_ERROR(camp::BadType(camp::userType, camp::mapType<ReturnType>()));
    }
};

} // namespace camp_ext
