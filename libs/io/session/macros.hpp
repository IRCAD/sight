/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include <io/session/session_reader.hpp>
#include <io/session/session_writer.hpp>

namespace sight::io::session
{

template<typename T>
struct SerializerRegister
{
    SerializerRegister(serializer_t _serializer, deserializer_t _deserializer)
    {
        sight::io::session::session_writer::set_serializer(T::classname(), _serializer);
        sight::io::session::session_reader::set_deserializer(T::classname(), _deserializer);
    }
};

#define SIGHT_REGISTER_SERIALIZER(dataName, serializer, deserializer) \
    static const sight::io::session::SerializerRegister<dataName> BOOST_PP_CAT(serializerRegister, __LINE__)(serializer, \
                                                                                                             deserializer);

} // namespace sight::io::session
