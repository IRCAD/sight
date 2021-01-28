/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include <atoms/Blob.hpp>

#include <core/memory/BufferObject.hpp>

#include <camp/valuemapper.hpp>

namespace camp_ext
{

/// New value mapper to manage conversion in camp world between sight::atoms::Blob and a core::memory::BufferObject
template <>
struct ValueMapper< sight::atoms::Blob::sptr >
{
    typedef sight::atoms::Blob::sptr ReturnType;

    static const int type = camp::userType;

    //------------------------------------------------------------------------------

    static const sight::core::memory::BufferObject::sptr to(const ReturnType& source)
    {
        return source->getBufferObject();
    }

    //------------------------------------------------------------------------------

    static ReturnType from(bool source)
    {
        CAMP_ERROR(camp::BadType(camp::boolType, camp::mapType<ReturnType>()));
    }

    //------------------------------------------------------------------------------

    static ReturnType from(long source)
    {
        CAMP_ERROR(camp::BadType(camp::intType, camp::mapType<ReturnType>()));
    }

    //------------------------------------------------------------------------------

    static ReturnType from(double source)
    {
        CAMP_ERROR(camp::BadType(camp::realType, camp::mapType<ReturnType>()));
    }

    //------------------------------------------------------------------------------

    static ReturnType from(const std::string& source)
    {
        CAMP_ERROR(camp::BadType(camp::realType, camp::mapType<ReturnType>()));
    }

    //------------------------------------------------------------------------------

    static ReturnType from(const camp::EnumObject& source)
    {
        CAMP_ERROR(camp::BadType(camp::enumType, camp::mapType<ReturnType>()));
    }

    //------------------------------------------------------------------------------

    static ReturnType from(const camp::UserObject& source)
    {
        sight::core::memory::BufferObject::sptr tmp = source.get< sight::core::memory::BufferObject::sptr>()->getSptr();
        return sight::atoms::Blob::New(tmp);
    }
};

}
